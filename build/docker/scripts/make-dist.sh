#!/bin/sh
set -ev

./bootstrap.sh
./configure $*
make dist -j $CONCURRENT_JOBS
