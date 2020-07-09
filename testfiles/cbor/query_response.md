<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# QueryResponse
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    / query-response = /
    [
        2,      / type : TEEP-TYPE-query-response /
        123,    / token : 123 /
        / options : /
        {
            5 : 1,    / selected-cipher-suite : TEEP-AES-CCM-16-64-128-HMAC256--256-X25519-EdDSA /
            6 : 11,   / selected-version /
            7 : h'0102030405060708',    / eat : 0x0102030405060708 /
            8 : [ h'010203', h'040506' ],   /ta-list : [ 0x010203, 0x040506 ] /
            9 : [ 1, 2, 3 ]     / ext-list : [ 1, 2, 3 ] /
        }
    ]

## CBOR binary
    83                        # array(3)
       02                     # unsigned(2)
       18 7B                  # unsigned(123)
       A5                     # map(5)
          05                  # unsigned(5)
          01                  # unsigned(1)
          06                  # unsigned(6)
          0B                  # unsigned(11)
          07                  # unsigned(7)
          48                  # bytes(8)
             0102030405060708 # "\x01\x02\x03\x04\x05\x06\a\b"
          08                  # unsigned(8)
          82                  # array(2)
             43               # bytes(3)
                010203        # "\x01\x02\x03"
             43               # bytes(3)
                040506        # "\x04\x05\x06"
          09                  # unsigned(9)
          83                  # array(3)
             01               # unsigned(1)
             02               # unsigned(2)
             03               # unsigned(3)

## Command
    echo -en "\x83\x02\x18\x7B\xA5\x05\x01\x06\x0B\x07\x48\x01\x02\x03\x04\x05\x06\x07\x08\x08\x82\x43\x01\x02\x03\x43\x04\x05\x06\x09\x83\x01\x02\x03" > query_response.cbor
