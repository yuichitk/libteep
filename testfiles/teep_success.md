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
    83                        # array(3)
        05                    # unsigned(5)
        1A 77777778           # unsigned(2004318072, 0x77777778)


## Command
    echo -en "\x83\x05\x1A\x77\x77\x77\x78" > teep_success.cbor
