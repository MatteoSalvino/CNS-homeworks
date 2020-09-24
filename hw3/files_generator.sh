#!bin/bash
file=$1
targetsize=$2
if test -f "$file"; then
  echo "$file already exist."
else
  eval "touch $file"
  echo "$file created."
fi

filesize=$(stat -c%s "$file")
data="data sample"
datasize="${#data}"

while [ "$filesize" -lt "$targetsize" ]
do
  echo "$data" >> "$file"
  filesize=$(($filesize+$datasize))
done
