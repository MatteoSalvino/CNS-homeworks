#!/bin/bash
if [ "$#" -ne 3 ]; then
  echo "Missing Parameters\n1) [cbc | gcm]\n2) file\n3) password"
  return
fi

mode=$1
file=$2
password=$3
filesize=$(stat -c%s "$file")
echo "filesize : $filesize"
enc_speed=0
dec_speed=0
i=0
while [ $i -lt 10 ];
do
  start=`date +%s%3N`
  eval openssl enc -aes-256-"$mode" -pbkdf2 -e -in "$file" -out "./result.enc" -pass pass:"$password"
  end=`date +%s%3N`
  runtime=$((end-start))
  echo "runtime $runtime ms"
  speed1=$(echo "scale=2; $filesize/($runtime*1000)" | bc)
  echo "current encryption speed : $speed1 MB/s"

  start=`date +%s%3N`
  eval openssl enc -aes-256-"$mode" -pbkdf2 -d -in "./result.enc" -out "./file.txt" -pass pass:"$password"
  end=`date +%s%3N`
  runtime=$((end-start))
  speed2=$(echo "scale=2; $filesize/($runtime*1000)" | bc)
  echo "current decryption speed : $speed2 MB/s\n\n"

  enc_speed=$(echo "scale=2; $enc_speed+$speed1" | bc)
  dec_speed=$(echo "scale=2; $dec_speed+$speed2" | bc)
  i=$((i+1))
done

avg_enc_speed=$(echo "scale=2; $enc_speed/10" | bc)
avg_dec_speed=$(echo "scale=2; $dec_speed/10" | bc)

echo "Average Encryption speed : $avg_enc_speed MB/s"
echo "Average Decryption speed : $avg_dec_speed MB/s" 
echo "Average speed ratio : " $(echo "scale=4; $avg_enc_speed/$avg_dec_speed" | bc)
