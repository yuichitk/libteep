/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TEEP_HTTP_CLIENT_H
#define TEEP_HTTP_CLIENT_H

#include <curl/curl.h>

teep_err_t teep_send_http_post(const char *url, UsefulBufC send_buffer, UsefulBuf *recv_buffer);

#endif  // TEEP_HTTP_CLIENT_H
