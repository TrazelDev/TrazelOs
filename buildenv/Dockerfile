# Getting the base ubntu image:
FROM ubuntu:latest

# Updating the system and installing essentials:
RUN apt-get update && apt-get install -y build-essential

# Installing regular important tools
RUN apt-get -y install nasm

# Installing essential tools for a cross compiler:
RUN apt-get install -y bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev

RUN mkdir /mnt/TrazelOs
WORKDIR /mnt/TrazelOs
