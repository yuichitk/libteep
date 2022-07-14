/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "teep_examples_common.h"

#define MAX_FILE_BUFFER_SIZE                1024

const char p256_private_key[] = \
    "ac56b410b2fadbdd3411044d29c58bd4" \
    "c5ea5b60e6a1601e79b8e6b6070d1bc6";

const char p256_public_key[] = \
    "044d5e5f3367ec6e411f0ec397452ac0" \
    "2e6541b212761314548a629379264c5a" \
    "44308aeffc285e452ede343c0f35d21e" \
    "0e2d3751f8bd32496f90af264d686ecd" \
    "ed";

const char p384_private_key[] = \
    "03df14f4b8a43fd8ab75a6046bd2b5" \
    "eaa6fd10b2b203fd8a78d7916de20a" \
    "a241eb37ec3d4c693d23ba2b4f6e5b" \
    "66f57f";

const char p384_public_key[] = \
    "04bdd9c3f818c9cef3e11e2d40e775" \
    "beb37bc376698d71967f93337a4e03" \
    "2dffb11b505067dddb4214b56d9bce" \
    "c59177eccd8ab05f50975933b9a738" \
    "d90c0b07eb9519567ef9075807cf77" \
    "139fc1fe85608851361136806123ed" \
    "c735ce5a03e8e4";

const char ed25519_private_key[] = \
    "2c32e95e5a6f3bd4803be94f0282c310" \
    "2d82dae312ab33aa0471d0141425708c";

const char ed25519_public_key[] = \
    "169b645d7e6d3a73064367200677412b" \
    "44c6de565164eb867b16228917d95c52";

int32_t create_cose(int32_t cose_algorithm_id, UsefulBufC raw_cbor, struct t_cose_key *cose_key, const char *filename) {
    int32_t result;
    printf("main : Create signed cose file.\n");
    UsefulBuf_MAKE_STACK_UB(tmp_signed_cose, 1024);
    result = sign_cose_sign1(raw_cbor, cose_key, cose_algorithm_id, &tmp_signed_cose);
    if (result != TEEP_SUCCESS) {
        printf("sign_cose_sign1() = %d\n", result);
        return result;
    }

    // Verify cose signed file.
    UsefulBufC signed_cose = {tmp_signed_cose.ptr, tmp_signed_cose.len};
    UsefulBufC returned_payload;
    result = verify_cose_sign1(signed_cose, cose_key, &returned_payload);
    if (result != TEEP_SUCCESS) {
        printf("Fail to verify file. %d\n", result);
        return result;
    }
    printf("main : Success to verify. Print cose payload.\n");
    teep_print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    size_t write_len = write_to_file(filename, signed_cose.len, signed_cose.ptr);
    if (!write_len) {
        printf("main : Can't write COSE signed CBOR file.%s\n", filename);
        exit(EXIT_FAILURE);
    }
    printf("main : Success to write to \"%s\".\n", filename);
    return TEEP_SUCCESS;
}

int32_t create_es256_cose(UsefulBufC raw_cbor, struct t_cose_key *cose_key, const char *filename) {
    return create_cose(T_COSE_ALGORITHM_ES256, raw_cbor, cose_key, filename);
}

int32_t create_es384_cose(UsefulBufC raw_cbor, struct t_cose_key *cose_key, const char *filename) {
    return create_cose(T_COSE_ALGORITHM_ES384, raw_cbor, cose_key, filename);
}

#define T_COSE_ALGORITHM_EDDSA -8
int32_t create_ed25519_cose(UsefulBufC raw_cbor, struct t_cose_key *cose_key, const char *filename) {
    return create_cose(T_COSE_ALGORITHM_EDDSA, raw_cbor, cose_key, filename);
}

int main(int argc, const char * argv[]) {
    int32_t result;

    // Check arguments.
    if (argc < 3) {
        printf("%s <CBOR input file> <ES256 output file> <EdDSA output file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read cbor file.
    printf("main : Read CBOR file.\n");
    uint8_t cbor_buf[MAX_FILE_BUFFER_SIZE];
    size_t cbor_len = read_from_file(argv[1], MAX_FILE_BUFFER_SIZE, cbor_buf);
    if (!cbor_len) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex_within_max(cbor_buf, cbor_len, cbor_len);
    printf("\n");

    UsefulBufC raw_cbor = {cbor_buf, cbor_len};
    struct t_cose_key cose_key_pair;

    result = create_es256_key_pair(p256_private_key, p256_public_key, &cose_key_pair);
    if (result != TEEP_SUCCESS) {
        printf("main : create_es256_key_pair() = %d\n", result);
        return EXIT_FAILURE;
    }
    result = create_es256_cose(raw_cbor, &cose_key_pair, argv[2]);
    if (result != TEEP_SUCCESS) {
        printf("main : create_es256_cose() = %d\n", result);
    }
    EC_KEY_free(cose_key_pair.k.key_ptr);

    result = create_es384_key_pair(p384_private_key, p384_public_key, &cose_key_pair);
    if (result != TEEP_SUCCESS) {
        printf("main : create_es384_key_pair() = %d\n", result);
        return EXIT_FAILURE;
    }
    result = create_es384_cose(raw_cbor, &cose_key_pair, argv[3]);
    if (result != TEEP_SUCCESS) {
        printf("main : create_es384_cose() = %d\n", result);
    }
    EC_KEY_free(cose_key_pair.k.key_ptr);

    return EXIT_SUCCESS;
}
