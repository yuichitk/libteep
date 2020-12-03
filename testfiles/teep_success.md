<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Success Message (for Install)
    https://tools.ietf.org/html/draft-ietf-teep-protocol-04#appendix-D.4

## CBOR Diagnostic Notation
    / teep-success = /
    [
        5,          / type : TEEP-TYPE-teep-success = 5 (fixed int) /
        /options :  /
        {
            20 : 2004318072, / token : 0x777777778 (uint), from Update message /
        }
    ]


## CBOR Binary Representation
    82                      # array(2)
       05                   # unsigned(5)
       A1                   # map(1)
          14                # unsigned(20)
          1A 77777778       # unsigned(2004318072, 0x77777778)


## Command
    echo -en "\x82\x05\xA1\x14\x1A\x77\x77\x77\x78" > teep_success.cbor
