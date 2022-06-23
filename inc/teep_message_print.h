/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TEEP_MESSAGE_PRINT_H
#define TEEP_MESSAGE_PRINT_H

#include <stdio.h>
#include "qcbor/qcbor.h"
#include "teep_common.h"

#define TEEP_MAX_PRINT_BYTE_COUNT       12
#define TEEP_MAX_PRINT_TEXT_COUNT       16

const char *teep_err_code_to_str(int32_t err_code);
const char *teep_cose_algs_key_to_str(int32_t cose_algs_key);
teep_err_t teep_print_query_request(const teep_query_request_t *query_request, uint32_t indent_space);
teep_err_t teep_print_query_response(const teep_query_response_t *query_response, uint32_t indent_space);
teep_err_t teep_print_update(const teep_update_t *teep_update, uint32_t indent_space, const char *ta_public_key);
teep_err_t teep_print_success(const teep_success_t *success, uint32_t indent_space);
teep_err_t teep_print_error(const teep_error_t *error, uint32_t indent_space);
teep_err_t teep_print_message(const teep_message_t *msg, uint32_t indent_space, const char *ta_public_key);

#endif  /* TEEP_MESSAGE_PRINT_H */
