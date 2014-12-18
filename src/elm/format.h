/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef ELM_FORMAT_H
#define ELM_FORMAT_H

#include <sc/elm/format.h>

#include <stddef.h> // size_t

SC_EXTERN const char*
elm_action_str(int action);

SC_EXTERN size_t
elm_action_len(int action);

SC_EXTERN const char*
elm_role_str(int role);

SC_EXTERN size_t
elm_role_len(int role);

SC_EXTERN const char*
elm_state_str(int state);

SC_EXTERN size_t
elm_state_len(int state);

#endif // ELM_FORMAT_H
