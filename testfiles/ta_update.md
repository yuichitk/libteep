

## CBOR binary (extracted)
    82                                              # array(2)
       03                                           # unsigned(3)
       A2                                           # map(2)
          0A                                        # unsigned(10)
          81                                        # array(1)
             59 010E                                # bytes(270)
                # SUIT_Envelope #
                A2                                  # map(2)
                   02                               # unsigned(2) / authentication-wrapper : /
                   58 72                            # bytes(114)
                      # SUIT_Authentication #
                      81                            # array(1) # NOTE: should be 82 #
                        # NOTE : need raw SUIT_Digest same below #
                        #58 20
                           #82                      # array(2)
                              #02                   # unsigned(2) / algorithm-id : "sha256" /
                              #58 20                # bytes(32)   / digest-bytes /
                                  #81532771898E4EBCCCF12C607420EBA62B5086192CAC4C99692835B58EE62F7B
                         58 6F                      # bytes(111)
                            # SUIT_Authentication_Block #
                            D2                      # tag(18) / COSE_Sign1 /
                               84                   # array(4)
                                  43                # bytes(3)
                                     # protected #
                                     A1             # map(1)
                                        01          # unsigned(1) / alg : /
                                        26          # negative(6) / -7 /
                                  A0                # map(0)
                                  58 24             # bytes(36)
                                     # payload = SUIT_Digest #
                                     82             # array(2)
                                        02          # unsigned(2) / algorithm-id : "sha256" /
                                        58 20       # bytes(32)   / digest-bytes /
                                           81532771898E4EBCCCF12C607420EBA62B5086192CAC4C99692835B58EE62F7B
                                  58 40             # bytes(64)
                                     815921E5148E9B81E79D8BE570DE6BB42BA2E903C8549F0E13DEE4D0EE420D90DD9F8537EBEAD3F92B37DF703539879129183B0BEAF3BA75CACD8A91E075A24E
                   03                               # unsigned(3) / manifest : /
                   58 95                            # bytes(149)
                      # SUIT_Manifest #
                      A5                            # map(5)
                         01                         # unsigned(1) / manifest-version : /
                         01                         # unsigned(1)
                         02                         # unsigned(2) / manifest-sequence-number : /
                         02                         # unsigned(2)
                         03                         # unsigned(3) / common : /
                         58 60                      # bytes(96)
                            # SUIT_Common #
                            A2                      # map(2)
                               02                   # unsigned(2) / components : /
                               44                   # bytes(4) # should not be bstr([[h'00']]) but [[h'00']]
                                  81                # array(1)    / [[h'00']]
                                     81             # array(1)
                                        41          # bytes(1)
                                           00       # "\x00"
                               04                   # unsigned(4) / common-sequence : /
                               58 56                # bytes(86)
                                  # SUIT_Common_Sequence #
                                  86                # array(6)
                                     14             # unsigned(20) / directive-override-parameters : /
                                     A4             # map(4)
                                        01          # unsigned(1) / vendor-id : /
                                        50          # bytes(16)
                                           FA6B4A53D5AD5FDFBE9DE663E4D41FFE
                                        02          # unsigned(2) / class-id : /
                                        50          # bytes(16)
                                           1492AF1425695E48BF429B2D51F2AB45
                                        03          # unsigned(3) / image-digest : /
                                        58 24       # bytes(36)
                                           # SUIT_Digest #
                                           82       # array(2)
                                              02    # unsigned(2) / suit-digest-algorithm-ids : algorithm-id-sha256 /
                                              58 20 # bytes(32)   / digest-bytes : /
                                                 00112233445566778899AABBCCDDEEFF0123456789ABCDEFFEDCBA9876543210
                                        0E          # unsigned(14) / image-size : /
                                        19 87D0     # unsigned(34768)
                                     01             # unsigned(1)   / condition-vendor-identifier : /
                                     F6             # primitive(22) / = null / # NOTE : is it OK to be null?, though the examples in draft-suit is 0x0F (not UUID) #
                                     02             # unsigned(2)   / condition-class-identifier : /
                                     F6             # primitive(22) / = null / # NOTE : is it OK to be null? #
                         09                         # unsigned(9) / install : /
                         58 25                      # bytes(37)
                            # SUIT_Common_Sequence #
                            86                      # array(6)
                               13                   # unsigned(19) / directive-set-parameters : /
                               A1                   # map(1)
                                  15                # unsigned(21) / uri : /
                                  78 1B             # text(27) / "http://example.com/file.bin" /
                                     687474703A2F2F6578616D706C652E636F6D2F66696C652E62696E
                               15                   # unsigned(21) / directive-fetch : /
                               F6                   # primitive(22) / = null / # NOTE : is it OK to be null? #
                               03                   # unsigned(3)  / condition-image-match : /
                               F6                   # primitive(22) / = null / # NOTE : is it OK to be null? #
                         0A                         # unsigned(10) / validate : /
                         43                         # bytes(3)
                            # SUIT_Common_Seqnece #
                            82                      # array(2)
                               03                   # unsigned(3)   / condition-image-match : /
                               F6                   # primitive(10) / = null /
          14                                        # unsigned(20) / token : /
          1A 77777778                               # unsigned(2004318072)


