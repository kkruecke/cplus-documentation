#!/usr/bin/env bash
rm -rf _build
rm ./cplusplus.tar.gz
make html
cd _build/html
tar -czvf ../../cplusplus.tar.gz 
cd ../..
