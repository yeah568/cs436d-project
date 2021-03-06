FROM ubuntu:bionic

RUN apt update
RUN apt -y install g++ libglfw3-dev mesa-common-dev libgles2-mesa-dev libegl1-mesa-dev libsdl2-dev libsdl2-mixer-dev libxrandr-dev libxi-dev libfreetype6-dev

RUN mkdir -p /beatcoin
WORKDIR /beatcoin
COPY . .