## CBOR binary (extracted and fixed)
    82                                              # array(2)
       03                                           # unsigned(3)
       A2                                           # map(2)
          14                                        # unsigned(20) / token : /
          1A 77777778                               # unsigned(2004318072)
          0A                                        # unsigned(10)
          81                                        # array(1)
             59 0133                                # bytes(307)
                # SUIT_Envelope #
                A2                                  # map(2)
                   02                               # unsigned(2) / authentication-wrapper : /
                   58 98                            # bytes(152)
                      # SUIT_Authentication #
                      82                            # array(1) # NOTE: should be 82 #
                         # NOTE : need raw SUIT_Digest same below #
                         58 24                      # bytes(36)
                            82                      # array(2)
                               02                   # unsigned(2) / algorithm-id : "sha256" /
                               58 20                # bytes(32)   / digest-bytes /
                                  81532771898E4EBCCCF12C607420EBA62B5086192CAC4C99692835B58EE62F7B
                         58 6F
                            D2
                               84
                                  43
                                     A1
                                        01
                                        26
                                     A0
                                     58 24
                                        82
                                           02
                                           58 20
                                              81532771898E4EBCCCF12C607420EBA62B5086192CAC4C99692835B58EE62F7B
                                     58 40
                                        815921E5148E9B81E79D8BE570DE6BB42BA2E903C8549F0E13DEE4D0EE420D90DD9F8537EBEAD3F92B37DF703539879129183B0BEAF3BA75CACD8A91E075A24E
                   03                               # unsigned(3) / manifest : /
                   58 94                            # bytes(148)
                      # SUIT_Manifest #
                      A5                            # map(5)
                         01                         # unsigned(1) / manifest-version : /
                         01                         # unsigned(1)
                         02                         # unsigned(2) / manifest-sequence-number : /
                         02                         # unsigned(2)
                         03                         # unsigned(3) / common : /
                         58 5F                      # bytes(95)
                            # SUIT_Common #
                            A2                      # map(2)
                               02                   # unsigned(2) / components : /
                               81                   # array(1)    / [[h'00']]
                                  81                # array(1)
                                     41             # bytes(1)
                                        00          # "\x00"
                               04                   # unsigned(4) / common-sequence : /
                               58 56                # bytes(86)
                                  # SUIT_Common_Sequence #
                                  86                # array(6)
                                     14             # unsigned(20) / directive-override-parameters : /
                                     A4             # map(4)
                                        01          # unsigned(1) / vendor-id : /
                                        50          # bytes(16)
                                           FA6B4A53D5AD5FDFBE9DE663E4D41FFE
                                        02          # unsigned(2) / class-id : /
                                        50          # bytes(16)
                                           1492AF1425695E48BF429B2D51F2AB45
                                        03          # unsigned(3) / image-digest : /
                                        58 24       # bytes(36)
                                           # SUIT_Digest #
                                           82       # array(2)
                                              02    # unsigned(2) / suit-digest-algorithm-ids : algorithm-id-sha256 /
                                              58 20 # bytes(32)   / digest-bytes : /
                                                 00112233445566778899AABBCCDDEEFF0123456789ABCDEFFEDCBA9876543210
                                        0E          # unsigned(14) / image-size : /
                                        19 87D0     # unsigned(34768)
                                     01             # unsigned(1)   / condition-vendor-identifier : /
                                     0F             # unsigned(15)
                                     02             # unsigned(2)   / condition-class-identifier : /
                                     0F             # unsigned(15)
                         09                         # unsigned(9) / install : /
                         58 25                      # bytes(37)
                            # SUIT_Common_Sequence #
                            86                      # array(6)
                               13                   # unsigned(19) / directive-set-parameters : /
                               A1                   # map(1)
                                  15                # unsigned(21) / uri : /
                                  78 1B             # text(27) / "http://example.com/file.bin" /
                                     687474703A2F2F6578616D706C652E636F6D2F66696C652E62696E
                               15                   # unsigned(21) / directive-fetch : /
                               0F                   # unsigned(15)
                               03                   # unsigned(3)  / condition-image-match : /
                               0F                   # unsigned(15)
                         0A                         # unsigned(10) / validate : /
                         43                         # bytes(3)
                            # SUIT_Common_Seqnece #
                            82                      # array(2)
                               03                   # unsigned(3)   / condition-image-match : /
                               0F                   # unsigned(15)


