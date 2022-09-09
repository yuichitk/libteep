/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#ifndef TEEP_COSE_H
#define TEEP_COSE_H

#include "teep/teep_common.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/t_cose_sign1_sign.h"
#if defined(LIBTEEP_PSA_CRYPTO_C)
#include "psa/crypto.h"
#else
#include "openssl/evp.h"
#include "openssl/ec.h"
#include "openssl/param_build.h"
#endif /* LIBTEEP_PSA_CRYPTO_C */

#define PRIME256V1_PRIVATE_KEY_LENGTH       32
#define PRIME256V1_PRIVATE_KEY_CHAR_LENGTH  64
#define PRIME256V1_PUBLIC_KEY_LENGTH        65
#define PRIME256V1_PUBLIC_KEY_CHAR_LENGTH   130
#define SECP384R1_PRIVATE_KEY_LENGTH        48
#define SECP384R1_PRIVATE_KEY_CHAR_LENGTH   96
#define SECP384R1_PUBLIC_KEY_LENGTH         97
#define SECP384R1_PUBLIC_KEY_CHAR_LENGTH    194
#define SECP521R1_PRIVATE_KEY_LENGTH        66
#define SECP521R1_PRIVATE_KEY_CHAR_LENGTH   132
#define SECP521R1_PUBLIC_KEY_LENGTH         133
#define SECP521R1_PUBLIC_KEY_CHAR_LENGTH    266

typedef struct teep_key {
    int cose_usage; // COSE_Sign1, COSE_Sign, COSE_Encrypt0, COSE_Encrypt, etc.
    const unsigned char *private_key;
    size_t private_key_len;
    const unsigned char *public_key;
    size_t public_key_len;
    int cose_algorithm_id;
    struct t_cose_key cose_key;
} teep_key_t;

teep_err_t teep_create_ec_key(teep_key_t *key);
teep_err_t teep_key_free(const teep_key_t *key);
teep_err_t teep_key_init_es256_key_pair(const unsigned char *private_key, const unsigned char *public_key, teep_key_t *cose_key_pair);
teep_err_t teep_key_init_es384_key_pair(const unsigned char *private_key, const unsigned char *public_key, teep_key_t *cose_key_pair);
teep_err_t teep_key_init_es512_key_pair(const unsigned char *private_key, const unsigned char *public_key, teep_key_t *cose_key_pair);
teep_err_t teep_key_init_es256_public_key(const unsigned char *public_key, teep_key_t *cose_public_key);
teep_err_t teep_key_init_es384_public_key(const unsigned char *public_key, teep_key_t *cose_public_key);
teep_err_t teep_key_init_es512_public_key(const unsigned char *public_key, teep_key_t *cose_public_key);

teep_err_t teep_sign_cose_sign1(const UsefulBufC raw_cbor, const teep_key_t *key_pair, UsefulBuf *returned_payload);
teep_err_t teep_verify_cose_sign1(const UsefulBufC signed_cose, const teep_key_t *public_key, UsefulBufC *returned_payload);

#endif  /* TEEP_COSE_H */
