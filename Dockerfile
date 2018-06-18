FROM ubuntu:16.04

RUN apt-get update \
&& apt-get install -y gcc g++ python \
&& apt-get clean

COPY ./src_docker /usr/src

WORKDIR /usr/src
RUN cd ntl-11.0.0 && make && make check && make install

RUN ./waf configure PREFIX=$HOME/latsoft \
&& ./waf install
