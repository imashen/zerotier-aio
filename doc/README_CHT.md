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

[`搭建獨立面板`](#usage "Go to definition")
[`環境變量`](#env "Go to definition")
[`文件服務器`](#fileserver "Go to definition")
[`搭建Moon/Planet服務器`](#mkmoon "Go to definition")

****

## <a id="usage">簡易使用</a>

```
docker run -d -p 9993:9993/udp -p 3443:3443 -p 3180:3180 \
    -v zerotier-one:/var/lib/zerotier-one \
    -v zerotier-webui:/opt/imashen/zerotier-webui/etc \
    -v zerotier-logs:/var/log/zerotier-aio \
    --env-file ./denv \
    --name zerotier-aio \
    imashen/zerotier-aio
```

> 除此之外，您可以將數據卷改為本地目錄掛載，引入環境變量的方式也可以改為-e引入 --env-file環境變量文件內容詳見倉庫中的denv

```
docker run -d -p 9993:9993/udp -p 3443:3443 -p 3180:3180 \
    -v /opt/zerotier/zerotier-one:/var/lib/zerotier-one \
    -v /opt/zerotier/zerotier-webui:/opt/imashen/zerotier-webui/etc \
    -v /opt/zerotier/zerotier-logs:/var/log/zerotier-aio \
    -e NODE_ENV=production \
    -e ZEROTIER-WEBUI_PASSWD=password \
    -e MYDOMAIN=site.test \
    --name zerotier-aio \
    imashen/zerotier-aio
```

`/var/lib/zerotier-one`用於存放zerotier默認的運行時和配置文件

![files-zt1](/doc/bash/files-zt1.png)

`/opt/imashen/zerotier-webui/etc`是文件服務器並存放有Web面板的證書

![files-webui](/doc/bash/files-webui.png)

`/var/log/zerotier-aio`存放日志文件

![files-logs](/doc/bash/files-logs.png)

****

## <a id="env">环境变量</a>

### 必需环境变量

| Name | 解釋 | 默認值 |
|:--------:|:--------:|:--------:|
| NODE_ENV | 參考 https://pugjs.org/api/express.html | production |
| MYADDR | 您的IP地址，建議使用公網IP | 無默認值 |

### 可選環境變量

| Name | 解釋 | 默認值 |
|:--------:|:--------:|:--------:|
| HTTPS_HOST | 僅監聽在 HTTPS_HOST:HTTPS_PORT | 無默認值 |
| <a id="https_port">HTTPS_PORT</a> | HTTPS端口 | 3443 |
| <a id="http_port">HTTP_PORT</a> | HTTP端口 | 3000 |
| HTTP_ALL_INTERFACES | 監聽所有接口，僅適用於HTTP，適用於反向代理 | 無默認值 |
| MYDOMAIN | 動態生成TLS證書的域名 | site.test(亦可是本機IP) |
| ZEROTIER-WEBUI_PASSWD | 動態生成的管理員密碼 | password(新版鏡像可能會動態生成，詳見日誌) |

> 請注意，爲數據傳輸的安全起見 [`HTTPS_PORT`](#https_port "Go to definition") 默認在全網口監聽，[`HTTP_PORT`](#http_port "Go to definition") 僅本地 `localhost/127.0.0.1` 監聽。

****

## <a id="fileserver">文件服務器</a>

此鏡像在端口 [`3180`](#usage "Go to definition") 上開放了一個HTTP服務器，因此您可以在 `/myfs/` 中保存文件以進行服務。例如，您可以使用此功能構建自己的根服務器並分發行星文件。

****

## Moon/Planet便捷部署

您可以使用 `docker exec -it zerotier-aio /bin/bash` 進入容器

![exec_container](/doc/bash/exec_container.png)

執行 [`mkmoon`](#mkmoon "Go to definition") 或 [`mkplanet`](#mkplanet "Go to definition") 查看幫助

****

### <a id="mkmoon">1.Moon服務器搭建|mkmoon</a>
輸入指令`mkmoon`獲取幫助信息

![usage_mkmoon](/doc/bash/usage_mkmoon.png)

```
Usage: mkmoon [options]
Options:
  -h, --help            Display this help message
  -4 <IPv4>             Set the IPv4 address for the moon
  -6 <IPv6>             Set the IPv6 address for the moon
  -p <port>             Set the port for the moon (default is 9993)
```

其中v4/v6地址必須填入一個 每個參數都可以缺省 Port默認爲9993

成功運行會得到下述效果:

![usage_mkmoon_out](/doc/bash/usage_mkmoon_out.png)

****

### <a id="mkplanet">2.Planet服務器搭建|mkplanet</a>
輸入指令`mkplanet`獲取幫助信息

![usage_mkplanet](/doc/bash/usage_mkplanet.png)

```
Usage: mkplanet [options]
Options:
  -h, --help          Display this help message
  -j2b, --json2bin      Convert from JSON file to planet
  -b2j, --bin2json      Convert from planet to JSON format
  -r, --reset              Reset planet from origin-planet.json
```

首先運行 `mkplanet -b2j` 或 `mkplanet --bin2json` 將官方自帶的planet轉義爲json文件，方便修改

![usage_mkplanet_b2j](/doc/bash/usage_mkplanet_b2j.png)

在`planet.json`中按照固定結構填入你自己的Planet服務器信息，最多填入四個！！！

![planet_json](/doc/bash/planet_json.png)

配置完成後再次運行 `mkplanet -j2b` 或 `mkplanet --json2bin` 重新將json文件打包爲planet
![usage_mkplanet_j2b](/doc/bash/usage_mkplanet_j2b.png)

此時的planet文件分發到各個設備中，即可實現自建Moon
爲方便分發，詳見[`文件服務器`](#fileserver "Go to definition")

此外mkplanet工具自帶了一鍵恢復planet功能，如果你的配置文件不幸損壞或者丟失官方文件，</br>可通過 `mkplanet -r` 或 `mkplanet --reset` 重新生成官方planet文件

![usage_mkplanet_reset](/doc/bash/usage_mkplanet_reset.png)