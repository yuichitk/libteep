<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TEEP Error
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    / teep-error = /
    [
        5,      / type : TEEP-TYPE-teep-error /
        123,    / token : 123 /
        / options : /
        {
            12 : "error",   / err-msg : "error" /
            1 : [ 2 ],  / cipher-suites : [ TEEP-AES-CCM-16-64-128-HMAC256--256-P-256-ES256 ] /
            3 : [ 11, 12, 13 ]  / version : [ 11, 12, 13] /
        },
        456     / err-code : 456 /
    ]

## CBOR binary
    84                  # array(4)
       05               # unsigned(5)
       18 7B            # unsigned(123)
       A3               # map(3)
          0C            # unsigned(12)
          65            # text(5)
             6572726F72 # "error"
          01            # unsigned(1)
          81            # array(1)
             02         # unsigned(2)
          03            # unsigned(3)
          83            # array(3)
             0B         # unsigned(11)
             0C         # unsigned(12)
             0D         # unsigned(13)
       19 01C8          # unsigned(456)

## Command
    echo -en "\x84\x05\x18\x7B\xA3\x0C\x65\x65\x72\x72\x6F\x72\x01\x81\x02\x03\x83\x0B\x0C\x0D\x19\x01\xC8" > teep_error.cbor
