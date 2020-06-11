#!/bin/bash -ev

mkdir -p ~/.oblivion
echo "rpcuser=username" >>~/.oblivion/oblivion.conf
echo "rpcpassword=`head -c 32 /dev/urandom | base64`" >>~/.oblivion/oblivion.conf

