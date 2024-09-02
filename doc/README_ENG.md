# README.md
- ENG [English](doc/README_ENG.md)
- CHS [简体中文](doc/README_CHS.md)
- CHT [繁體中文](doc/README_CHT.md)

# Zerotier All in One

![GitHub License](https://img.shields.io/github/license/imashen/zerotier-aio)
![Docker Automated build](https://img.shields.io/docker/automated/imashen/zerotier-aio)
![Current Release](https://img.shields.io/github/v/release/imashen/zerotier-aio.svg)
![GitHub stars](https://img.shields.io/github/stars/imashen/zerotier-aio?style=social)
![GitHub forks](https://img.shields.io/github/forks/imashen/zerotier-aio?style=social)
![Docker Pulls](https://img.shields.io/docker/pulls/imashen/zerotier-aio)
![Docker Image Size](https://img.shields.io/docker/image-size/imashen/zerotier-aio/latest)
![GitHub issues](https://img.shields.io/github/issues/imashen/zerotier-aio)
![GitHub closed issues](https://img.shields.io/github/issues-closed/imashen/zerotier-aio)
![GitHub contributors](https://img.shields.io/github/contributors/imashen/zerotier-aio)

Quick Navigation:

[`Setup Standalone Panel`](#usage "Go to definition")
[`Environment Variables`](#env "Go to definition")
[`File Server`](#fileserver "Go to definition")
[`Setup Moon/Planet Server`](#mkmoon "Go to definition")

****

## <a id="usage">Simple Usage</a>

```
docker run -d -p 9993:9993/udp -p 3443:3443 -p 3180:3180 \
    -v zerotier-one:/var/lib/zerotier-one \
    -v zerotier-webui:/www/zerotier-webui/etc \
    -v zerotier-logs:/logs \
    --env-file ./denv \
    --name zerotier-aio \
    imashen/zerotier-aio
```

> Additionally, you can change the data volume to be mounted as a local directory, and the method of introducing environment variables can also be changed to use the `-e` option. For the contents of the `--env-file` environment variable file, please refer to the `denv` in the repository.

```
docker run -d -p 9993:9993/udp -p 3443:3443 -p 3180:3180 \
    -v /opt/zerotier/zerotier-one:/var/lib/zerotier-one \
    -v /opt/zerotier/zerotier-webui:/www/zerotier-webui/etc \
    -v /opt/zerotier/zerotier-logs:/logs \
    -e NODE_ENV=production \
    -e ZEROTIER-WEBUI_PASSWD=password123 \
    -e MYDOMAIN=site.test \
    --name zerotier-aio \
    imashen/zerotier-aio
```

### Files Func

`/var/lib/zerotier-one` is used to store Zerotier's default runtime and configuration files.

![files-zt1](/doc/bash/files-zt1.png)

`/www/zerotier-webui/etc` is the file server and contains the certificates for the Web UI.

![files-webui](/doc/bash/files-webui.png)

`/logs` stores the log files.

![files-logs](/doc/bash/files-logs.png)

****

## <a id="env">Environment Variables</a>

### Required Environment Variables

| Name | Description | Default Value |
|:--------:|:--------:|:--------:|
| NODE_ENV | Refer to https://pugjs.org/api/express.html | production |
| MYADDR | Your IP address, preferably a public IP | No default value |

### Optional Environment Variables

| Name | Description | Default Value |
|:--------:|:--------:|:--------:|
| HTTPS_HOST | Listen only on HTTPS_HOST:HTTPS_PORT | No default value |
| <a id="https_port">HTTPS_PORT</a> | HTTPS Port | 3443 |
| <a id="http_port">HTTP_PORT</a> | HTTP Port | 3000 |
| HTTP_ALL_INTERFACES | Listen on all interfaces, applicable only to HTTP, suitable for reverse proxy | No default value |
| MYDOMAIN | Domain name for dynamically generated TLS certificates | site.test(can also be the local IP) |
| ZEROTIER-WEBUI_PASSWD | Password for the dynamically generated admin | password123(Weak passwords are dynamically generated, as detailed in the logs) |

> Please note that for security, [`HTTPS_PORT`](#https_port "Go to definition") listens on all network interfaces by default, while [`HTTP_PORT`](#http_port "Go to definition") listens only on `localhost/127.0.0.1`.

****

## <a id="fileserver">File Server</a>

This image opens an HTTP server on port [`3180`](#usage "Go to definition"), allowing you to serve files stored in `/myfs/`. For example, you can use this to build your own root server and distribute planet files.

****

## Moon/Planet Easy Deployment

You can enter the container using `docker exec -it zerotier-aio /bin/bash`.

![exec_container](/doc/bash/exec_container.png)

Run [`mkmoon`](#mkmoon "Go to definition") or [`mkplanet`](#mkplanet "Go to definition") to view help.

****

### <a id="mkmoon">1. Setup Moon Server | mkmoon</a>
Enter the command `mkmoon` to get help information.

![usage_mkmoon](/doc/bash/usage_mkmoon.png)

```
Usage: mkmoon [options]
Options:
  -h, --help            Display this help message
  -4 <IPv4>             Set the IPv4 address for the moon
  -6 <IPv6>             Set the IPv6 address for the moon
  -p <port>             Set the port for the moon (default is 9993)
```

You must provide either an IPv4 or IPv6 address. Each parameter can be omitted. The default port is 9993.

The following is an example of a successful operation:

![usage_mkmoon_out](/doc/bash/usage_mkmoon_out.png)

****

### <a id="mkplanet">2. Setup Planet Server | mkplanet</a>
Enter the command `mkplanet` to get help information.

![usage_mkplanet](/doc/bash/usage_mkplanet.png)

```
Usage: mkplanet [options]
Options:
  -h, --help          Display this help message
  -j2b, --json2bin      Convert from JSON file to planet
  -b2j, --bin2json      Convert from planet to JSON format
  -r, --reset              Reset planet from origin-planet.json
```

First, run `mkplanet -b2j` or `mkplanet --bin2json` to convert the official planet file to a JSON file for easy modification.

![usage_mkplanet_b2j](/doc/bash/usage_mkplanet_b2j.png)

Fill in your own Planet server information in the `planet.json` file following the specified structure. You can fill in up to four entries.

![planet_json](/doc/bash/planet_json.png)

After configuring, run `mkplanet -j2b` or `mkplanet --json2bin` to repackage the JSON file into a planet file.

![usage_mkplanet_j2b](/doc/bash/usage_mkplanet_j2b.png)

Distribute this planet file to your devices to set up your own Moon server. For easier distribution, see [`File Server`](#fileserver "Go to definition").

Additionally, the `mkplanet` tool includes a one-click reset feature. If your configuration file is corrupted or you lose the official file, </br>you can use `mkplanet -r` or `mkplanet --reset` to regenerate the official planet file.

![usage_mkplanet_reset](/doc/bash/usage_mkplanet_reset.png)