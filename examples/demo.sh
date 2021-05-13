#!/bin/bash
set -e echo off
trap 'err=$?; echo >&2 "Failed with error #$err"; exit $err' ERR

if [ ! -d "../tools/bin" ]; then
	cd ../tools
	source ./mktools.sh
	cd -
fi

mkdir -p ../tools/bin
echo Adding source files to ROM...
../tools/bin/padbin 0x100 ../lua4gba.gba
../tools/bin/gbfs demo.gbfs demo.lua
cat ../lua4gba.gba demo.gbfs > demo.gba
rm -f demo.gbfs
echo Done! [ demo.gba ] created.
