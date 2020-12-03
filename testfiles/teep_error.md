<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Error Message (for Install)
    https://tools.ietf.org/html/draft-ietf-teep-protocol-04#appendix-D.5

## CBOR Diagnostic Notation
    / teep-error = /
    [
        6,          / type : TEEP-TYPE-teep-error = 6 (fixed int) /
        / options :  /
        {
            20 : 2004318072, / token : 0x777777778 (uint), from Update message /
            12 : "disk-full"  / err-msg = 12 (mapkey) :
                                    "disk-full" (UTF-8 string) /
        }
        ERR_MANIFEST_PROCESSING_FAILED
            / err-code : ERR_MANIFEST_PROCESSING_FAILED = 17 (uint) /
    ]


## CBOR binary Representation
    83                       # array(3)
       06                    # unsigned(6)
       A2                    # map(2)
          14                 # unsigned(20)
          1A 77777778        # unsigned(2004318072, 0x77777778)
          0C                 # unsigned(12)
          69                 # text(9)
             6469736b2d66756c6c  # "disk-full"
       11                    # unsigned(17)

## Command
    echo -en "\x83\x06\xA2\x14\x1A\x77\x77\x77\x78\x0C\x69\x64\x69\x73\x6B\x2D\x66\x75\x6C\x6C\x11" > teep_error.cbor
