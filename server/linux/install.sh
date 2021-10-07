#!/usr/bin/env bash

if [ "$UID" -ne "0" ]
then
    echo "Must be root to run this script"
    exit 87
fi

apt update && apt install -y libhidapi-libusb0

cp ./server.py /usr/local/sbin/nasper_server.py
chmod +x /usr/local/sbin/nasper_server.py

cp ./nasper_server.service /etc/systemd/system/

pip3 install -r ./py_requirements.txt