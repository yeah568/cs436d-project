FROM ubuntu:bionic

RUN sudo apt update
RUN sudo apt install mesa-common-dev libgles2-mesa-dev libegl1-mesa-dev libglfw-dev libsdl2-dev libsdl2-mixer-dev libxrandr-dev libxi-dev 

RUN mkdir -p /beatcoin
WORKDIR /beatcoin
COPY . .

CMD ["make", "make test"]