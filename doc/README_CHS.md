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

快速导航：

[`搭建独立面板`](#usage "Go to definition")
[`环境变量`](#env "Go to definition")
[`文件服务器`](#fileserver "Go to definition")
[`搭建Moon/Planet服务器`](#mkmoon "Go to definition")

## <a id="usage">简易使用</a>

```
docker run -d -p 9993:9993/udp -p 3443:3443 -p 3180:3180 \
    -v zerotier-one:/var/lib/zerotier-one \
    -v zerotier-webui:/opt/imashen/zerotier-webui/etc \
    -v zerotier-logs:/var/log/zerotier-aio \
    --env-file ./denv \
    --name zerotier-aio \
    imashen/zerotier-aio
```
> 除此之外，您可以将数据卷改为本地目录挂载，引入环境变量的方式也可以改为-e引入。

`/var/lib/zerotier-one`用于存放zerotier默认的运行时和配置文件

![files-zt1](/doc/bash/files-zt1.png)

`/opt/imashen/zerotier-webui/etc`是文件服务器并存放有Web面板的证书

![files-webui](/doc/bash/files-webui.png)

`/var/log/zerotier-aio`存放日志文件

![files-logs](/doc/bash/files-logs.png)

## <a id="env">环境变量</a>

### 必需环境变量

| Name | 解释 | 默认值 |
|:--------:|:--------:|:--------:|
| NODE_ENV | 参考 https://pugjs.org/api/express.html | production |
| MYADDR | 您的IP地址，建议使用公网IP | 无默认值 |

### 可选环境变量

| Name | 解释 | 默认值 |
|:--------:|:--------:|:--------:|
| HTTPS_HOST | 仅监听在 HTTPS_HOST:HTTPS_PORT | 无默认值 |
| <a id="https_port">HTTPS_PORT</a> | HTTPS端口 | 3443 |
| <a id="http_port">HTTP_PORT</a> | HTTP端口 | 3000 |
| HTTP_ALL_INTERFACES | 监听所有接口，仅适用于HTTP，适用于反向代理 | 无默认值 |
| MYDOMAIN | 动态生成TLS证书的域名 | site.test |
| ZEROTIER-WEBUI_PASSWD | 动态生成的管理员密码 | password |

> 请注意，为数据传输的安全起见 [`HTTPS_PORT`](#https_port "Go to definition") 默认在全网口监听，[`HTTP_PORT`](#http_port "Go to definition") 仅本地 `localhost/127.0.0.1` 监听。

## <a id="fileserver">文件服务器</a>

此镜像在端口 [`3180`](#usage "Go to definition") 上开放了一个HTTP服务器，因此您可以在 `/myfs/` 中保存文件以进行服务。例如，您可以使用此功能构建自己的根服务器并分发行星文件。


## Moon/Planet便捷部署

您可以使用 `docker exec -it zerotier-aio /bin/bash` 进入容器

![exec_container](/doc/bash/exec_container.png)

执行 [`mkmoon`](#mkmoon "Go to definition") 或 [`mkplanet`](#mkplanet "Go to definition") 查看帮助


### <a id="mkmoon">1.Moon服务器搭建|mkmoon</a>
输入指令`mkmoon`获取帮助信息

![usage_mkmoon](/doc/bash/usage_mkmoon.png)

```
Usage: mkmoon [options]
Options:
  -h, --help            Display this help message
  -4 <IPv4>             Set the IPv4 address for the moon
  -6 <IPv6>             Set the IPv6 address for the moon
  -p <port>             Set the port for the moon (default is 9993)
```

其中v4/v6地址必须填入一个 每个参数都可以缺省 Port默认为9993

成功运行会得到下述效果:

![usage_mkmoon_out](/doc/bash/usage_mkmoon_out.png)


### <a id="mkplanet">2.Planet服务器搭建|mkplanet</a>
输入指令`mkplanet`获取帮助信息

![usage_mkplanet](/doc/bash/usage_mkplanet.png)

```
Usage: mkplanet [options]
Options:
  -h, --help          Display this help message
  -j2b, --json2bin      Convert from JSON file to planet
  -b2j, --bin2json      Convert from planet to JSON format
  -r, --reset              Reset planet from origin-planet.json
```

首先运行 `mkplanet -b2j` 或 `mkplanet --bin2json` 将官方自带的planet转义为json文件，方便修改

![usage_mkplanet_b2j](/doc/bash/usage_mkplanet_b2j.png)

在`planet.json`中按照固定结构填入你自己的Planet服务器信息，最多填入四个！！！

![planet_json](/doc/bash/planet_json.png)

配置完成后再次运行 `mkplanet -j2b` 或 `mkplanet --json2bin` 重新将json文件打包为planet

![usage_mkplanet_j2b](/doc/bash/usage_mkplanet_j2b.png)

此时的planet文件分发到各个设备中，即可实现自建Moon
为方便分发，详见[`文件服务器`](#fileserver "Go to definition")

此外mkplanet工具自带了一键恢复planet功能，如果你的配置文件不幸损坏或者丢失官方文件，可通过 `mkplanet -r` 或 `mkplanet --reset` 重新生成官方planet文件

![usage_mkplanet_reset](/doc/bash/usage_mkplanet_reset.png)