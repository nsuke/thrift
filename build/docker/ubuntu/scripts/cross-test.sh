#!/bin/sh
set -ev

./bootstrap.sh
./configure --enable-tutorial=no
make -j4 precross
make cross
