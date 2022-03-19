<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Error Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-08#appendix-D.6

## CBOR Diagnostic Notation
    / teep-error = /
    [
      / type: / 6 / TEEP-TYPE-teep-error = 6 /,
      / options: /
      {
        / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF',
        / err-msg / 12 : "disk-full"
      },
      / err-code: / 17 / ERR_MANIFEST_PROCESSING_FAILED = 17 /
    ]


## CBOR binary Representation
    83                  # array(3)
       06               # unsigned(6)
       A2               # map(2)
          14            # unsigned(20)
          50            # bytes(16)
             A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
          0C            # unsigned(12)
          69            # text(9)
             6469736B2D66756C6C # "disk-full"
       11               # unsigned(17)


## Command
    echo -en "\x83\x06\xA2\x14\x50\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF\x0C\x69\x64\x69\x73\x6B\x2D\x66\x75\x6C\x6C\x11" > teep_error.cbor
