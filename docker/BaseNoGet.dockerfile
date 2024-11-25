FROM ubuntu:16.04 AS base_root

ARG ROOT_BIN=root_v6.08.00.Linux-ubuntu16-x86_64-gcc5.4.tar.gz

WORKDIR /opt

COPY packages_root packages

RUN apt-get update -qq \
 && ln -sf /usr/share/zoneinfo/UTC /etc/localtime \
 && apt-get -y install $(cat packages) wget \
 && rm -rf /var/lib/apt/lists/* \
 && wget https://root.cern/download/${ROOT_BIN} \
 && tar -xzvf ${ROOT_BIN} \
 && rm -f ${ROOT_BIN} \
 && rm -f packages \
 && echo /opt/root/lib >> /etc/ld.so.conf \
 && ldconfig

ENV ROOTSYS=/opt/root
ENV PATH=$ROOTSYS/bin:$PATH
ENV CLING_STANDARD_PCH=none