#!/bin/sh

fastcgi3-page-compiler -updateFactory=true -updateMakefile=true *.cpsp
./autogen.sh
./configure
make	
cp ./.libs/server_pages.so.0.0.0 ../
ln -sf ../server_pages.so.0.0 ../server_pages.so
ln -sf ../server_pages.so ../server_pages.so.0

