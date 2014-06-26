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

#include <dbr/fig.h>

typedef struct {
    DbrBool doobry;
    ngx_str_t mdaddr;
    ngx_str_t traddr;
    ngx_uint_t tmout;
    ngx_uint_t capacity;
    DbrCtx ctx;
} ngx_http_doobry_loc_conf_t;

static void
ngx_http_doobry_cleanup(void* data)
{
}

static void*
ngx_http_doobry_create_loc_conf(ngx_conf_t* cf)
{
    ngx_http_doobry_loc_conf_t* lcf = ngx_pcalloc(cf->pool, sizeof(ngx_http_doobry_loc_conf_t));
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

    cln->handler = ngx_http_doobry_cleanup;
    cln->data = lcf;
    return lcf;
}

static char*
ngx_http_doobry_merge_loc_conf(ngx_conf_t* cf, void* prev, void* conf)
{
    ngx_http_doobry_loc_conf_t* parent = prev;
    ngx_http_doobry_loc_conf_t* child = conf;

    ngx_conf_merge_str_value(child->mdaddr, parent->mdaddr, "tcp://localhost:3270");
    ngx_conf_merge_str_value(child->traddr, parent->traddr, "tcp://localhost:3271");
    ngx_conf_merge_uint_value(child->tmout, parent->tmout, 5000);
    ngx_conf_merge_uint_value(child->capacity, parent->capacity, 8 * 1024 * 1024);

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_doobry_handler(ngx_http_request_t* r)
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
ngx_http_doobry(ngx_conf_t* cf, ngx_command_t* cmd, void* conf)
{
    ngx_http_core_loc_conf_t* clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    // Handler for the 'doobry' directive.
    clcf->handler = ngx_http_doobry_handler;

    ngx_http_doobry_loc_conf_t* lcf = conf;
    lcf->doobry = DBR_TRUE;

    return NGX_CONF_OK;
}

static char*
ngx_http_doobry_mdaddr(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

static char*
ngx_http_doobry_traddr(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

static char*
ngx_http_doobry_tmout(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

static char*
ngx_http_doobry_capacity(ngx_conf_t* cf, void* post, void* data)
{
    return NGX_CONF_OK;
}

static ngx_conf_post_handler_pt ngx_http_doobry_mdaddr_p = ngx_http_doobry_mdaddr;
static ngx_conf_post_handler_pt ngx_http_doobry_traddr_p = ngx_http_doobry_traddr;
static ngx_conf_post_handler_pt ngx_http_doobry_tmout_p = ngx_http_doobry_tmout;
static ngx_conf_post_handler_pt ngx_http_doobry_capacity_p = ngx_http_doobry_capacity;

// Never called?
static ngx_int_t
ngx_http_doobry_init_master(ngx_log_t* log)
{
    ngx_log_error(NGX_LOG_NOTICE, log, 0, "init_master()");
    return NGX_OK;
}

static ngx_int_t
ngx_http_doobry_init_module(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "init_module()");
    return NGX_OK;
}

static ngx_int_t
ngx_http_doobry_init_process(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "init_process()");
    return NGX_OK;
}

static ngx_int_t
ngx_http_doobry_init_thread(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "term_thread()");
    return NGX_OK;
}

static void
ngx_http_doobry_exit_thread(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exit_thread()");
}

static void
ngx_http_doobry_exit_process(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exit_process()");
}

static void
ngx_http_doobry_exit_master(ngx_cycle_t* cycle)
{
    ngx_log_error(NGX_LOG_NOTICE, cycle->log, 0, "exit_master()");
}

// The context type required for http modules.

static ngx_http_module_t ngx_http_doobry_module_ctx = {
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
    .create_loc_conf = ngx_http_doobry_create_loc_conf,
    // char* (*merge_loc_conf)(ngx_conf_t* cf, void* prev, void* conf);
    .merge_loc_conf = ngx_http_doobry_merge_loc_conf
};

static ngx_command_t ngx_http_doobry_commands[] = {
    {
        .name = ngx_string("doobry"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_NOARGS,
        .set = ngx_http_doobry,
        .conf = 0,
        .offset = 0,
        .post = NULL
    },
    {
        .name = ngx_string("doobry_mdaddr"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_str_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(ngx_http_doobry_loc_conf_t, mdaddr),
        .post = &ngx_http_doobry_mdaddr_p
    },
    {
        .name = ngx_string("doobry_traddr"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_str_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(ngx_http_doobry_loc_conf_t, traddr),
        .post = &ngx_http_doobry_traddr_p
    },
    {
        .name = ngx_string("doobry_tmout"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_num_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(ngx_http_doobry_loc_conf_t, tmout),
        .post = &ngx_http_doobry_tmout_p
    },
    {
        .name = ngx_string("doobry_capacity"),
        .type = NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
        .set = ngx_conf_set_num_slot,
        .conf = NGX_HTTP_LOC_CONF_OFFSET,
        .offset = offsetof(ngx_http_doobry_loc_conf_t, capacity),
        .post = &ngx_http_doobry_capacity_p
    },
    ngx_null_command
};

ngx_module_t ngx_http_doobry_module = {
    // version - contains the version of Nginx API (currently it is 1).
    NGX_MODULE_V1,
    // ctx - points to a global context of the module (the actual type of structure this pointer
    // points to depends on the value of type field).
    // void* ctx;
    .ctx = &ngx_http_doobry_module_ctx,
    // commands - points to a vector of descriptors of module commands.
    // ngx_command_t* commands;
    .commands = ngx_http_doobry_commands,
    // type - contains the type of the module: NGX_HTTP_MODULE, NGX_EVENT_MODULE, NGX_MAIL_MODULE
    // and others.
    // ngx_uint_t type;
    .type = NGX_HTTP_MODULE,
    // ngx_int_t (*init_master)(ngx_log_t* log);
    .init_master = ngx_http_doobry_init_master,
    // init_module - points to a handler, that is called upon initialisation of the module in the
    // main process before entering the event loop.
    // ngx_int_t (*init_module)(ngx_cycle_t* cycle);
    .init_module = ngx_http_doobry_init_module,
    // init_process - points to a handler, that is called upon initialisation of the module in a
    // worker process.
    // ngx_int_t (*init_process)(ngx_cycle_t* cycle);
    .init_process = ngx_http_doobry_init_process,
    // ngx_int_t (*init_thread)(ngx_cycle_t* cycle);
    .init_thread = ngx_http_doobry_init_thread,
    // void (*exit_thread)(ngx_cycle_t* cycle);
    .exit_thread = ngx_http_doobry_exit_thread,
    // exit_process - points to a handler, that is called upon termination of a worker process.
    // void (*exit_process)(ngx_cycle_t* cycle);
    .exit_process = ngx_http_doobry_exit_process,
    // exit_master - points to a handler, that is called upon termination of the master process.
    // void (*exit_master)(ngx_cycle_t* cycle);
    .exit_master = ngx_http_doobry_exit_master,
    NGX_MODULE_V1_PADDING
};
