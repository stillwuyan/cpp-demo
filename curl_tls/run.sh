#!/bin/bash

if [ "$1" == "server" ];then
    ~/workspace/3rd-package/nginx-1.21.4/dist/sbin/nginx -c /srv/data/Joynext/xu_q2/workspace/demo/cpp-demo/curl_tls/conf/nginx.conf
elif [ "$1" == "stop" ];then
    pkill nginx
elif [ "$1" == "client" ];then
    curl -vvvv --cacert ./cert/ca.crt --cert ./cert/client.crt --key ./cert/client.key  https://localhost:9530
fi
