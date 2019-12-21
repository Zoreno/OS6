FROM ubuntu:18.04

RUN apt-get update 

RUN mkdir -p /OS6

COPY . /OS6

WORKDIR /OS6

CMD ["/bin/bash"]