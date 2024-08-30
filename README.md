# README.md
- CHS [简体中文](doc/README_CHS.md)
- CHT [繁体中文](doc/README_CHT.md)
- EN [English](doc/README_EN.md)



# Zerotier All in One

![GitHub License](https://img.shields.io/github/license/imashen/zerotier-server)
![Docker Automated build](https://img.shields.io/docker/automated/imashen/zerotier-server)
![Current Release](https://img.shields.io/github/v/release/imashen/zerotier-server.svg)

快速导航：

[`搭建独立面板`](#usage "Go to definition")
[`环境变量`](#env "Go to definition")
[`文件服务器`](#fileserver "Go to definition")
[`搭建Moon/Planet服务器`](#mkmoon "Go to definition")

## <a id="usage">简易使用</a>

```
docker run -d -p 9993:9993/udp -p 3443:3443 -p <a id="3180">3180:3180</a> \
    -v zerotier-one:/var/lib/zerotier-one \
    -v zerotier-webui:/opt/imashen/zerotier-webui/etc \
    -v zerotier-logs:/var/log/zerotier-server \
    --env-file ./denv \
    --name zerotier-server \
    imashen/zerotier-server
```
> 除此之外，您可以将数据卷改为本地目录挂载，引入环境变量的方式也可以改为-e引入。

`/var/lib/zerotier-one`用于存放zerotier默认的运行时和配置文件

![files-zt1](/doc/bash/files-zt1.png)

`/opt/imashen/zerotier-webui/etc`是文件服务器并存放有Web面板的证书

![files-webui](/doc/bash/files-webui.png)

`/var/log/zerotier-server`存放日志文件

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

> 请注意，为数据传输的安全起见 [`HTTPS_PORT`](#https_port "Go to definition")默认在全网口监听，[`HTTP_PORT`](#http_port "Go to definition") 仅本地 `localhost/127.0.0.1` 监听。

## <a id="fileserver">文件服务器</a>

此镜像在端口 [`3180`](#3180 "Go to definition") 上开放了一个HTTP服务器，因此您可以在 `/myfs/` 中保存文件以进行服务。例如，您可以使用此功能构建自己的根服务器并分发行星文件。


## Moon/Planet便捷部署

您可以使用 `docker exec -it zerotier-server /bin/bash` 进入容器

![exec_container](/doc/bash/exec_container.png)

执行 [`mkmoon`](#mkmoon "Go to definition") 或 [`mkplanet`](#mkplanet "Go to definition")查看帮助


### <a id="mkmoon">1.mkmoon</a>
![usage_mkplanet](/doc/bash/usage_mkplanet.png)
Usage: mkplanet [options]
Options:
  -h, --help          Display this help message
  -j2b, --json2bin      Convert from JSON file to planet
  -b2j, --bin2json      Convert from planet to JSON format
  -r, --reset              Reset planet from origin-planet.json

### <a id="mkplanet">2.mkplanet</a>
