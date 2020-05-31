#!/usr/bin/sh

packer.exe
objcopy -I binary -O pe-i386 -B i386 data.dat data.obj

