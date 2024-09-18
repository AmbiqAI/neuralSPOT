# Use the base image with Python and the specified version
FROM mcr.microsoft.com/devcontainers/python:1-3.11-bullseye

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y \
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

# Update PATH
# ENV PATH="/opt/gcc-arm-none-eabi-10.3-2021.10/bin:${PATH}"

# Set the working directory
WORKDIR /workspace