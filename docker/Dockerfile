FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    nasm \
    grub-pc-bin \
    grub-common \
    xorriso \
    qemu-system-x86 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
