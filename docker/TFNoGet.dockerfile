FROM ubuntu:24.04

ARG ROOT_BIN=root_v6.34.00-rc1.Linux-ubuntu24.10-x86_64-gcc14.2.tar.gz
WORKDIR /tmp

COPY root_packages packages

RUN apt-get update -qq \
 && ln -sf /usr/share/zoneinfo/UTC /etc/localtime \
 && apt-get -y install $(cat packages) wget \
 && apt install unminimize \
 && rm -rf /var/lib/apt/lists/* \
 && wget https://root.cern/download/${ROOT_BIN} \
 && tar -xzvf ${ROOT_BIN} \
 && rm -f ${ROOT_BIN} \
 && mv root /usr/local/root \
 && echo /usr/local/root/lib >> /etc/ld.so.conf \
 && ldconfig
RUN yes | unminimize

ENV ROOTSYS /usr/local/root
ENV PATH $ROOTSYS/bin:$PATH
ENV PYTHONPATH $ROOTSYS/lib:$PYTHONPATH
ENV CLING_STANDARD_PCH none

ARG GEANT4_VERSION=11.3.0
RUN wget https://github.com/Geant4/geant4/archive/refs/tags/v${GEANT4_VERSION}.tar.gz -O geant.tar.gz \
    && tar xf geant.tar.gz && rm geant.tar.gz
RUN cmake -S geant4-${GEANT4_VERSION} -Bbuild \
    -DGEANT4_INSTALL_DATA=ON -DCMAKE_INSTALL_PREFIX=/usr/local/ \
    && cmake --build build --target install -- -j 10 \
    && rm -r build geant4-${GEANT4_VERSION} \
    && ldconfig

WORKDIR /app