/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep_cose.h"
#include "teep_common.h"
#include "t_cose/t_cose_sign1_sign.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"


int32_t create_es256_key_pair(const char *private_key, const char *public_key, struct t_cose_key *cose_key_pair) {
    EC_GROUP    *ec_group = NULL;
    EC_KEY      *ec_key = NULL;
    BIGNUM      *private_key_bn = NULL;
    EC_POINT    *pub_key_point = NULL;
    int         result = 0;

    ec_group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
    if (ec_group == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    ec_key = EC_KEY_new();
    if (ec_key == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    result = EC_KEY_set_group(ec_key, ec_group);
    if (!result) {
        return TEEP_UNEXPECTED_ERROR;
    }
    private_key_bn = BN_new();
    if (private_key_bn == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    BN_zero(private_key_bn);
    result = BN_hex2bn(&private_key_bn, private_key);
    if(private_key_bn == 0) {
        return TEEP_UNEXPECTED_ERROR;
    }
    result = EC_KEY_set_private_key(ec_key, private_key_bn);
    if (!result) {
        return TEEP_UNEXPECTED_ERROR;
    }
    pub_key_point = EC_POINT_new(ec_group);
    if (pub_key_point == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    pub_key_point = EC_POINT_hex2point(ec_group, public_key, pub_key_point, NULL);
    if (pub_key_point == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    result = EC_KEY_set_public_key(ec_key, pub_key_point);
    if (result == 0) {
        return TEEP_UNEXPECTED_ERROR;
    }

    cose_key_pair->k.key_ptr  = ec_key;
    cose_key_pair->crypto_lib = T_COSE_CRYPTO_LIB_OPENSSL;
    return TEEP_SUCCESS;
}

int32_t create_es256_public_key(const char *public_key, struct t_cose_key *cose_public_key) {
    EC_GROUP    *ec_group = NULL;
    EC_KEY      *ec_key = NULL;
    EC_POINT    *ec_point = NULL;
    int         result = 0;

    ec_group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
    if (ec_group == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    ec_key = EC_KEY_new();
    if (ec_key == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    result = EC_KEY_set_group(ec_key, ec_group);
    if (!result) {
        return TEEP_UNEXPECTED_ERROR;
    }
    ec_point = EC_POINT_new(ec_group);
    if (ec_point == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    ec_point = EC_POINT_hex2point(ec_group, public_key, ec_point, NULL);
    if (ec_point == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    result = EC_KEY_set_public_key(ec_key, ec_point);
    if (result == 0) {
        return TEEP_UNEXPECTED_ERROR;
    }

    cose_public_key->k.key_ptr  = ec_key;
    cose_public_key->crypto_lib = T_COSE_CRYPTO_LIB_OPENSSL;
    return TEEP_SUCCESS;
}

int32_t verify_cose_sign1(const UsefulBufC *signed_cose, const char *public_key, UsefulBufC *returned_payload) {
    struct t_cose_key   cose_public_key;
    int32_t             result = TEEP_SUCCESS;
    result = create_es256_public_key(public_key, &cose_public_key);
    if (result != TEEP_SUCCESS) {
        printf("Fail make_ossl_ecdsa_public_key : result = %d\n", result);
        return result;
    }

    struct t_cose_sign1_verify_ctx  verify_ctx;
    struct t_cose_parameters        parameters;
    enum t_cose_err_t               cose_result;
    t_cose_sign1_verify_init(&verify_ctx, 0);
    t_cose_sign1_set_verification_key(&verify_ctx, cose_public_key);
    cose_result = t_cose_sign1_verify(&verify_ctx,
                                      *signed_cose,
                                      returned_payload,
                                      &parameters);
    EC_KEY_free(cose_public_key.k.key_ptr);
    if (cose_result != T_COSE_SUCCESS) {
        printf("Fail t_cose_sign1_verify : result = %d\n", cose_result);
        return TEEP_CBOR_WITHOUT_SIGN1;
    }

    return TEEP_SUCCESS;
}

int32_t sign_cose_sign1(const UsefulBufC *raw_cbor, const char *private_key, const char *public_key, UsefulBuf *signed_cose) {
    // Create cose signed file.
    struct t_cose_key cose_key_pair;
    struct t_cose_sign1_sign_ctx sign_ctx;
    enum t_cose_err_t cose_result;
    UsefulBufC tmp_signed_cose;
    UsefulBuf_MAKE_STACK_UB(signed_cose_buffer, 1024);

    int32_t result = create_es256_key_pair(private_key, public_key, &cose_key_pair);
    if (result != TEEP_SUCCESS) {
        printf("Fail make_ossl_ecdsa_key_pair : result = %d\n", result);
        return result;
    }

    t_cose_sign1_sign_init(&sign_ctx, 0, T_COSE_ALGORITHM_ES256);
    t_cose_sign1_set_signing_key(&sign_ctx, cose_key_pair, NULL_Q_USEFUL_BUF_C);
    cose_result = t_cose_sign1_sign(&sign_ctx, *raw_cbor, signed_cose_buffer, &tmp_signed_cose);
    EC_KEY_free(cose_key_pair.k.key_ptr);

    if (cose_result != T_COSE_SUCCESS) {
        printf("Fail t_cose_sign1_sign : result = %d\n", result);
        return TEEP_UNEXPECTED_ERROR;
    }
    memcpy(signed_cose->ptr, tmp_signed_cose.ptr, tmp_signed_cose.len);
    signed_cose->len = tmp_signed_cose.len;
    return TEEP_SUCCESS;
}
