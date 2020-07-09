/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TEEP_HTTP_CLIENT_H
#define TEEP_HTTP_CLIENT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "teep_common.h"

int32_t teep_send_http_post(char *url, teep_buf_t *send_buffer, teep_buf_t *recv_buffer);

#endif  // TEEP_HTTP_CLIENT_H
