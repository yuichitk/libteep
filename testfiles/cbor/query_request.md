<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryRequest
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    / query-request = /
    [
        1,      / type : TEEP-TYPE-query-request /
        123,    / token : 123 /
        / options : /
        {
            5 : 1,    / selected-cipher-suite : TEEP-AES-CCM-16-64-128-HMAC256--256-X25519-EdDSA /
            2 : h'0102030405060708',    / nonce : 0x0102030405060708 /
            3 : [ 11, 12, 13 ], / version : [ 11, 12, 13] /
            4 : h'010203'   / oscp-data : 0x010203 /
        },
        2   / data-item-requested : trusted-apps /
    ]

## CBOR binary
    84                        # array(4)
       01                     # unsigned(1)
       18 7B                  # unsigned(123)
       A4                     # map(4)
          05                  # unsigned(5)
          01                  # unsigned(1)
          02                  # unsigned(2)
          48                  # bytes(8)
             0102030405060708 # "\x01\x02\x03\x04\x05\x06\a\b"
          03                  # unsigned(3)
          83                  # array(3)
             0B               # unsigned(11)
             0C               # unsigned(12)
             0D               # unsigned(13)
          04                  # unsigned(4)
          43                  # bytes(3)
             010203           # "\x01\x02\x03"
       02                     # unsigned(2)

## Command
    echo -en "\x84\x01\x18\x7B\xA4\x05\x01\x02\x48\x01\x02\x03\x04\x05\x06\x07\x08\x03\x83\x0B\x0C\x0D\x04\x43\x01\x02\x03\x02" > query_request.cbor
