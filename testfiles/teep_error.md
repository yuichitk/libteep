<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Error Message (for QueryRequest or Update)
    https://tools.ietf.org/html/draft-ietf-teep-protocol-04#appendix-D.5

## CBOR Diagnostic Notation
    / teep-error = /
    [
        6,          / type : TEEP-TYPE-teep-error = 6 (fixed int) /
        / options :  /
        {
            20 : 0xaba1a2a3a4a5a6a7,
                / token : h'aba1a2a3a4a5a6a7' (bstr .size (8..64)) /
                / given from TAM's QueryRequest message /
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
          48                 # bytes(8)
             ABA1A2A3A4A5A6A7
          0C                 # unsigned(12)
          69                 # text(9)
             6469736b2d66756c6c  # "disk-full"
       11                    # unsigned(17)

## Command
    echo -en "\x83\x06\xA2\x14\x48\xAB\xA1\xA2\xA3\xA4\xA5\xA6\xA7\x0C\x69\x64\x69\x73\x6B\x2D\x66\x75\x6C\x6C\x11" > teep_error.cbor
