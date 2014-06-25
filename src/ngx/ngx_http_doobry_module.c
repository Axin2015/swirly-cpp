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

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <dbr/elm/types.h>

struct Conf {
    DbrBool doobry;
    ngx_str_t mdaddr;
    ngx_str_t traddr;
    ngx_uint_t tmout;
    ngx_uint_t capacity;
};

static void
cleanup_loc_conf(void* data)
{
}

static void*
create_loc_conf(ngx_conf_t* cf)
{
    struct Conf* lcf = ngx_pcalloc(cf->pool, sizeof(struct Conf));
    if (!lcf) {
        ngx_log_error(NGX_LOG_ERR, cf->log, 0, "failed to allocate local config");
        return NULL;
    }

    lcf->tmout = NGX_CONF_UNSET_UINT;
    lcf->capacity = NGX_CONF_UNSET_UINT;

    ngx_pool_cleanup_t* cln = ngx_pool_cleanup_add(cf->pool, 0);
    if (!cln) {
        ngx_log_error(NGX_LOG_ERR, cf->log, 0, "failed to allocate pool cleanup");
        return NULL;
    }

    cln->handler = cleanup_loc_conf;
    cln->data = lcf;
    return lcf;
}

static char*
merge_loc_conf(ngx_conf_t* cf, void* prev, void* conf)
{
    struct Conf* parent = prev;
    struct Conf* child = conf;

    ngx_conf_merge_str_value(child->mdaddr, parent->mdaddr, "tcp://localhost:3270");
    ngx_conf_merge_str_value(child->traddr, parent->traddr, "tcp://localhost:3271");
    ngx_conf_merge_uint_value(child->tmout, parent->tmout, 5000);
    ngx_conf_merge_uint_value(child->capacity, parent->capacity, 8 * 1024 * 1024);

    return NGX_CONF_OK;
}

static ngx_int_t
http_handler(ngx_http_request_t* r)
{
    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char*)"text/plain";

    ngx_buf_t* b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));
    ngx_chain_t out = {
        .buf = b,
        .next = NULL
    };

    static u_char msg[] = "Test";

    b->pos = msg;
    b->last = msg + sizeof(msg);
    b->memory = 1;
    b->last_buf = 1;

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = sizeof(msg);

    ngx_http_send_header(r);
    return ngx_http_output_filter(r, &out);
}

static char*
set_doobry(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
{
    ngx_http_core_loc_conf_t* clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    // Handler for the 'doobry' directive.
    clcf->handler = http_handler;
    return NGX_CONF_OK;
}

static char*
set_doobry_mdaddr(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

static char*
set_doobry_traddr(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

static char*
set_doobry_tmout(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

static char*
set_doobry_capacity(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

// Never called?
static ngx_int_t
init_master(ngx_log_t* log)
{
    ngx_log_error(NGX_LOG_NOTICE, log, 0, "init_master()");
    return NGX_OK;
}

static ngx_int_t
init_module(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "init_module()");
    return NGX_OK;
}

static ngx_int_t
init_process(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "init_process()");
    return NGX_OK;
}

static ngx_int_t
init_thread(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "term_thread()");
    return NGX_OK;
}

static void
exit_thread(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exit_thread()");
}

static void
exit_process(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exit_process()");
}

static void
exit_master(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exit_master()");
}

// The context type required for http modules.

static ngx_http_module_t module_ctx = {
    // ngx_int_t (*preconfiguration)(ngx_conf_t* cf);
    .preconfiguration = NULL,
    // ngx_int_t (*postconfiguration)(ngx_conf_t* cf);
    .postconfiguration = NULL,
    // void* (*create_main_conf)(ngx_conf_t* cf);
    .create_main_conf = NULL,
    // char* (*init_main_conf)(ngx_conf_t* cf, void* conf);
    .init_main_conf = NULL,
    // void* (*create_srv_conf)(ngx_conf_t* cf);
    .create_srv_conf = NULL,
    // char* (*merge_srv_conf)(ngx_conf_t* cf, void* prev, void* conf);
    .merge_srv_conf = NULL,
    // void* (*create_loc_conf)(ngx_conf_t* cf);
    .create_loc_conf = create_loc_conf,
    // char* (*merge_loc_conf)(ngx_conf_t* cf, void* prev, void* conf);
    .merge_loc_conf = merge_loc_conf
};

static ngx_conf_post_handler_pt set_doobry_mdaddr_p = set_doobry_mdaddr;
static ngx_conf_post_handler_pt set_doobry_traddr_p = set_doobry_traddr;
static ngx_conf_post_handler_pt set_doobry_tmout_p = set_doobry_tmout;
static ngx_conf_post_handler_pt set_doobry_capacity_p = set_doobry_capacity;

static ngx_command_t module_commands[] = {
    {
        .name = ngx_string("doobry"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        .set = set_doobry,
        .conf = 0,
        .offset = 0,
        .post = NULL
    },
    {
        .name = ngx_string("doobry_mdaddr"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_str_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(struct Conf, mdaddr),
        .post = &set_doobry_mdaddr_p
    },
    {
        .name = ngx_string("doobry_traddr"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_str_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(struct Conf, traddr),
        .post = &set_doobry_traddr_p
    },
    {
        .name = ngx_string("doobry_tmout"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_num_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(struct Conf, tmout),
        .post = &set_doobry_tmout_p
    },
    {
        .name = ngx_string("doobry_capacity"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_num_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(struct Conf, capacity),
        .post = &set_doobry_capacity_p
    },
    ngx_null_command
};

ngx_module_t ngx_http_doobry_module = {
    // version - contains the version of Nginx API (currently it is 1).
    NGX_MODULE_V1,
    // ctx - points to a global context of the module (the actual type of structure this pointer
    // points to depends on the value of type field).
    // void* ctx;
    .ctx = &module_ctx,
    // commands - points to a vector of descriptors of module commands.
    // ngx_command_t* commands;
    .commands = module_commands,
    // type - contains the type of the module: NGX_HTTP_MODULE, NGX_EVENT_MODULE, NGX_MAIL_MODULE
    // and others.
    // ngx_uint_t type;
    .type = NGX_HTTP_MODULE,
    // ngx_int_t (*init_master)(ngx_log_t* log);
    .init_master = init_master,
    // init_module - points to a handler, that is called upon initialisation of the module in the
    // main process before entering the event loop.
    // ngx_int_t (*init_module)(ngx_cycle_t* cycle);
    .init_module = init_module,
    // init_process - points to a handler, that is called upon initialisation of the module in a
    // worker process.
    // ngx_int_t (*init_process)(ngx_cycle_t* cycle);
    .init_process = init_process,
    // ngx_int_t (*init_thread)(ngx_cycle_t* cycle);
    .init_thread = init_thread,
    // void (*exit_thread)(ngx_cycle_t* cycle);
    .exit_thread = exit_thread,
    // exit_process - points to a handler, that is called upon termination of a worker process.
    // void (*exit_process)(ngx_cycle_t* cycle);
    .exit_process = exit_process,
    // exit_master - points to a handler, that is called upon termination of the master process.
    // void (*exit_master)(ngx_cycle_t* cycle);
    .exit_master = exit_master,
    NGX_MODULE_V1_PADDING
};
