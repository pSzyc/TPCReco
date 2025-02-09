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
ENV LD_LIBRARY_PATH=$ROOTSYS/lib


RUN apt-get update -qq \
    && apt-get -y install git \
    && rm -rf /var/lib/apt/lists/*
ARG GEANT4_VERSION=10.4.3
ARG CADMESH_VERSION=1.1
RUN wget https://github.com/Geant4/geant4/archive/refs/tags/v${GEANT4_VERSION}.tar.gz -O geant.tar.gz \
    && tar xf geant.tar.gz && rm geant.tar.gz
RUN cmake -S geant4-${GEANT4_VERSION} -Bbuild \
    -DGEANT4_INSTALL_DATA=ON -DCMAKE_INSTALL_PREFIX=/usr/local/ \
    && cmake --build build --target install -- -j 10 \
    && rm -r build geant4-${GEANT4_VERSION} \
    && ldconfig

RUN wget https://github.com/christopherpoole/CADMesh/archive/refs/tags/v${CADMESH_VERSION}.tar.gz -O cadmesh.tar.gz \
    && tar xf cadmesh.tar.gz && rm cadmesh.tar.gz
RUN cmake -S CADMesh-${CADMESH_VERSION} -Bbuild \
    -DCMAKE_INSTALL_PREFIX=/usr/local/ \
    && cmake --build build --target install \
    && rm -r build CADMesh-${CADMESH_VERSION} \
    && ldconfig

# grrr source geant4.sh is configured on build
ENV G4LEVELGAMMADATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/PhotonEvaporation5.2
ENV G4NEUTRONXSDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/G4NEUTRONXS1.4
ENV G4LEDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/G4EMLOW7.3
ENV G4NEUTRONHPDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/G4NDL4.5
ENV G4RADIOACTIVEDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/RadioactiveDecay5.2
ENV G4ENSDFSTATEDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/G4ENSDFSTATE2.2
ENV G4ABLADATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/G4ABLA3.1
ENV G4PIIDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/G4PII1.3
ENV G4SAIDXSDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/G4SAIDDATA1.1
ENV G4REALSURFACEDATA=/usr/local/share/Geant4-${GEANT4_VERSION}/data/RealSurface2.1.1

COPY requirements_apt.txt /tmp
COPY requirements_pip3.txt /tmp
RUN apt-get update -qq \
    && apt-get -y install $(cat /tmp/requirements_apt.txt) \ 
    less vim emacs nano \
    && rm -rf /var/lib/apt/lists/*
RUN yes | pip3 install --no-cache-dir -r /tmp/requirements_pip3.txt
RUN rm /tmp/requirements_*.txt

RUN version=2.18.0 && \
    cpu_or_gpu="cpu" && \
    echo "Download Tensorflow-$cpu_or_gpu-$version" && \
    tensorflow_link="https://storage.googleapis.com/tensorflow/versions/$version/libtensorflow-$cpu_or_gpu-linux-x86_64.tar.gz" && \
    wget -O /tmp/tensorflow.tar.gz $tensorflow_link && \
    echo "Extracting..." && \
    tar -C /usr/local -xzf /tmp/tensorflow.tar.gz && \
    ldconfig
