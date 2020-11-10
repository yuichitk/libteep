<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Success Message (for Install)
    https://tools.ietf.org/html/draft-ietf-teep-protocol-04#appendix-D.5

## CBOR Diagnostic Notation
    / teep-success = /
    [
        5,          / type : TEEP-TYPE-teep-success = 5 (fixed int) /
        2004318072, / token : 0x777777778 (uint), from Install message /
    ]


## CBOR Binary Representation
    83                      # array(3)
       05                   # unsigned(5)
       1A 77777778          # unsigned(2004318072)
       A1                   # map(1)
          11                # unsigned(17)
          67                # text(7)
             73756363657373 # "success"


## Command
    echo -en "\x83\x05\x1A\x77\x77\x77\x78\xA1\x11\x67\x73\x75\x63\x63\x65\x73\x73" > teep_success.cbor
