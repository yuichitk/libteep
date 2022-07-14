/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep/teep.h"
#include <CUnit/CUnit.h>
#include <CUnit/Console.h>

void test_set_out_of_teep_buf(void);

int main(int argc, char *argv[]) {
    CU_pSuite suite;
    CU_initialize_registry();
    suite = CU_add_suite("TEEP", NULL, NULL);
    CU_add_test(suite, "test_set_out_of_teep_buf", test_set_out_of_teep_buf);
    CU_console_run_tests();
    CU_cleanup_registry();
    return 0;
}

void test_set_out_of_teep_buf_from_buf(uint8_t *ptr, size_t len, teep_buf_t *buf) {
    QCBORDecodeContext context;
    QCBORItem item;
    QCBORError error;
    QCBORDecode_Init(&context, (UsefulBufC){ptr, len}, QCBOR_DECODE_MODE_NORMAL);
    error = QCBORDecode_GetNext(&context, &item);
    CU_ASSERT(error == QCBOR_SUCCESS);
    int32_t result = teep_set_out_of_teep_buf(&context, &item, &error, buf);
    CU_ASSERT(result == TEEP_SUCCESS);
    return;
}

void test_set_out_of_teep_buf(void) {
    teep_buf_t teep_buf;

    /* SUIT_Envelope as .cbor bstr */
    uint8_t buf0[] = {0x81, 0x46, 0xA2, 0x02, 0x80, 0x03, 0x41, 0xA0};
    test_set_out_of_teep_buf_from_buf(buf0, sizeof(buf0), &teep_buf);
    CU_ASSERT(teep_buf.len == sizeof(buf0));
    CU_ASSERT(teep_buf.ptr == buf0);

    /* SUIT_Component_Identifier as Array of Array of bstr */
    uint8_t buf1[] = {0x83, 0x81, 0x41, 0x00, 0x81, 0x41, 0x02, 0x81, 0x41, 0x01};
    test_set_out_of_teep_buf_from_buf(buf1, sizeof(buf1), &teep_buf);
    CU_ASSERT(teep_buf.len == sizeof(buf1));
    CU_ASSERT(teep_buf.ptr == buf1);
}
