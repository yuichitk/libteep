<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# Error Message
    https://tools.ietf.org/html/draft-ietf-teep-protocol-10#appendix-D.6

## CBOR Diagnostic Notation
~~~~cbor-diag
/ teep-error = /
[
  / type: / 6 / TEEP-TYPE-teep-error /,
  / options: /
  {
    / token / 20 : h'A0A1A2A3A4A5A6A7A8A9AAABACADAEAF',
    / err-msg / 12 : "disk-full"
  },
  / err-code: / 17 / ERR_MANIFEST_PROCESSING_FAILED /
]
~~~~


## CBOR binary Representation
~~~~
83                  # array(3)
   06               # unsigned(6) / TEEP-TYPE-teep-error /
   A2               # map(2)
      14            # unsigned(20) / token: /
      50            # bytes(16)
         A0A1A2A3A4A5A6A7A8A9AAABACADAEAF
      0C            # unsigned(12) / err-msg: /
      69            # text(9)
         6469736B2D66756C6C # "disk-full"
   11               # unsigned(17) / ERR_MANIFEST_PROCESSING_FAILED /
~~~~
