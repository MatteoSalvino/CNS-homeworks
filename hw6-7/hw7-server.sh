#! bin/bash

while :
do
  if [ -e "./req.pem" ]
  then
    echo "New certificate signing request received."
  else
    echo "No any new certificate signing request found."
  fi

  sleep 1
done
