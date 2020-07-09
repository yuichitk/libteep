<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TrustedAppInstall
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    / trusted-app-install = /
    [
        3,      / type : TEEP-TYPE-trusted-app-install /
        123,    / token : 123 /
        / options : /
        {
            10 : [ h'010203', h'040506' ]  / manifest-list : [ 0x010203, 0x040506 ] /
        }
    ]

## CBOR binary
    83                 # array(3)
       03              # unsigned(3)
       18 7B           # unsigned(123)
       A1              # map(1)
          0A           # unsigned(10)
          82           # array(2)
             43        # bytes(3)
                010203 # "\x01\x02\x03"
             43        # bytes(3)
                040506 # "\x04\x05\x06"

## Command
    echo -en "\x83\x03\x18\x7B\xA1\x0A\x82\x43\x01\x02\x03\x43\x04\x05\x06" > trusted_app_install.cbor
