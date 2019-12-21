#!/bin/bash

docker rmi $(docker images --format '{{.Repository}}:{{.Tag}}' | grep -e 'os6_build' -e '<none>')

docker build -t os6_build .

docker run -it --rm --name os6 os6_build