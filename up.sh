#!/usr/bin/env bash
rm -rf _build
rm ./cplusplus.tar.gz
make html
cd _build/html
tar -czvf ../../cplusplus.tar.gz  ./*
cd ../..
sshpass -pkk0457 scp ./cplusplus.tar.gz kurt@66.172.33.113:~/
sshpass -pkk0457 ssh kurt@66.172.33.113 
