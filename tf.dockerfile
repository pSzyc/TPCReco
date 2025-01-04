FROM ubuntu:16.04

WORKDIR /app

# Install dependencies
RUN apt-get update && apt-get install -y \
    zip \
    build-essential \
    wget \
    libcurl3-dev \
    libfreetype6-dev \
    libhdf5-serial-dev \
    gcc \
    libzmq3-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Download TensorFlow
RUN version=2.18.0 && \
    cpu_or_gpu="cpu" && \
    echo "Download Tensorflow-$cpu_or_gpu-$version" && \
    tensorflow_link="https://storage.googleapis.com/tensorflow/versions/$version/libtensorflow-$cpu_or_gpu-linux-x86_64.tar.gz" && \
    wget -O /tmp/tensorflow.tar.gz $tensorflow_link && \
    echo "Extracting..." && \
    tar -C /usr/local -xzf /tmp/tensorflow.tar.gz && \
    ldconfig


# Install CMake 3.5.1
ARG cmake_file=cmake-3.5.1-Linux-x86_64
RUN wget -O "/tmp/$cmake_file.tar.gz" "https://cmake.org/files/v3.5/$cmake_file.tar.gz" && \
    tar -C /usr/local -xf "/tmp/$cmake_file.tar.gz" && \
    ln -s /usr/local/$cmake_file/bin/* /usr/local/bin/ && \
    rm -rf "/tmp/$cmmake_file.tar.gz"

ENV CMAKE_ROOT=/usr/local/$cmake_file

COPY ./Tensorflow /app

# Build the test
RUN cd /app && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make