## Command
    echo -en "\x82\x03\xA2\x0A\x81\x59\x01\x0E\xA2\x02\x58\x72\x81\x58\x6F\xD2\x84\x43\xA1\x01\x26\xA0\x58\x24\x82\x02\x58\x20\x81\x53\x27\x71\x89\x8E\x4E\xBC\xCC\xF1\x2C\x60\x74\x20\xEB\xA6\x2B\x50\x86\x19\x2C\xAC\x4C\x99\x69\x28\x35\xB5\x8E\xE6\x2F\x7B\x58\x40\x81\x59\x21\xE5\x14\x8E\x9B\x81\xE7\x9D\x8B\xE5\x70\xDE\x6B\xB4\x2B\xA2\xE9\x03\xC8\x54\x9F\x0E\x13\xDE\xE4\xD0\xEE\x42\x0D\x90\xDD\x9F\x85\x37\xEB\xEA\xD3\xF9\x2B\x37\xDF\x70\x35\x39\x87\x91\x29\x18\x3B\x0B\xEA\xF3\xBA\x75\xCA\xCD\x8A\x91\xE0\x75\xA2\x4E\x03\x58\x95\xA5\x01\x01\x02\x02\x03\x58\x60\xA2\x02\x44\x81\x81\x41\x00\x04\x58\x56\x86\x14\xA4\x01\x50\xFA\x6B\x4A\x53\xD5\xAD\x5F\xDF\xBE\x9D\xE6\x63\xE4\xD4\x1F\xFE\x02\x50\x14\x92\xAF\x14\x25\x69\x5E\x48\xBF\x42\x9B\x2D\x51\xF2\xAB\x45\x03\x58\x24\x82\x02\x58\x20\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10\x0E\x19\x87\xD0\x01\xF6\x02\xF6\x09\x58\x25\x86\x13\xA1\x15\x78\x1B\x68\x74\x74\x70\x3A\x2F\x2F\x65\x78\x61\x6D\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x66\x69\x6C\x65\x2E\x62\x69\x6E\x15\xF6\x03\xF6\x0A\x43\x82\x03\xF6\x14\x1A\x77\x77\x77\x78" > ta_update.cbor


