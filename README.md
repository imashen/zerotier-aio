# Zerotier-Server

## Usage

```
docker run -d -p 9993:9993/udp -p 3443:3443 -p 3180:3180 \
    -v zerotier-one:/var/lib/zerotier-one \
    -v zerotier-webui:/opt/imashen/zerotier-webui/etc \
    -v zerotier-logs:/var/log/zerotier-server \
    --env-file ./denv \
    --name zerotier-server \
    imashen/zerotier-server
```
## Note

| REQUIRED | Name | Explanation | Default Value |
|:--------:|:--------:|:--------:|:--------:|
| YES | NODE_ENV | https://pugjs.org/api/express.html | production |
| no | HTTPS_HOST | Only Listen on HTTPS_HOST:HTTPS_PORT | NO DEFAULT |
| no | HTTPS_PORT | HTTPS_PORT | 3443 |
| no | HTTP_PORT | HTTP_PORT | 3000 |
| no | HTTP_ALL_INTERFACES | Listen on all interfaces, useful for reverse proxy, HTTP only | NO DEFAULT |

Additional environment variables used in this Docker image:

| REQUIRED | Name | Explanation | Default Value |
|:--------:|:--------:|:--------:|:--------:|
| no | MYDOMAIN | generate TLS certs on the fly | site.test |
| no | ZEROTIER-WEBUI_PASSWD | generate admin password on the fly | password |
| YES | MYADDR | your ip address, public ip address preferred | NO DEFAULT |


This image exposes an http server at port `3180`, so you could save a file in `/myfs/` to serve it. For example, you could use this to build your own root server and distribute a planet file.

> Please note that once the `HTTPS_PORT` is set, the `HTTP_PORT` will no longer be accessible.

## More Func
In addition to this, you can use the `docker exec -it zerotier-server /bin/bash` to enter the container
and then execute `./gen/moon.sh` or `./gen/planet.sh` to quickly and easily make your server a moon or planet server.