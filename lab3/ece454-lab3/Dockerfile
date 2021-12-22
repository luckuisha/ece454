FROM ubuntu:focal

# Install packages
RUN apt update && \
    DEBIAN_FRONTEND=noninteractive apt install -y \
        vim \
        python3-pip \
        cmake \
        build-essential \
        htop

RUN mkdir -p /root/ECE454-Lab3/
COPY . /root/ECE454-Lab3/
RUN rm /root/ECE454-Lab3/src/mm.c