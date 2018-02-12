FROM ubuntu:bionic

# RUN apt update
RUN git clone https://github.com/glfw/glfw && mkdir build && cd build && cmake ../glfw && make -j4 && sudo make install
RUN apt -y install mesa-common-dev libgles2-mesa-dev libegl1-mesa-dev libsdl2-dev libsdl2-mixer-dev libxrandr-dev libxi-dev 

RUN mkdir -p /beatcoin
WORKDIR /beatcoin
COPY . .
