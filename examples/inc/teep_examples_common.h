/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#ifndef TEEP_EXAMPLES_COMMON_H
#define TEEP_EXAMPLES_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "t_cose/t_cose_sign1_verify.h"
#include "openssl/ecdsa.h"
#include "openssl/obj_mac.h"

#define TEEP_EXAMPLE_SUCCESS    1
#define TEEP_EXAMPLE_ERROR      0

#define PRIME256V1_PUBLIC_KEY_DER_SIZE      91
#define PRIME256V1_PUBLIC_KEY_CHAR_SIZE     130
#define PRIME256V1_PRIVATE_KEY_DER_SIZE     121
#define PRIME256V1_PRIVATE_KEY_CHAR_SIZE    64

#define PRIME256V1_PRIVATE_KEY_START_INDEX  7
#define PRIME256V1_PRIVATE_KEY_LENGTH       32
#define PRIME256V1_PUBLIC_KEY_IN_KEY_PAIR_START_INDEX  56
#define PRIME256V1_PUBLIC_KEY_START_INDEX   26
#define PRIME256V1_PUBLIC_KEY_LENGTH        65

#define ED25519_PRIVATE_KEY_LENGTH          32
#define ED25519_PUBLIC_KEY_LENGTH           32

typedef struct uint8_buf {
    uint8_t     *ptr;
    size_t      len;
} uint8_buf_t;

size_t read_from_file(const char *file_path, const size_t buf_len, uint8_t *buf);
size_t write_to_file(const char *file_path, const size_t buf_len, const void *buf);

int32_t create_es256_key_pair(const char *private_key, const char *public_key, struct t_cose_key *cose_key_pair);
int32_t create_es384_key_pair(const char *private_key, const char *public_key, struct t_cose_key *cose_key_pair);
int32_t create_ed25519_key_pair(const char *private_key, const char *public_key, struct t_cose_key *cose_key_pair);

void read_prime256v1_key_pair(const uint8_t *key_der, char *private_key, char *public_key);
void read_prime256v1_public_key(const uint8_t *public_key_der, char *public_key);
int32_t read_char_key_pair_from_der(const char *der_file_path, char *private_key, char *public_key);
int32_t read_char_public_key_from_der(const char *der_file_path, char *public_key);

#endif  /* TEEP_EXAMPLES_COMMON_H */
