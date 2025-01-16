#!/bin/bash

cd src
gcc -shared -fPIC -o pushpin_plugin.so pushpin_plugin.c -I. -lcurl
cd ..
mv src/pushpin_plugin.so plugins/