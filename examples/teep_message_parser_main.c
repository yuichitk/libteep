/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "qcbor/qcbor.h"
#include "teep_common.h"
#include "teep_cose.h"
#include "teep_message_data.h"
#include "teep_message_print.h"
#include "teep_examples_common.h"

#define MAX_FILE_BUFFER_SIZE                512

int main(int argc, const char * argv[]) {
    // Check arguments.
    if (argc < 3) {
        printf("teep_message_parser <DER file path> <CBOR file path>\n");
        return EXIT_FAILURE;
    }

    // Read der file.
    printf("\nmain : Read DER file.\n");
    uint8_t der_buf[PRIME256V1_PUBLIC_KEY_DER_SIZE];
    size_t der_len = read_file(argv[1], PRIME256V1_PUBLIC_KEY_DER_SIZE, der_buf);
    if (!der_len) {
        printf("main : Can't read DER file.\n");
        return EXIT_FAILURE;
    }
    print_hex(der_buf, der_len);
    printf("\n");

    // Read key from der file.
    // This code is only available for openssl prime256v1.
    printf("\nmain : Read public key from DER file.\n");
    char key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE];
    read_prime256v1_public_key(der_buf, key_buf);
    printf("%s\n", key_buf);

    // Read cbor file.
    printf("\nmain : Read CBOR file.\n");
    uint8_t cbor_buf[MAX_FILE_BUFFER_SIZE];
    size_t cbor_len = read_file(argv[2], MAX_FILE_BUFFER_SIZE, cbor_buf);
    if (!cbor_len) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    print_hex(cbor_buf, cbor_len);
    printf("\n");

    // Verify cbor file.
    printf("\nmain : Verify CBOR file.\n");
    UsefulBufC signed_cose = {cbor_buf, cbor_len};
    UsefulBufC returned_payload;
    int32_t result;
    result = verify_cose_sign1(&signed_cose, key_buf, &returned_payload);
    if (result) {
        printf("main : Fail to verify CBOR file.\n");
        return EXIT_FAILURE;
    }
    printf("\nmain : Success to verify. Print cose payload.\n");
    print_hex(returned_payload.ptr, returned_payload.len);
    printf("\n");

    // Print teep message.
    print_teep_message(returned_payload.ptr, returned_payload.len);

    return EXIT_SUCCESS;
}
