FROM ubuntu:bionic

RUN apt update
RUN apt -y install git cmake mesa-common-dev libgles2-mesa-dev libegl1-mesa-dev libsdl2-dev libsdl2-mixer-dev libxrandr-dev libxi-dev 
RUN git clone https://github.com/glfw/glfw && mkdir build && cd build && cmake ../glfw && make -j4 && make install

RUN mkdir -p /beatcoin
WORKDIR /beatcoin
COPY . .
