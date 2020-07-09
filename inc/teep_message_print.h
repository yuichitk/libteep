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

#define MAX_PRINT_BYTE_COUNT       3

void print_teep_query_request(const teep_query_request_t *query_request);
void print_teep_query_response(const teep_query_response_t *query_response);
void print_teep_trusted_app_install(const teep_trusted_app_install_t *app_install);
void print_teep_trusted_app_delete(const teep_trusted_app_delete_t *app_delete);
void print_teep_success(const teep_success_t *success);
void print_teep_error(const teep_error_t *error);
void print_teep_message(const uint8_t *message, const size_t message_len);

#endif  /* TEEP_MESSAGE_PRINT_H */
