#!/bin/sh

fastcgi3-page-compiler -updateFactory=true -updateMakefile=true *.cpsp
./autogen.sh
./configure
make	
