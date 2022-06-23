#!/bin/bash
echo -ne "\nRun TAM Server.\n"

DIR=`dirname "${0}"`
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

QUERY_REQUEST_ERR0_FILE="${DIR}/query_request.cbor"
QUERY_REQUEST_ERR1_FILE="${DIR}/query_request_err1_cose.cbor"
QUERY_REQUEST_ERR2_FILE="${DIR}/query_request_err2_cose.cbor"
QUERY_REQUEST_ERR3_FILE="${DIR}/query_request_err3_cose.cbor"
QUERY_REQUEST_FILE="${DIR}/query_request_cose.cbor"
UPDATE_FILE="${DIR}/update_cose.cbor"

function send_teep_cbor {
    # send_teep_cbor PATH_TO_CBOR_FILE
    CBOR_FILE=$1
    if [ ! -e "${CBOR_FILE}" ]; then
        echo "ERROR: File \"${CBOR_FILE}\" does not exist"
        exit
    fi
    CBOR_FILE_SIZE=`du -b "${CBOR_FILE}" | cut -f1`
    ( \
    echo "HTTP/1.1 200 OK"; \
    echo "Content-Type: application/teep+cbor"; \
    echo "Content-Length: ${CBOR_FILE_SIZE}"; \
    echo "Server: Bar/2.2"; \
    echo "Cache-Control: no-store"; \
    echo "X-Content-Type-Options: nosniff"; \
    echo "Content-Security-Policy: default-src 'none'"; \
    echo "Referrer-Policy: no-referrer"; \
    echo; \
    cat ${CBOR_FILE} \
    ) | nc -l 8080
}


echo -ne "\nSend TEEP/HTTP Update with illegal timing.\n"
send_teep_cbor $UPDATE_FILE

echo -ne "\nSend TEEP/HTTP QueryRequest without cose.\n"
send_teep_cbor $QUERY_REQUEST_ERR0_FILE

echo -ne "\nSend TEEP/HTTP QueryRequest with versions = [1].\n"
send_teep_cbor $QUERY_REQUEST_ERR1_FILE

echo -ne "\nSend TEEP/HTTP QueryRequest with ciphersuites = {enc: a256gcm}.\n"
send_teep_cbor $QUERY_REQUEST_ERR2_FILE

echo -ne "\nSend TEEP/HTTP QueryRequest.\n"
send_teep_cbor $QUERY_REQUEST_FILE

echo -ne "\nSend TEEP/HTTP Update.\n"
send_teep_cbor $UPDATE_FILE

( \
echo "HTTP/1.1 204 No Content"; \
echo "Server: Bar/2.2"; \
echo; \
) | nc -l 8080

echo -ne "\nSend TEEP/HTTP successful response.\n"
