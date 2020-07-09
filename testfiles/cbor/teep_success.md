<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TEEP Success
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    / teep-success = /
    [
        6,      / type : TEEP-TYPE-teep-success /
        123,    / token : 123 /
        / options : /
        {
            11 : "success"  / msg : "success" /
        }
    ]

## CBOR binary
    83                      # array(3)
       06                   # unsigned(6)
       18 7B                # unsigned(123)
       A1                   # map(1)
          0B                # unsigned(11)
          67                # text(7)
             73756363657373 # "success"

## Command
    echo -en "\x83\x06\x18\x7B\xA1\x0B\x67\x73\x75\x63\x63\x65\x73\x73" > teep_success.cbor
