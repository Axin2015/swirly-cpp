/*
 *  Copyright (C) 2013, 2014 Mark Aylett <mark.aylett@gmail.com>
 *
 *  This file is part of Doobry written by Mark Aylett.
 *
 *  Doobry is free software; you can redistribute it and/or modify it under the terms of the GNU
 *  General Public License as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  Doobry is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 *  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with this program; if
 *  not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA.
 */

#include "plugin.h"

#include "log.h"
#include "buffer.h"
#include "response.h"

#include <string.h> // strncmp()

/*
  plugin_config
  =============

  Every plugin can opt-in to handle conditionals from the global configfile and store the different
  settings in a by-condition plugin_config. This struct is linked in the next plugin-wide struct:
  plugin_data.
*/

typedef struct {
	array* matchuri;
} plugin_config;

/*
  plugin_data
  ===========

  Each plugin has some local data which only belongs to the plugin (like the config and some
  temporary buffers).
*/

typedef struct {
	PLUGIN_DATA;

	buffer* query_str;
	array* get_params;

	plugin_config** config_storage;
	plugin_config conf;
} plugin_data;

/*
  handler_ctx
  ===========

  If a plugin has to store connection specific information, you need another structure with one
  entry per connection. Take a look at mod_rewrite as an example.
 */

typedef struct {
	size_t foo;
} handler_ctx;

static handler_ctx*
handler_ctx_init()
{
	handler_ctx*  hctx;
	hctx = calloc(1, sizeof(*hctx));
	return hctx;
}

static void
handler_ctx_free(handler_ctx* hctx)
{
	free(hctx);
}

/*
  conditionals
  ============

  If you are interested in conditonals, you need some more work as you have to call
  config_insert_values_global() once per condition. Look at mod_access.c to see how that works.

  You need a config_storage for all the conditionals and a patch function.

  The patch function sets the basic defaults and applies the necessary modifications for the
  currently valid conditionals.

  Don't forget to check if the patched config makes sense when you use it.

  The patch functions have to be called as soon as one of the work-handlers is called (like
  uri_handler and friends).

    patch_connection(srv, con, p);
*/

#define PATCH(x) \
	p->conf.x = s->x;

static int
patch_connection(server* srv, connection* con, plugin_data* p)
{
	size_t i, j;
	plugin_config* s = p->config_storage[0];

	PATCH(matchuri);

	// Skip the first, the global context.
	for (i = 1; i < srv->config_context->used; i++) {

		data_config* dc = (data_config*)srv->config_context->data[i];
		s = p->config_storage[i];

		// Condition didn't match.
		if (!config_check_cond(srv, con, dc))
            continue;

		// Merge config.
		for (j = 0; j < dc->value->used; j++) {
			data_unset* du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("doobry.array"))) {
				PATCH(matchuri);
			}
		}
	}

	return 0;
}

#undef PATCH

static int split_get_params(array* get_params, buffer* qrystr) {
	size_t is_key = 1;
	size_t i;
	char* key = NULL, * val = NULL;

	key = qrystr->ptr;

	/* we need the \0 */
	for (i = 0; i < qrystr->used; i++) {
		switch(qrystr->ptr[i]) {
		case '=':
			if (is_key) {
				val = qrystr->ptr + i + 1;
				qrystr->ptr[i] = '\0';
				is_key = 0;
			}
			break;
		case '&':
		case '\0': /* fin symbol */
			if (!is_key) {
				data_string* ds;
				/* we need at least a = since the last & */

				/* terminate the value */
				qrystr->ptr[i] = '\0';

				if (NULL == (ds = (data_string*)array_get_unused_element(get_params,
                                                                         TYPE_STRING))) {
					ds = data_string_init();
				}
				buffer_copy_string_len(ds->key, key, strlen(key));
				buffer_copy_string_len(ds->value, val, strlen(val));
				array_insert_unique(get_params, (data_unset*)ds);
			}

			key = qrystr->ptr + i + 1;
			val = NULL;
			is_key = 1;
			break;
		}
	}
	return 0;
}

/*
  init
  ====

  The init function is called to initialize the plugin itself and returns the plugin_data struct
  with some useful defaults.
*/

static void*
mod_doobry_init()
{
	plugin_data* p;
	p = calloc(1, sizeof(*p));
	p->query_str = buffer_init();
	p->get_params = array_init();
	return p;
}

/*
  free
  ====

  The free function is called at the end of the life of a plugin and is used to tell the plugin to
  release all allocated memory. Keep in mind that it is preferred not to let the program termination
  clean up memory for you. Free what ever you have malloced. Use valgrind or something else to
  verify your work.
*/

static handler_t
mod_doobry_free(server* srv, void* p_d)
{
	plugin_data* p = p_d;
	if (!p)
        return HANDLER_GO_ON;

	if (p->config_storage) {
		size_t i;
		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config* s = p->config_storage[i];
			if (!s)
                continue;
			array_free(s->matchuri);
			free(s);
		}
		free(p->config_storage);
	}
	buffer_free(p->query_str);
	array_free(p->get_params);
	free(p);
	return HANDLER_GO_ON;
}

