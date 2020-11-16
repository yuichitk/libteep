<!--
 Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.

 SPDX-License-Identifier: BSD-2-Clause
-->

# TrustedAppInstall
    https://tools.ietf.org/html/draft-ietf-teep-protocol-02#appendix-C

## CBOR Diagnostic
    18([
        h'A10126',
        {},
        h'8303187BA10A824301020343040506',
        h'3C224014EE28D6EA847729BA181B5FF4E78AAB45AEDA5EE95F4BF19D88630ED4E5B87FFDF7E94D9CA0714339438536B7C495F64807C505B0AB7A3A3F291740B0'
    ])

## CBOR binary
    D2                                      # tag(18)
       84                                   # array(4)
          43                                # bytes(3)
             A10126                         # "\xA1\x01&"
          A0                                # map(0)
          4F                                # bytes(15)
             8303187BA10A824301020343040506
          58 40                             # bytes(64)
             3C224014EE28D6EA847729BA181B5FF4E78AAB45AEDA5EE95F4BF19D88630ED4E5B87FFDF7E94D9CA0714339438536B7C495F64807C505B0AB7A3A3F291740B0

## Command
    echo -en "\xd2\x84\x43\xa1\x01\x26\xa0\x4f\x83\x03\x18\x7b\xa1\x0a\x82\x43\x01\x02\x03\x43\x04\x05\x06\x58\x40\x3c\x22\x40\x14\xee\x28\xd6\xea\x84\x77\x29\xba\x18\x1b\x5f\xf4\xe7\x8a\xab\x45\xae\xda\x5e\xe9\x5f\x4b\xf1\x9d\x88\x63\x0e\xd4\xe5\xb8\x7f\xfd\xf7\xe9\x4d\x9c\xa0\x71\x43\x39\x43\x85\x36\xb7\xc4\x95\xf6\x48\x07\xc5\x05\xb0\xab\x7a\x3a\x3f\x29\x17\x40\xb0" > trusted_app_install_cose.cbor

## Signing Key
    ../key/tam_prime256v1.pem

## Payload CBOR definition
    ../cbor/trusted_app_install.md
