# Linux build image. The "dev" stage is also used by the devcontainer.
FROM ubuntu:24.04 AS build

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        ninja-build \
        qt6-base-dev \
        qt6-svg-dev \
        libgl-dev \
        qt6-qpa-plugins \
    && rm -rf /var/lib/apt/lists/*

CMD ["/src/docker/linux/build.sh"]

FROM build AS dev

ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        clangd \
        clang-format \
        gdb \
        git \
        ca-certificates \
        qt6-wayland \
        libxkbcommon-x11-0 \
        libxcb-cursor0 \
        sudo \
    && rm -rf /var/lib/apt/lists/* \
    && echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu

USER ubuntu
