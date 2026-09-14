FROM ubuntu

RUN apt-get update && apt install -y cmake gcc make build-essential git

WORKDIR tmp
COPY . .

RUN mkdir build
RUN cd build && cmake .. && make && cp extract-xiso /bin

RUN rm -rf *