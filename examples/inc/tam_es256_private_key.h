/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef TAM_PRIVATE_KEY_H
#define TAM_PRIVATE_KEY_H

const unsigned char tam_es256_private_key_0[] = {
    0x84, 0x1a, 0xeb, 0xb7, 0xb9, 0xea, 0x6f, 0x02,
    0x60, 0xbe, 0x73, 0x55, 0xa2, 0x45, 0x88, 0xb9,
    0x77, 0xd2, 0x3d, 0x2a, 0xc5, 0xbf, 0x2b, 0x6b,
    0x2d, 0x83, 0x79, 0x43, 0x2a, 0x1f, 0xea, 0x98
};

const unsigned char tam_es256_private_key_1[] = {
    0xaa, 0xc6, 0x6f, 0x84, 0x3b, 0xbe, 0xda, 0xa4,
    0xfe, 0x46, 0x8b, 0x15, 0x01, 0xf5, 0xf0, 0xfe,
    0x5b, 0x94, 0x5d, 0x8b, 0xd6, 0xd0, 0x08, 0x84,
    0x41, 0x7b, 0xe1, 0x95, 0x6d, 0x9c, 0xee, 0x13
};

const unsigned char *tam_es256_private_keys[] = {
    tam_es256_private_key_0,
    tam_es256_private_key_1
};

#endif /* TAM_PRIVATE_KEY_H */
