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

static ngx_int_t
init_process(ngx_cycle_t* cycle)
{
    ngx_log_debug0(NGX_LOG_INFO, cycle->log, 0, "init_process()");
    return NGX_OK;
}

static void
exit_process(ngx_cycle_t* cycle)
{
    ngx_log_debug0(NGX_LOG_INFO, cycle->log, 0, "term_process()");
}

static ngx_command_t module_commands[] = {
    {
        // ngx_str_t name;
        .name = ngx_string("doobry"),
        // ngx_uint_t type;
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        // char* (*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
        .set = set_doobry,
        // ngx_uint_t conf;
        .conf = 0,
        // ngx_uint_t offset;
        .offset = 0,
        // void *post;
        .post = NULL
    },
    ngx_null_command
};

// The context type required for http modules.

static ngx_http_module_t module_ctx = {
    // ngx_int_t (*preconfiguration)(ngx_conf_t *cf);
    .preconfiguration = NULL,
    // ngx_int_t (*postconfiguration)(ngx_conf_t *cf);
    .postconfiguration = NULL,
    // void* (*create_main_conf)(ngx_conf_t *cf);
    .create_main_conf = NULL,
    // char* (*init_main_conf)(ngx_conf_t *cf, void *conf);
    .init_main_conf = NULL,
    // void* (*create_srv_conf)(ngx_conf_t *cf);
    .create_srv_conf = NULL,
    // char* (*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);
    .merge_srv_conf = NULL,
    // void* (*create_loc_conf)(ngx_conf_t *cf);
    .create_loc_conf = NULL,
    // char* (*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
    .merge_loc_conf = NULL
};

ngx_module_t ngx_http_doobry_module = {
    NGX_MODULE_V1,
    // void *ctx;
    .ctx = &module_ctx,
    // ngx_command_t *commands;
    .commands = module_commands,
    // ngx_uint_t type;
    .type = NGX_HTTP_MODULE,
    // ngx_int_t (*init_master)(ngx_log_t *log);
    .init_master = NULL,
    // ngx_int_t (*init_module)(ngx_cycle_t *cycle);
    .init_module = NULL,
    // ngx_int_t (*init_process)(ngx_cycle_t *cycle);
    .init_process = init_process,
    // ngx_int_t (*init_thread)(ngx_cycle_t *cycle);
    .init_thread = NULL,
    // void (*exit_thread)(ngx_cycle_t *cycle);
    .exit_thread = NULL,
    // void (*exit_process)(ngx_cycle_t *cycle);
    .exit_process = exit_process,
    // void (*exit_master)(ngx_cycle_t *cycle);
    .exit_master = NULL,
    NGX_MODULE_V1_PADDING
};
