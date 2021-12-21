#!/bin/bash

if [ "$#" -ne 3 ] ; then
  echo "Usage $#: $0 ID SECRET_MESSAGE FILENAME" >&2
  exit 1
fi

if [ -f "$3" ]; then
sudo rm $3
fi
echo $1 | head -c 7   >  $3
echo $2 | head -c 256 >> $3
sudo chmod 600 $3
sudo chown dummy:dummy  $3
exit
