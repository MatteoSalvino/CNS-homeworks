#!bin/bash

echo "Generating a key pair..."
eval openssl genpkey -algorithm RSA -out private.pem

echo -e "\n\nExtracting public key from private key..."
eval openssl rsa -in private.pem -pubout -out public.pem

echo -e "\n\nGenerating self-signed certificate..."
eval openssl req -x509 -new -key private.pem -out ca.pem

echo -e "\n\nShow certicate's details"
eval openssl x509 -text -noout -in ca.pem

echo -e "\n\nValidating self-signed certificate..."
eval openssl verify -CAfile ca.pem ca.pem

if [ $# -eq 1 ]
then
  echo -e "\n\nDigital signing document $1"
  eval openssl dgst -sha256 -sign private.pem -out sign.sha256 $1
  echo -e "\n\nReadable signature : \n"
  eval openssl base64 -in sign.sha256 -out sign
  eval cat ./sign
fi

echo -e "\n\nVerifying $1's digital signature..."
eval openssl dgst -sha256 -verify public.pem -signature sign.sha256 $1

echo -e "\n\nConverting ca.pem certificate to ca.der certificate..."
eval openssl x509 -in ca.pem -inform PEM -out ca.der -outform DER
