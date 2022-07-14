/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep/teep_cose.h"
#include "t_cose/t_cose_sign1_sign.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"

teep_err_t teep_create_es_key(teep_key_t *key) {
    teep_err_t      result = TEEP_SUCCESS;
    EVP_PKEY        *pkey = NULL;
    EVP_PKEY_CTX    *ctx = NULL;
    OSSL_PARAM_BLD  *param_bld = NULL;
    OSSL_PARAM      *params = NULL;
    BIGNUM *priv;

    const char *group_name;
    switch (key->cose_algorithm_id) {
    case T_COSE_ALGORITHM_ES256:
        group_name = "prime256v1";
        break;
    case T_COSE_ALGORITHM_ES384:
        group_name = "secp384r1";
        break;
    case T_COSE_ALGORITHM_ES512:
        group_name = "secp521r1";
        break;
    default:
        return TEEP_ERR_INVALID_VALUE;
    }

    param_bld = OSSL_PARAM_BLD_new();
    if (param_bld == NULL) {
        return TEEP_ERR_FATAL;
    }
    if (!OSSL_PARAM_BLD_push_utf8_string(param_bld, "group", group_name, 0)
        || !OSSL_PARAM_BLD_push_octet_string(param_bld, "pub", key->public_key, key->public_key_len)) {
        result = TEEP_ERR_FATAL;
        goto out;
    }
    if (key->private_key != NULL) {
        priv = BN_bin2bn(key->private_key, key->private_key_len, NULL);
        if (priv == NULL) {
            result = TEEP_ERR_FATAL;
            goto out;
        }
        if (!OSSL_PARAM_BLD_push_BN(param_bld, "priv", priv)) {
            result = TEEP_ERR_FATAL;
            goto out;
        }
    }
    params = OSSL_PARAM_BLD_to_param(param_bld);

    if (params == NULL) {
        result = TEEP_ERR_FATAL;
        goto out;
    }
    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, NULL);
    if (ctx == NULL) {
        result = TEEP_ERR_FATAL;
        goto out;
    }
    if (ctx == NULL
        || EVP_PKEY_fromdata_init(ctx) <= 0
        || EVP_PKEY_fromdata(ctx, &pkey, EVP_PKEY_KEYPAIR, params) <= 0) {
        result = TEEP_ERR_FATAL;
        goto out;
    }

    key->cose_key.k.key_ptr  = pkey;
    key->cose_key.crypto_lib = T_COSE_CRYPTO_LIB_OPENSSL;
    return TEEP_SUCCESS;

out:
    EVP_PKEY_CTX_free(ctx);
    OSSL_PARAM_free(params);
    BN_free(priv);
    OSSL_PARAM_BLD_free(param_bld);
    return result;
}

teep_err_t teep_key_init_es256_key_pair(const unsigned char *private_key, const unsigned char *public_key, teep_key_t *cose_key_pair) {
    cose_key_pair->private_key = private_key;
    cose_key_pair->private_key_len = PRIME256V1_PRIVATE_KEY_LENGTH;
    cose_key_pair->public_key = public_key;
    cose_key_pair->public_key_len = PRIME256V1_PUBLIC_KEY_LENGTH;
    cose_key_pair->cose_algorithm_id = T_COSE_ALGORITHM_ES256;
    return teep_create_es_key(cose_key_pair);
}

teep_err_t teep_key_init_es384_key_pair(const unsigned char *private_key, const unsigned char *public_key, teep_key_t *cose_key_pair) {
    cose_key_pair->private_key = private_key;
    cose_key_pair->private_key_len = SECP384R1_PRIVATE_KEY_LENGTH;
    cose_key_pair->public_key = public_key;
    cose_key_pair->public_key_len = SECP384R1_PUBLIC_KEY_LENGTH;
    cose_key_pair->cose_algorithm_id = T_COSE_ALGORITHM_ES384;
    return teep_create_es_key(cose_key_pair);
}

