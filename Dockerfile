FROM ubuntu:latest

ADD toolchain/OS6_cross_cc.tar.gz /opt

ARG DEBIAN_FRONTEND="noninteractive"
RUN apt-get update && apt-get install -y --no-install-suggests --no-install-recommends \
    clang-format \
    clang-tidy \
    cmake \
    doxygen \
    git \
    gcc-10 \
    g++-10 \
    grub-pc-bin \
    make \
    mtools \
    nasm \
    sudo \
    qemu-system-x86 \
    qemu-utils \
    wget \
    xorriso

RUN groupadd builder
RUN useradd -g builder -G sudo builder && passwd -d builder

RUN mkdir /workspace
RUN chown -R builder:builder /workspace
USER builder
ADD . /workspace
WORKDIR /workspace

CMD ["bash"]
