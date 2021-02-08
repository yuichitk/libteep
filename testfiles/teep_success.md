<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Success Message (for Update)
    https://tools.ietf.org/html/draft-ietf-teep-protocol-04#appendix-D.4

## CBOR Diagnostic Notation
    / teep-success = /
    [
        5,          / type : TEEP-TYPE-teep-success = 5 (fixed int) /
        / options :  /
        {
            20 : 0xaba1a2a3a4a5a6a7,
                / token : h'aba1a2a3a4a5a6a7' (bstr .size (8..64)) /
                / given from TAM's QueryRequest message /
        }
    ]


## CBOR Binary Representation
    82                      # array(2)
       05                   # unsigned(5)
       A1                   # map(1)
          14                # unsigned(20)
          48                 # bytes(8)
             ABA1A2A3A4A5A6A7


## Command
    echo -en "\x82\x05\xA1\x14\x48\xAB\xA1\xA2\xA3\xA4\xA5\xA6\xA7" > teep_success.cbor
