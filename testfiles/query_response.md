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
        2004318071, / token : 0x77777777 (uint), from TAM's QueryRequest
                          message /
        / options : /
        {
            5 : 1,  / selected-cipher-suite = 5(mapkey) :/
                    / TEEP-AES-CCM-16-64-128-HMAC256--256-X25519-EdDSA =
                          1 (uint .size 8) /
            6 : 0,  / selected-version = 6 (mapkey) : 0 (uint .size 4) /
            8 : [ h'0102030405060708090a0b0c0d0e0f',
                  h'1102030405060708090a0b0c0d0e0f' ]
                    / ta-list = 8 (mapkey) :
                          [ 0x0102030405060708090a0b0c0d0e0f,
                            0x1102030405060708090a0b0c0d0e0f ]
                          (array of bstr) /
        }
    ]


## CBOR binary Representation
    83                        # array(3)
       02                     # unsigned(2)
       1A 77777777            # unsigned(2004318071, 0x77777777)
       A3                     # map(3)
          05                  # unsigned(5)
          01                  # unsigned(1) within .size 8
          06                  # unsigned(6)
          00                  # unsigned(0) within .size 4
          08                  # unsigned(8)
          82                  # array(2)
             4F               # bytes(16)
                0102030405060708090A0B0C0D0D0F
             4F               # bytes(16)
                1102030405060708090A0B0C0D0D0F


## Command
    echo -en "\x83\x02\x1A\x77\x77\x77\x77\xA3\x05\x01\x06\x00\x08\x82\x4F\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0D\x0F\x4F\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0D\x0F" > query_response.cbor