## Command (fixed)
    echo -en "\x82\x03\xA2\x14\x1A\x77\x77\x77\x78\x0A\x81\x59\x01\x33\xA2\x02\x58\x98\x82\x58\x24\x82\x02\x58\x20\x81\x53\x27\x71\x89\x8E\x4E\xBC\xCC\xF1\x2C\x60\x74\x20\xEB\xA6\x2B\x50\x86\x19\x2C\xAC\x4C\x99\x69\x28\x35\xB5\x8E\xE6\x2F\x7B\x58\x6F\xD2\x84\x43\xA1\x01\x26\xA0\x58\x24\x82\x02\x58\x20\x81\x53\x27\x71\x89\x8E\x4E\xBC\xCC\xF1\x2C\x60\x74\x20\xEB\xA6\x2B\x50\x86\x19\x2C\xAC\x4C\x99\x69\x28\x35\xB5\x8E\xE6\x2F\x7B\x58\x40\x81\x59\x21\xE5\x14\x8E\x9B\x81\xE7\x9D\x8B\xE5\x70\xDE\x6B\xB4\x2B\xA2\xE9\x03\xC8\x54\x9F\x0E\x13\xDE\xE4\xD0\xEE\x42\x0D\x90\xDD\x9F\x85\x37\xEB\xEA\xD3\xF9\x2B\x37\xDF\x70\x35\x39\x87\x91\x29\x18\x3B\x0B\xEA\xF3\xBA\x75\xCA\xCD\x8A\x91\xE0\x75\xA2\x4E\x03\x58\x94\xA5\x01\x01\x02\x02\x03\x58\x5F\xA2\x02\x81\x81\x41\x00\x04\x58\x56\x86\x14\xA4\x01\x50\xFA\x6B\x4A\x53\xD5\xAD\x5F\xDF\xBE\x9D\xE6\x63\xE4\xD4\x1F\xFE\x02\x50\x14\x92\xAF\x14\x25\x69\x5E\x48\xBF\x42\x9B\x2D\x51\xF2\xAB\x45\x03\x58\x24\x82\x02\x58\x20\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10\x0E\x19\x87\xD0\x01\x0F\x02\x0F\x09\x58\x25\x86\x13\xA1\x15\x78\x1B\x68\x74\x74\x70\x3A\x2F\x2F\x65\x78\x61\x6D\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x66\x69\x6C\x65\x2E\x62\x69\x6E\x15\x0F\x03\x0F\x0A\x43\x82\x03\x0F" > ta_update_fixed.cbor


## CBOR binary (fixed SUIT_Envelope)
    A2                                  # map(2)
       02                               # unsigned(2) / authentication-wrapper : /
       58 98                            # bytes(152)
          # SUIT_Authentication #
          82                            # array(1) # NOTE: should be 82 #
             # NOTE : need raw SUIT_Digest same below #
             58 24                      # bytes(36)
                82                      # array(2)
                   02                   # unsigned(2) / algorithm-id : "sha256" /
                   58 20                # bytes(32)   / digest-bytes /
                      81532771898E4EBCCCF12C607420EBA62B5086192CAC4C99692835B58EE62F7B
             58 6F
                D2
                   84
                      43
                         A1
                            01
                            26
                         A0
                         58 24
                            82
                               02
                               58 20
                                  81532771898E4EBCCCF12C607420EBA62B5086192CAC4C99692835B58EE62F7B
                         58 40
                            815921E5148E9B81E79D8BE570DE6BB42BA2E903C8549F0E13DEE4D0EE420D90DD9F8537EBEAD3F92B37DF703539879129183B0BEAF3BA75CACD8A91E075A24E
       03                               # unsigned(3) / manifest : /
       58 94                            # bytes(148)
          # SUIT_Manifest #
          A5                            # map(5)
             01                         # unsigned(1) / manifest-version : /
             01                         # unsigned(1)
             02                         # unsigned(2) / manifest-sequence-number : /
             02                         # unsigned(2)
             03                         # unsigned(3) / common : /
             58 5F                      # bytes(95)
                # SUIT_Common #
                A2                      # map(2)
                   02                   # unsigned(2) / components : /
                   81                   # array(1)    / [[h'00']]
                      81                # array(1)
                         41             # bytes(1)
                            00          # "\x00"
                   04                   # unsigned(4) / common-sequence : /
                   58 56                # bytes(86)
                      # SUIT_Common_Sequence #
                      86                # array(6)
                         14             # unsigned(20) / directive-override-parameters : /
                         A4             # map(4)
                            01          # unsigned(1) / vendor-id : /
                            50          # bytes(16)
                               FA6B4A53D5AD5FDFBE9DE663E4D41FFE
                            02          # unsigned(2) / class-id : /
                            50          # bytes(16)
                               1492AF1425695E48BF429B2D51F2AB45
                            03          # unsigned(3) / image-digest : /
                            58 24       # bytes(36)
                               # SUIT_Digest #
                               82       # array(2)
                                  02    # unsigned(2) / suit-digest-algorithm-ids : algorithm-id-sha256 /
                                  58 20 # bytes(32)   / digest-bytes : /
                                     00112233445566778899AABBCCDDEEFF0123456789ABCDEFFEDCBA9876543210
                            0E          # unsigned(14) / image-size : /
                            19 87D0     # unsigned(34768)
                         01             # unsigned(1)   / condition-vendor-identifier : /
                         0F             # unsigned(15)
                         02             # unsigned(2)   / condition-class-identifier : /
                         0F             # unsigned(15)
             09                         # unsigned(9) / install : /
             58 25                      # bytes(37)
                # SUIT_Common_Sequence #
                86                      # array(6)
                   13                   # unsigned(19) / directive-set-parameters : /
                   A1                   # map(1)
                      15                # unsigned(21) / uri : /
                      78 1B             # text(27) / "http://example.com/file.bin" /
                         687474703A2F2F6578616D706C652E636F6D2F66696C652E62696E
                   15                   # unsigned(21) / directive-fetch : /
                   0F                   # unsigned(15)
                   03                   # unsigned(3)  / condition-image-match : /
                   0F                   # unsigned(15)
             0A                         # unsigned(10) / validate : /
             43                         # bytes(3)
                # SUIT_Common_Seqnece #
                82                      # array(2)
                   03                   # unsigned(3)   / condition-image-match : /
                   0F                   # unsigned(15)