/*
  set_defaults
  ============

  As soon as the configfile is parsed, each plugin gets the chance to fetch its config-values from
  the config and verify it internally. A config_values_t struct contains the config-key and the type
  you expect. Don't forget that the last entry of the struct should be NULL to mark the end.

  If you don't care about conditionals, the set_defaults function is quite simple:

  - set the destinations of the config_values_t
  - call config_insert_values_global()
*/

static handler_t
mod_doobry_set_defaults(server* srv, void* p_d)
{
	plugin_data* p = p_d;
	size_t i = 0;

    /*
      Example config:

      doobry.matchuri = (
        "foo",
        "bar"
      )
    */
	config_values_t cv[] = {
		{ "doobry.matchuri",  NULL, T_CONFIG_ARRAY, T_CONFIG_SCOPE_CONNECTION },
		{ NULL, NULL, T_CONFIG_UNSET, T_CONFIG_SCOPE_UNSET }
	};

	if (!p)
        return HANDLER_ERROR;

	p->config_storage = calloc(1, srv->config_context->used * sizeof(plugin_config*));

	for (i = 0; i < srv->config_context->used; i++) {
		plugin_config* s;

		s = calloc(1, sizeof(plugin_config));
		s->matchuri = array_init();

		cv[0].destination = s->matchuri;
		p->config_storage[i] = s;

		if (0 != config_insert_values_global(srv, ((data_config*)srv
                                                   ->config_context->data[i])->value, cv)) {
			return HANDLER_ERROR;
		}
	}
	return HANDLER_GO_ON;
}

/*
  return values
  =============

  In most cases, you will only work with HANDLER_GO_ON, HANDLER_FINISHED and HANDLER_ERROR.

  HANDLER_GO_ON is returned in most of the cases when you want another plugin to have a chance to
  touch the request too. If you know that this request is not for you, just return HANDLER_GO_ON and
  you are done.

  HANDLER_ERROR should only be called if something fatal happens as it terminates the current
  connection if called in one of the handlers or terminates lighttpd if called in set_defaults.

  HANDLER_FINISHED is the final return-code used to signal that everything is ready to serve the
  request like:

  - a non-200 status-code has been set;
  - content has been generated you everything is in place.

  HANDLER_WAIT_FOR_EVENT and HANDLER_WAIT_FOR_FD should be returned if you are not finished yet and
  have to wait for an fd-event or if you ran out of FDs.

  HANDLER_COMEBACK is useful if you want to re-check the request-structure. This is used in
  mod_rewrite to handle the rewritten URI.
*/

static handler_t
mod_doobry_handle_uri(server* srv, connection* con, void* p_d)
{
	plugin_data* p = p_d;
	int s_len;
	size_t k, i;

	// Go on if another module has handled this request.
	if (con->mode != DIRECT)
        return HANDLER_GO_ON;

	if (con->uri.path->used == 0)
        return HANDLER_GO_ON;

	patch_connection(srv, con, p);

	s_len = con->uri.path->used - 1;

	for (k = 0; k < p->conf.matchuri->used; k++) {
		data_string* ds = (data_string*)p->conf.matchuri->data[k];
		int ct_len = ds->value->used - 1;

		if (ct_len > s_len)
            continue;

		if (ds->value->used == 0)
            continue;

        // Match tail of URI path with value.
		if (0 == strncmp(con->uri.path->ptr + s_len - ct_len, ds->value->ptr, ct_len)) {

			array_reset(p->get_params);
			buffer_copy_string_buffer(p->query_str, con->uri.query);
			split_get_params(p->get_params, p->query_str);

            buffer* b = chunkqueue_get_append_buffer(con->write_queue);
            buffer_append_string_len(b, CONST_STR_LEN("Hello, World\n"));

            data_string* foo = (data_string*)array_get_element(p->get_params, "foo");
			if (foo) {
                buffer_append_string_buffer(b, foo->value);
                buffer_append_string_len(b, CONST_STR_LEN("\n"));
			}

            response_header_overwrite(srv, con, CONST_STR_LEN("Content-Type"),
                                      CONST_STR_LEN("text/plain"));
            con->file_finished = 1;

            con->http_status = 200;
            return HANDLER_FINISHED;
		}
	}

	// Not found.
	return HANDLER_GO_ON;
}

/*
  plugin_init
  ===========

  Each plugin has to have a plugin_init function which is called when the plugin is called. It
  should only set p->name to a buffer containing the name of the plugin (human readable) and should
  fill the hooks (init, set_defaults, cleanup) and return with 0;
*/

int
mod_doobry_plugin_init(plugin* p)
{
	p->version = LIGHTTPD_VERSION_ID;
	p->name = buffer_init_string("doobry");
	p->init = mod_doobry_init;
	p->cleanup = mod_doobry_free;
	p->set_defaults = mod_doobry_set_defaults;
	p->handle_uri_clean = mod_doobry_handle_uri;
	p->data = NULL;
	return 0;
}
