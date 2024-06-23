#!/bin/bash

cd /opt/imashen/zerotier-webui

if [ -z $MYADDR ]; then
    echo "Set Your IP Address to continue."
    echo "If you don't do that, I will automatically detect."
    MYEXTADDR=$(curl --connect-timeout 5 ip.sb)
    if [ -z $MYEXTADDR ]; then
        MYINTADDR=$(ifconfig eth0 | grep -Eo 'inet (addr:)?([0-9]*\.){3}[0-9]*' | grep -Eo '([0-9]*\.){3}[0-9]*' | grep -v '127.0.0.1')
        MYADDR=${MYINTADDR}
    else
        MYADDR=${MYEXTADDR}
    fi
    echo "YOUR IP: ${MYADDR}"
fi

MYDOMAIN=${MYDOMAIN:-zerotier-webui.docker.test}   # Used for minica
ZEROTIER_WEBUI_PASSWD=${ZEROTIER_WEBUI_PASSWD:-password}   # Used for argon2g
MYADDR=${MYADDR}
HTTP_ALL_INTERFACES=${HTTP_ALL_INTERFACES}
HTTP_PORT=${HTTP_PORT:-3000}
HTTPS_PORT=${HTTPS_PORT:-3443}

while [ ! -f /var/lib/zerotier-one/authtoken.secret ]; do
    echo "ZT1 AuthToken is not found... Wait for ZT1 to start..."
    sleep 2
done
chown zerotier-one:zerotier-one /var/lib/zerotier-one/authtoken.secret
chmod 640 /var/lib/zerotier-one/authtoken.secret

cd /opt/imashen/zerotier-webui

echo "NODE_ENV=production" > /opt/imashen/zerotier-webui/.env
echo "MYADDR=$MYADDR" >> /opt/imashen/zerotier-webui/.env
echo "HTTP_PORT=$HTTP_PORT" >> /opt/imashen/zerotier-webui/.env
if [ ! -z $HTTP_ALL_INTERFACES ]; then
  echo "HTTP_ALL_INTERFACES=$HTTP_ALL_INTERFACES" >> /opt/imashen/zerotier-webui/.env
else
  [ ! -z $HTTPS_PORT ] && echo "HTTPS_PORT=$HTTPS_PORT" >> /opt/imashen/zerotier-webui/.env
fi

echo "zerotier-webui ENV CONFIGURATION: "
cat /opt/imashen/zerotier-webui/.env

mkdir -p /opt/imashen/zerotier-webui/etc/storage 
mkdir -p /opt/imashen/zerotier-webui/etc/tls
mkdir -p /opt/imashen/zerotier-webui/etc/myfs # for planet files

if [ ! -f /opt/imashen/zerotier-webui/etc/passwd ]; then
    echo "Default Password File Not Exists... Generating..."
    cd /opt/imashen/zerotier-webui/etc
    /usr/local/bin/argon2g 
    cd ../
fi

if [ ! -f /opt/imashen/zerotier-webui/etc/tls/fullchain.pem ] || [ ! -f /opt/imashen/zerotier-webui/etc/tls/privkey.pem ]; then
    echo "Cannot detect TLS Certs, Generating..."
    cd /opt/imashen/zerotier-webui/etc/tls
    /usr/local/bin/minica -domains "$MYDOMAIN"
    cp -f "$MYDOMAIN/cert.pem" fullchain.pem
    cp -f "$MYDOMAIN/key.pem" privkey.pem
    cd ../../
fi

chown -R zerotier-one:zerotier-one /opt/imashen/zerotier-webui
chmod 0755 /opt/imashen/zerotier-webui/dist/index.js

unset ZEROTIER_WEBUI_PASSWD
gosu zerotier-one:zerotier-one webui
