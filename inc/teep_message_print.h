/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TEEP_MESSAGE_PRINT_H
#define TEEP_MESSAGE_PRINT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "qcbor/qcbor.h"

#define TEEP_MAX_PRINT_BYTE_COUNT       3
#define TEEP_MAX_PRINT_TEXT_COUNT       16

int32_t print_teep_query_request(const teep_query_request_t *query_request, uint32_t indent_space);
int32_t print_teep_query_response(const teep_query_response_t *query_response, uint32_t indent_space);
int32_t print_teep_update(const teep_update_t *teep_update, uint32_t indent_space);
int32_t print_teep_success(const teep_success_t *success, uint32_t indent_space);
int32_t print_teep_error(const teep_error_t *error, uint32_t indent_space);
int32_t print_teep_message(const teep_message_t *msg, uint32_t indent_space);

#endif  /* TEEP_MESSAGE_PRINT_H */
