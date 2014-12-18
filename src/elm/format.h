/*
 *  Copyright (C) 2013, 2014 Swirly Cloud Limited. All rights reserved.
 */
#ifndef ELM_FORMAT_H
#define ELM_FORMAT_H

#include <dbr/elm/format.h>

#include <stddef.h> // size_t

DBR_EXTERN const char*
elm_action_str(int action);

DBR_EXTERN size_t
elm_action_len(int action);

DBR_EXTERN const char*
elm_role_str(int role);

DBR_EXTERN size_t
elm_role_len(int role);

DBR_EXTERN const char*
elm_state_str(int state);

DBR_EXTERN size_t
elm_state_len(int state);

#endif // ELM_FORMAT_H
