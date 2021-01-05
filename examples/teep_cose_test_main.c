/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "teep_cose.h"
#include "teep_common.h"
#include "teep_examples_common.h"

#define MAX_FILE_BUFFER_SIZE                1024

int main(int argc, const char * argv[]) {
    int32_t result;

    // Check arguments.
    if (argc < 3) {
        printf("teep_cose_test <DER file path> <CBOR file path>\n");
        return EXIT_FAILURE;
    }

    // Read der file.
    char private_key_buf[PRIME256V1_PRIVATE_KEY_CHAR_SIZE + 1];
    char public_key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE + 1];
    printf("\nmain : Read keys from DER file.\n");
    result = read_char_key_pair_from_der(argv[1], private_key_buf, public_key_buf);
    if (result != TEEP_SUCCESS) {
        printf("Failed to load key pair.\n");
        return EXIT_FAILURE;
    }
    printf("private_key_buf : %s\n", private_key_buf);
    printf("public_key_buf : %s\n\n", public_key_buf);

    // Read cbor file.
    printf("main : Read CBOR file.\n");
    uint8_t cbor_buf[MAX_FILE_BUFFER_SIZE];
    size_t cbor_len = read_from_file(argv[2], MAX_FILE_BUFFER_SIZE, cbor_buf);
    if (!cbor_len) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex_within_max(cbor_buf, cbor_len, cbor_len);
    printf("\n");

    // Create cose signed file.
    printf("main : Create signed cose file.\n");
    UsefulBufC raw_cbor = {cbor_buf, cbor_len};
    UsefulBuf tmp_signed_cose = {cbor_buf, 0}; // reuse the original cbor buffer for signed cose
    result = sign_cose_sign1(&raw_cbor, private_key_buf, public_key_buf, &tmp_signed_cose);

    // Verify cose signed file.
    UsefulBufC signed_cose = {tmp_signed_cose.ptr, tmp_signed_cose.len};
    UsefulBufC returned_payload;
    result = verify_cose_sign1(&signed_cose, public_key_buf, &returned_payload);
    if (result != TEEP_SUCCESS) {
        printf("Fail to verify file.\n");
        return EXIT_FAILURE;
    }
    printf("main : Success to verify. Print cose payload.\n");
    teep_print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    if (argc > 3) {
        size_t write_len = write_to_file(argv[3], signed_cose.len, signed_cose.ptr);
        if (!write_len) {
            printf("main : Can't write COSE signed CBOR file.%s\n", argv[3]);
            return EXIT_FAILURE;
        }
        printf("main : Success to write to \"%s\".\n", argv[3]);
    }

    return EXIT_SUCCESS;
}
