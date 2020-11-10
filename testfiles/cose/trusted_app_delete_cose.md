<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TrustedAppDelete
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    18([
        h'A10126',
        {},
        h'8304187BA108824301020343040506',
        h'5BB55DCAC28AB0B4DCA92CA9507771686F5F8E62FCBA81BA980DEE25DFF41995D521A8A5BA1AAB4C5A6B10A9BB73EFBCE2A7921AD7E2537112F1BAAC7229B326'
    ])

## CBOR binary
    D2                                      # tag(18)
       84                                   # array(4)
          43                                # bytes(3)
             A10126                         # "\xA1\x01&"
          A0                                # map(0)
          4F                                # bytes(15)
             8304187BA108824301020343040506
          58 40                             # bytes(64)
             5BB55DCAC28AB0B4DCA92CA9507771686F5F8E62FCBA81BA980DEE25DFF41995D521A8A5BA1AAB4C5A6B10A9BB73EFBCE2A7921AD7E2537112F1BAAC7229B326

## Command
    echo -en "\xd2\x84\x43\xa1\x01\x26\xa0\x4f\x83\x04\x18\x7b\xa1\x08\x82\x43\x01\x02\x03\x43\x04\x05\x06\x58\x40\x5b\xb5\x5d\xca\xc2\x8a\xb0\xb4\xdc\xa9\x2c\xa9\x50\x77\x71\x68\x6f\x5f\x8e\x62\xfc\xba\x81\xba\x98\x0d\xee\x25\xdf\xf4\x19\x95\xd5\x21\xa8\xa5\xba\x1a\xab\x4c\x5a\x6b\x10\xa9\xbb\x73\xef\xbc\xe2\xa7\x92\x1a\xd7\xe2\x53\x71\x12\xf1\xba\xac\x72\x29\xb3\x26" > trusted_app_delete_cose.cbor

## Signing Key
    key/tam_prime256v1.pem

## Payload CBOR definition
    cbor/trusted_app_delete.md
