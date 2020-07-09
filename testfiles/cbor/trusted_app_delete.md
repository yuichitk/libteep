<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TrustedAppDelete
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    / trusted-app-delete = /
    [
        4,      / type : TEEP-TYPE-trusted-app-delete /
        123,    / token : 123 /
        / options : /
        {
            8 : [ h'010203', h'040506' ]    /ta-list : [ 0x010203, 0x040506 ] /
        }
    ]

## CBOR binary
    83                 # array(3)
       04              # unsigned(4)
       18 7B           # unsigned(123)
       A1              # map(1)
          08           # unsigned(8)
          82           # array(2)
             43        # bytes(3)
                010203 # "\x01\x02\x03"
             43        # bytes(3)
                040506 # "\x04\x05\x06"

## Command
    echo -en "\x83\x04\x18\x7B\xA1\x08\x82\x43\x01\x02\x03\x43\x04\x05\x06" > trusted_app_delete.cbor