teep_err_t teep_key_init_es512_key_pair(const unsigned char *private_key, const unsigned char *public_key, teep_key_t *cose_key_pair) {
    cose_key_pair->private_key = private_key;
    cose_key_pair->private_key_len = SECP521R1_PRIVATE_KEY_LENGTH;
    cose_key_pair->public_key = public_key;
    cose_key_pair->public_key_len = SECP521R1_PUBLIC_KEY_LENGTH;
    cose_key_pair->cose_algorithm_id = T_COSE_ALGORITHM_ES512;
    return teep_create_es_key(cose_key_pair);
}

teep_err_t teep_key_init_es256_public_key(const unsigned char *public_key, teep_key_t *cose_public_key) {
    cose_public_key->private_key = NULL;
    cose_public_key->private_key_len = 0;
    cose_public_key->public_key = public_key;
    cose_public_key->public_key_len = PRIME256V1_PUBLIC_KEY_LENGTH;
    cose_public_key->cose_algorithm_id = T_COSE_ALGORITHM_ES256;
    return teep_create_es_key(cose_public_key);
}

teep_err_t teep_key_init_es384_public_key(const unsigned char *public_key, teep_key_t *cose_public_key) {
    cose_public_key->private_key = NULL;
    cose_public_key->private_key_len = 0;
    cose_public_key->public_key = public_key;
    cose_public_key->public_key_len = SECP384R1_PUBLIC_KEY_LENGTH;
    cose_public_key->cose_algorithm_id = T_COSE_ALGORITHM_ES384;
    return teep_create_es_key(cose_public_key);
}

teep_err_t teep_key_init_es512_public_key(const unsigned char *public_key, teep_key_t *cose_public_key) {
    cose_public_key->private_key = NULL;
    cose_public_key->private_key_len = 0;
    cose_public_key->public_key = public_key;
    cose_public_key->public_key_len = SECP521R1_PUBLIC_KEY_LENGTH;
    cose_public_key->cose_algorithm_id = T_COSE_ALGORITHM_ES512;
    return teep_create_es_key(cose_public_key);
}

teep_err_t teep_key_free(const teep_key_t *key) {
    EVP_PKEY_free(key->cose_key.k.key_ptr);
    return TEEP_SUCCESS;
}

teep_err_t teep_sign_cose_sign1(const UsefulBufC raw_cbor, const teep_key_t *key_pair, UsefulBuf *returned_payload) {
    // Create cose signed buffer.
    struct t_cose_sign1_sign_ctx sign_ctx;
    enum t_cose_err_t cose_result;
    UsefulBufC tmp_signed_cose;

    t_cose_sign1_sign_init(&sign_ctx, 0, key_pair->cose_algorithm_id);
    t_cose_sign1_set_signing_key(&sign_ctx, key_pair->cose_key, NULL_Q_USEFUL_BUF_C);
    cose_result = t_cose_sign1_sign(&sign_ctx, raw_cbor, *returned_payload, &tmp_signed_cose);
    if (cose_result != T_COSE_SUCCESS) {
        returned_payload->len = 0;
        return TEEP_ERR_SIGNING_FAILED;
    }
    *returned_payload = UsefulBuf_Unconst(tmp_signed_cose);
    return TEEP_SUCCESS;
}

teep_err_t teep_verify_cose_sign1(const UsefulBufC signed_cose, const teep_key_t *public_key, UsefulBufC *returned_payload) {
    teep_err_t result = TEEP_SUCCESS;
    struct t_cose_sign1_verify_ctx verify_ctx;
    //struct t_cose_parameters parameters;
    enum t_cose_err_t cose_result;

    if (public_key == NULL) {
        return TEEP_ERR_VERIFICATION_FAILED;
    }

    t_cose_sign1_verify_init(&verify_ctx, 0);
    t_cose_sign1_set_verification_key(&verify_ctx, public_key->cose_key);
    cose_result = t_cose_sign1_verify(&verify_ctx,
                                      signed_cose,
                                      returned_payload,
                                      NULL);
    if (cose_result != T_COSE_SUCCESS) {
        result = TEEP_ERR_VERIFICATION_FAILED;
    }
    return result;
}


