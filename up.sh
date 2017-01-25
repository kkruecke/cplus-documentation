#!/usr/bin/env bash
rm -rf _build
rm ./cplusplus.tar.gz
make html
tar -czvf cplusplus.tar.gz  -C _build/html . 
scp cplusplus.tar.gz kurt@direct.krueckeberg.org:/var/www/kurttest.com/cplusplus/
