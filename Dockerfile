FROM debian:bullseye-slim AS builder

ENV NODEJS_MAJOR=20

ARG DEBIAN_FRONTEND=noninteractive
LABEL MAINTAINER="Hutu Tech | imashen"
LABEL Description="ZEROTIER ONE + ZEROTIER WEB UI"

# BUILD zerotier-webui IN FIRST STAGE
WORKDIR /build
RUN apt update -y && \
    apt install -y --no-install-recommends \
    curl gnupg2 ca-certificates zip unzip build-essential git && \
    curl -sL https://deb.nodesource.com/setup_${NODEJS_MAJOR}.x | bash - && \
    apt install -y --no-install-recommends nodejs && \
    git clone https://github.com/imashen/zerotier-webui && \
    cd zerotier-webui/src && \
    npm install && \
    npm run build && \
    zip -r /build/artifact.zip dist node_modules views public etc && \
    apt clean && rm -rf /var/lib/apt/lists/*
# Clone zerotier-world-generator
RUN mkdir -p /var/lib/zerotier-one && \
    cd /var/lib/zerotier-one && \
    git clone https://github.com/imashen/zerotier-world-generator.git

# BUILD GO UTILS
FROM golang:1.22-bullseye AS utilsbuilder
WORKDIR /buildsrc
COPY plugins/argon2g /buildsrc/argon2g
COPY plugins/fileserv /buildsrc/fileserv
RUN mkdir -p binaries && \
    cd argon2g && \
    go mod download && \
    go build -ldflags='-s -w' -trimpath -o ../binaries/argon2g && \
    cd .. && \
    git clone https://github.com/jsha/minica && \
    cd minica && \
    go mod download && \
    go build -ldflags='-s -w' -trimpath -o ../binaries/minica && \
    cd .. && \
    git clone https://github.com/tianon/gosu && \
    cd gosu && \
    go mod download && \
    go build -o ../binaries/gosu -ldflags='-s -w' -trimpath && \
    cd .. && \
    cd fileserv && \
    go build -ldflags='-s -w' -trimpath -o ../binaries/fileserv main.go

# START RUNNER
FROM debian:bullseye-slim AS runner
RUN apt update -y && \
    apt install -y --no-install-recommends \
    curl gnupg2 ca-certificates unzip supervisor net-tools procps && \
    groupadd -g 2222 zerotier-one && \
    useradd -u 2222 -g 2222 zerotier-one && \
    curl -sL https://install.zerotier.com | bash && \
    apt clean && rm -rf /var/lib/apt/lists/*

WORKDIR /opt/imashen/zerotier-webui
COPY --from=builder /build/artifact.zip .
RUN unzip ./artifact.zip && rm -f ./artifact.zip

COPY --from=utilsbuilder /buildsrc/binaries/* /usr/local/bin/
COPY --from=builder /var/lib/zerotier-one/zerotier-world-generator/ /var/lib/zerotier-one/zerotier-world-generator/

COPY start_zerotierone.sh /start_zerotierone.sh
COPY start_zerotier-webui.sh /start_zerotier-webui.sh
COPY supervisord.conf /etc/supervisord.conf

COPY world-generator.sh /usr/local/bin/world-generator
RUN chmod +x /usr/local/bin/world-generator

RUN chmod 0755 /usr/local/bin/* && \
    chmod 0755 /start_*.sh

EXPOSE 3000/tcp 3180/tcp 8000/tcp 3443/tcp 9993/udp

WORKDIR /

VOLUME ["/opt/imashen/zerotier-webui/etc", "/var/lib/zerotier-one"]

ENTRYPOINT ["/usr/bin/supervisord"]