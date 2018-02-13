FROM ubuntu:bionic

RUN apt update
RUN apt -y install g++ mesa-common-dev libgles2-mesa-dev libegl1-mesa-dev libsdl2-dev libsdl2-mixer-dev libxrandr-dev libxi-dev 

RUN mkdir -p /beatcoin
WORKDIR /beatcoin
COPY . .
