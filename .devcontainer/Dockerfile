# Use the base image with Python and the specified version
FROM mcr.microsoft.com/devcontainers/python:1-3.11-bookworm

# The base image's Yarn apt source is signed with a key the image lacks, which breaks
# apt-get update; nothing here uses Yarn.
RUN rm -f /etc/apt/sources.list.d/yarn.list && \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y \
    ca-certificates \
    wget \
    tar \
    dpkg \
    libudev-dev \
    libhdf5-serial-dev && \
    rm -rf /var/lib/apt/lists/*

# Install arm gcc toolchain
RUN TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/10.3-2021.10/gcc-arm-none-eabi-10.3-2021.10-aarch64-linux.tar.bz" && \
    wget $TOOLCHAIN_URL -O /tmp/arm-gnu-toolchain.tar.xz && \
    tar -xvf /tmp/arm-gnu-toolchain.tar.xz -C /opt && \
    rm /tmp/arm-gnu-toolchain.tar.xz

    
# Set the working directory
WORKDIR /workspace