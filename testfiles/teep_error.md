<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Error Message (for Install)
    https://tools.ietf.org/html/draft-ietf-teep-protocol-04#appendix-D.6

## CBOR Diagnostic Notation
    / teep-error = /
    [
        6,          / type : TEEP-TYPE-teep-error = 6 (fixed int) /
        2004318072, / token : 0x777777778 (uint), from Install message /
        ERR_MANIFEST_PROCESSING_FAILED,
            / err-code : ERR_MANIFEST_PROCESSING_FAILED = 17 (uint) /
        / options :  /
        {
            12 : "disk-full"  / err-msg = 12 (mapkey) :
                                    "disk-full" (UTF-8 string) /
        }
    ]


## CBOR binary Representation
    83                        # array(3)
        06                    # unsigned(6)
        1A 77777778           # unsigned(2004318072, 0x77777778)
        11                    # unsigned(17)
        A1                    # map(1)
           0B                 # unsigned(12)
           69                 # text(9)
              6469736b2d66756c6c  # "disk-full"


## Command
    echo -en "\x83\x06\x1A\x77\x77\x77\x78\x11\xA1\x0B\x69\x64\x69\x73\x6B\x2D\x66\x75\x6C\x6C" > teep_error.cbor
