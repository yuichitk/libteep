/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep/teep_cose.h"
#include "t_cose/t_cose_sign1_sign.h"
#include "t_cose/t_cose_sign1_verify.h"
#include "t_cose/q_useful_buf.h"


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

teep_err_t teep_sign_cose_sign(const UsefulBufC raw_cbor, const teep_key_t *key_pairs, const int num_key_pair, UsefulBuf *returned_payload) {
    // Create cose signed buffer.
    struct t_cose_sign_sign_ctx sign_ctx;
    struct t_cose_signature_sign signers[TEEP_NUM_SIGNERS];
    enum t_cose_err_t cose_result;
    UsefulBufC tmp_signed_cose;

    if (num_key_pair > TEEP_NUM_SIGNERS) {
        return TEEP_ERR_NO_MEMORY;
    }

    t_cose_sign_sign_init(&sign_ctx, T_COSE_OPT_MESSAGE_TYPE_SIGN);
    for (int i = 0; i < TEEP_NUM_SIGNERS; i++) {
        t_cose_signature_sign_main_init(&signers[i], T_COSE_ALGORITHM_ES256);
        t_cose_signature_sign_main_set_signing_key(&signers[i], &key_pairs[i], NULL_Q_USEFULBUF_C);
        t_cose_sign_add_signer(&sign_ctx, t_cose_signature_sign_from_item(&signers[i]));
    }

    cose_result = t_cose_sign_sign(&sign_ctx, raw_cbor, *returned_payload, &tmp_signed_cose);
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

teep_err_t teep_verify_cose_sign(const UsefulBufC signed_cose, const teep_key_t *public_keys, const int num_key, UsefulBufC *returned_payload) {
    teep_err_t result = TEEP_SUCCESS;
    struct t_cose_sign_verify_ctx verify_ctx;
    struct t_cose_signature_verify_main verifiers[TEEP_NUM_VERIFIERS];
    enum t_cose_err_t cose_result;

    if (public_key == NULL) {
        return TEEP_ERR_VERIFICATION_FAILED;
    }
    if (num_key > TEEP_NUM_VERIFIERS) {
        return TEEP_ERR_NO_MEMORY;
    }

    t_cose_sign_verify_init(&verify_ctx, T_COSE_OPT_MESSAGE_TYPE_SIGN);
    for (int i = 0; i < TEEP_NUM_VERIFIERS; i++) {
        t_cose_signature_verify_main_init(&verifiers[i]);
        t_cose_signature_verify_main_set_key(&verifiers[i], public_key->cose_key);
        t_cose_sign_add_verifier(&verify_ctx, t_cose_signature_verify_from_main(&verifiers[i]));
    }
    cose_result = t_cose_sign1_verify(&verify_ctx,
                                      signed_cose,
                                      returned_payload,
                                      NULL);
    if (cose_result != T_COSE_SUCCESS) {
        result = TEEP_ERR_VERIFICATION_FAILED;
    }
    return result;
}
