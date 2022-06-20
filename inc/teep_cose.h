/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#ifndef TEEP_COSE_H
#define TEEP_COSE_H

#include "t_cose/t_cose_sign1_verify.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"
#include "teep_common.h"

#define PRIME256V1_PUBLIC_KEY_DER_SIZE      91
#define PRIME256V1_PUBLIC_KEY_CHAR_SIZE     130
#define PRIME256V1_PRIVATE_KEY_DER_SIZE     121
#define PRIME256V1_PRIVATE_KEY_CHAR_SIZE    64

teep_err_t create_key_pair(int nid, const char *private_key, const char *public_key, struct t_cose_key *cose_key_pair);
teep_err_t create_public_key(int nid, const char *public_key, struct t_cose_key *cose_public_key);

teep_err_t verify_cose_sign1(const UsefulBufC signed_cose, const struct t_cose_key *cose_key, UsefulBufC *returned_payload);
teep_err_t sign_cose_sign1(const UsefulBufC raw_cbor, const struct t_cose_key *cose_key_pair, int32_t cose_algorithm_id, UsefulBuf *signed_cose);

#endif  /* TEEP_COSE_H */
