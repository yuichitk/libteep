#!/bin/bash
echo -ne "\nRun TAM Server.\n"

DIR=`dirname "${0}"`
ENABLE_COSE="_cose"
while getopts d OPT
do
    case $OPT in
    d)  if [ -n "${ENABLE_COSE}" ]; then
            ENABLE_COSE=""
            printf "Send CBOR WITHOUT COSE SIGN1\n"
        fi
        ;;
    *)  printf "Usage: %s: [-d]\n" $0
        exit 2
        ;;
    esac
done

QUERY_REQUEST_FILE=${DIR}"/query_request${ENABLE_COSE}.cbor"
INSTALL_FILE=${DIR}"/install${ENABLE_COSE}.cbor"

QUERY_REQUEST_SIZE=`du -b "${QUERY_REQUEST_FILE}" | cut -f1`
INSTALL_SIZE=`du -b "${INSTALL_FILE}" | cut -f1`

( \
echo "HTTP/1.1 200 OK"; \
echo "Content-Type: application/teep+cbor"; \
echo "Content-Length: ${QUERY_REQUEST_SIZE}"; \
echo "Server: Bar/2.2"; \
echo "Cache-Control: no-store"; \
echo "X-Content-Type-Options: nosniff"; \
echo "Content-Security-Policy: default-src 'none'"; \
echo "Referrer-Policy: no-referrer"; \
echo; \
cat $QUERY_REQUEST_FILE \
) | nc -l 8080

echo -ne "\nSend TEEP/HTTP QueryRequest.\n"

( \
echo "HTTP/1.1 200 OK"; \
echo "Content-Type: application/teep+cbor"; \
echo "Content-Length: ${INSTALL_SIZE}"; \
echo "Server: Bar/2.2"; \
echo "Cache-Control: no-store"; \
echo "X-Content-Type-Options: nosniff"; \
echo "Content-Security-Policy: default-src 'none'"; \
echo "Referrer-Policy: no-referrer"; \
echo; \
cat $INSTALL_FILE \
) | nc -l 8080

echo -ne "\nSend TEEP/HTTP TrustedAppInstall.\n"

( \
echo "HTTP/1.1 204 No Content"; \
echo "Server: Bar/2.2"; \
echo; \
) | nc -l 8080

echo -ne "\nSend TEEP/HTTP successful response.\n"
