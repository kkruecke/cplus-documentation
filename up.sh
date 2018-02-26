#!/usr/bin/env bash
rm -rf _build
rm ./cplusplus.tar.gz
make html
tar -czvf cplusplus.tar.gz  -C _build/html . 
sshpass -pkk0457 scp ./cplusplus.tar.gz kurt@66.172.33.113:~/
sshpass -pkk0457 ssh kurt@66.172.33.113 
