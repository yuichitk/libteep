<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryRequest Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-04#appendix-D.3

## CBOR Diagnostic Notation
    / query-response = /
    [
        2,          / type : TEEP-TYPE-query-response = 2 (fixed int) /
        / options : /
        {
            20 : 0xaba1a2a3a4a5a6a7,
                / token : h'aba1a2a3a4a5a6a7' (bstr .size (8..64)) /
                / given from TAM's QueryRequest message /
            5 : 1,  / selected-cipher-suite = 5(mapkey) :/
                    / TEEP-AES-CCM-16-64-128-HMAC256--256-X25519-EdDSA =
                          1 (uint .size 8) /
            6 : 0,  / selected-version = 6 (mapkey) : 0 (uint .size 4) /
            8 : [   / tc-list = 8 (mapkey) : (array of tc-info) /
                    {
                        16 : [ 0x0102030405060708090a0b0c0d0e0f ] / component-id = 16 (mapkey) : /
                        / [ h'0102030405060708090a0b0c0d0e0f' ] (SUIT_Component_Identifier, array of bstr) /
                    },
                    {
                        16 : [ 0x1102030405060708090a0b0c0d0e0f ] / component-id = 16 (mapkey) : /
                        / [ h'1102030405060708090a0b0c0d0e0f' ] (SUIT_Component_Identifier, array of bstr) /
                    }
                ]
        }
    ]


## CBOR binary Representation
    82                        # array(2)
       02                     # unsigned(2)
       A4                     # map(4)
          14                  # unsigned(20)
          48                 # bytes(8)
             ABA1A2A3A4A5A6A7
          05                  # unsigned(5)
          01                  # unsigned(1) within .size 8
          06                  # unsigned(6)
          00                  # unsigned(0) within .size 4
          08                  # unsigned(8)
          82                  # array(2)
             A1               # map(1)
                10            # unsigned(16)
                81            # array(1)
                   4F         # bytes(15)
                      0102030405060708090A0B0C0D0E0F
             A1               # map(1)
                10            # unsigned(16)
                81            # array(1)
                   4F         # bytes(15)
                      1102030405060708090A0B0C0D0E0F


## Command
    echo -en "\x82\x02\xA4\x14\x48\xAB\xA1\xA2\xA3\xA4\xA5\xA6\xA7\x05\x01\x06\x00\x08\x82\xA1\x10\x81\x4F\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F\xA1\x10\x81\x4F\x11\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F" > query_response.cbor
