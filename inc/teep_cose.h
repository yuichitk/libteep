/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#ifndef TEEP_COSE_H
#define TEEP_COSE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "t_cose/t_cose_sign1_verify.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"

int32_t create_es256_key_pair(const char *private_key, const char *public_key, struct t_cose_key *cose_key_pair);
int32_t create_es256_public_key(const char *public_key, struct t_cose_key *cose_public_key);
int32_t verify_cose_sign1(UsefulBufC *signed_cose, const char *public_key, UsefulBufC *returned_payload);

#endif  /* TEEP_COSE_H */