## Command (SUIT_Envelope)
    echo -en "\xA2\x02\x58\x94\x82\x58\x24\x82\x02\x58\x20\x81\x53\x27\x71\x89\x8E\x4E\xBC\xCC\xF1\x2C\x60\x74\x20\xEB\xA6\x2B\x50\x86\x19\x2C\xAC\x4C\x99\x69\x28\x35\xB5\x8E\xE6\x2F\x7B\x58\x6F\xD2\x84\x43\xA1\x01\x26\xA0\x58\x24\x82\x02\x58\x20\x81\x53\x27\x71\x89\x8E\x4E\xBC\xCC\xF1\x2C\x60\x74\x20\xEB\xA6\x2B\x50\x86\x19\x2C\xAC\x4C\x99\x69\x28\x35\xB5\x8E\xE6\x2F\x7B\x58\x40\x81\x59\x21\xE5\x14\x8E\x9B\x81\xE7\x9D\x8B\xE5\x70\xDE\x6B\xB4\x2B\xA2\xE9\x03\xC8\x54\x9F\x0E\x13\xDE\xE4\xD0\xEE\x42\x0D\x90\xDD\x9F\x85\x37\xEB\xEA\xD3\xF9\x2B\x37\xDF\x70\x35\x39\x87\x91\x29\x18\x3B\x0B\xEA\xF3\xBA\x75\xCA\xCD\x8A\x91\xE0\x75\xA2\x4E\x03\x58\x94\xA5\x01\x01\x02\x02\x03\x58\x5F\xA2\x02\x81\x81\x41\x00\x04\x58\x56\x86\x14\xA4\x01\x50\xFA\x6B\x4A\x53\xD5\xAD\x5F\xDF\xBE\x9D\xE6\x63\xE4\xD4\x1F\xFE\x02\x50\x14\x92\xAF\x14\x25\x69\x5E\x48\xBF\x42\x9B\x2D\x51\xF2\xAB\x45\x03\x58\x24\x82\x02\x58\x20\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xAA\xBB\xCC\xDD\xEE\xFF\x01\x23\x45\x67\x89\xAB\xCD\xEF\xFE\xDC\xBA\x98\x76\x54\x32\x10\x0E\x19\x87\xD0\x01\x0F\x02\x0F\x09\x58\x25\x86\x13\xA1\x15\x78\x1B\x68\x74\x74\x70\x3A\x2F\x2F\x65\x78\x61\x6D\x70\x6C\x65\x2E\x63\x6F\x6D\x2F\x66\x69\x6C\x65\x2E\x62\x69\x6E\x15\x0F\x03\x0F\x0A\x43\x82\x03\x0F" > ta_update_envelope.cbor
