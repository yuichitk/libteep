#include "teep/teep_common.h"
#include "teep_examples_common.h"

#if defined(LIBTEEP_PSA_CRYPTO_C)
teep_err_t teep_create_es_key(teep_key_t *key) {
    psa_key_attributes_t key_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_handle_t     key_handle = 0;
    psa_status_t         result;

    int nid;
    int hash;
    switch (key->cose_algorithm_id) {
    case T_COSE_ALGORITHM_ES256:
        nid = PSA_ECC_FAMILY_SECP_R1;
        hash = PSA_ALG_SHA_256;
        break;
    case T_COSE_ALGORITHM_ES384:
        nid = PSA_ECC_FAMILY_SECP_R1;
        hash = PSA_ALG_SHA_384;
        break;
    case T_COSE_ALGORITHM_ES512:
        nid = PSA_ECC_FAMILY_SECP_R1;
        hash = PSA_ALG_SHA_512;
        break;
    default:
        return TEEP_ERR_INVALID_VALUE;
    }

    result = psa_crypto_init();

    if (result != PSA_SUCCESS) {
        return TEEP_ERR_VERIFICATION_FAILED;
    }

    psa_key_usage_t usage = PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_EXPORT;
    if (key->private_key != NULL) {
        usage |= PSA_KEY_USAGE_SIGN_HASH;
    }

    psa_set_key_usage_flags(&key_attributes, usage);
    psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDSA(hash));
    if (key->private_key == NULL) {
        psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(nid));
        result = psa_import_key(&key_attributes,
                                (const unsigned char*)key->public_key,
                                key->public_key_len,
                                &key_handle);
    }
    else {
        psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(nid));
        result = psa_import_key(&key_attributes,
                                (const unsigned char*)key->private_key,
                                key->private_key_len,
                                &key_handle);
    }
    if (result != PSA_SUCCESS) {
        return TEEP_ERR_VERIFICATION_FAILED;
    }

    key->cose_key.k.key_handle  = key_handle;
    key->cose_key.crypto_lib    = T_COSE_CRYPTO_LIB_PSA;

    return TEEP_SUCCESS;
}

#else /* LIBTEEP_PSA_CRYPTO_C */
#if OPENSSL_VERSION_NUMBER >= OPENSSL_VERSION_300
/*
    \brief      Internal function calls OpenSSL functions to create public key.

    \param[in]  nid                 EC network id.
    \param[in]  public_key          Pointer of char array type of public key.
    \param[out] cose_public_key     Pointer and length of the resulting key.

    \return     This returns TEEP_SUCCESS or TEEP_ERR_FAILED_TO_VERIFY.
 */
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
#else /* OPENSSL_VERSION_NUMBER < OPENSSL_VERSION_300 */
teep_err_t teep_create_es_key(teep_key_t *key) {
    /* ****************************************** */
    /* cose algorithm enum -> openssl group name  */
    /* ****************************************** */
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

    /* ********************************* */
    /* create EC_KEY based on group name */
    /* ********************************* */
    int curveID = OBJ_txt2nid(group_name);
    EC_KEY *pEC = EC_KEY_new_by_curve_name(curveID);
    if (!pEC) {
        return TEEP_ERR_FATAL;
    }

    /* ****************************************************************** */
    /* set a public key raw data and a private key raw data into EC_KEY   */
    /* ****************************************************************** */
    if(!EC_KEY_oct2key(pEC,(unsigned char*) key->public_key, key->public_key_len, NULL)) {
        goto err;
    }
    if (key->private_key != NULL) {
        if(!EC_KEY_oct2priv(pEC,(unsigned char*) key->private_key, key->private_key_len)) {
            goto err;
        }
    }

    /* ************************* */
    /* validity check of EC_KEY  */
    /* ************************* */
    if (!EC_KEY_check_key(pEC)){
        goto err;
    }

    /* *************************************** */
    /* EC_KEY -> EVP_PKEY and set out variable */
    /* *************************************** */
    EVP_PKEY *pkey = EVP_PKEY_new();
    if (!EVP_PKEY_set1_EC_KEY(pkey, pEC)) {
        goto err;
    } else {
        key->cose_key.k.key_ptr  = pkey;
        key->cose_key.crypto_lib = T_COSE_CRYPTO_LIB_OPENSSL;
        EC_KEY_free(pEC);
        return TEEP_SUCCESS;
    }
err:
    EC_KEY_free(pEC);
    return TEEP_ERR_FATAL;
}
#endif /* OPENSSL_VERSION_NUMBER */
#endif /* LIBTEEP_PSA_CRYPTO_C */

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

teep_err_t teep_key_init_es521_key_pair(const unsigned char *private_key, const unsigned char *public_key, teep_key_t *cose_key_pair) {
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

teep_err_t teep_key_init_es521_public_key(const unsigned char *public_key, teep_key_t *cose_public_key) {
    cose_public_key->private_key = NULL;
    cose_public_key->private_key_len = 0;
    cose_public_key->public_key = public_key;
    cose_public_key->public_key_len = SECP521R1_PUBLIC_KEY_LENGTH;
    cose_public_key->cose_algorithm_id = T_COSE_ALGORITHM_ES512;
    return teep_create_es_key(cose_public_key);
}

teep_err_t teep_free_key(const teep_key_t *key) {
#if defined(LIBTEEP_PSA_CRYPTO_C)
    psa_destroy_key(key->cose_key.k.key_handle );
#else
    EVP_PKEY_free(key->cose_key.k.key_ptr);
#endif
    return TEEP_SUCCESS;
}
