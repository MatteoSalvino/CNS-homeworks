#!/bin/bash
input="./google-10000-english-usa.txt"
size=$(stat -c%s ./ciphertext.enc)

while read -r line
do
  echo "key : $line"
  start=`date +%s%3N`
  eval openssl enc -aes-192-cbc -pbkdf2 -d -in ./ciphertext.enc -out ./plaintext.txt -pass pass:"$line"

  if [ $? -eq 0 ]
  then
    eval file ./plaintext.txt | grep -i ascii
    if [ $? -eq 0 ]
    then
      end=`date +%s%3N`
      runtime=$((end-start))
      speed=$(echo "scale=2; $size/$runtime" | bc)

      echo "KEY FOUND !!!!\n"
      echo "decryption speed : $speed KB/s \n"
      echo "Decrypted file :\n"
      eval cat ./plaintext.txt
      break
    fi
  fi
  echo "\n"
done < "$input"
