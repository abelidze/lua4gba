#!/bin/bash
set -e echo off
trap 'err=$?; echo >&2 "Failed with error #$err"; exit $err' ERR

mkdir -p bin
echo Compiling...
gcc -Wall -O2 -s gbfs.c -I../include -o ./bin/gbfs
gcc -Wall -O2 -s lsgbfs.c -I../include -o ./bin/lsgbfs
gcc -Wall -O2 -s ungbfs.c -I../include -o ./bin/ungbfs
gcc -Wall -O2 -s padbin.c -I../include -o ./bin/padbin
echo Done!
