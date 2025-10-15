FROM ubuntu:latest
COPY . /usr/src/Dockertest
WORKDIR /usr/src/Dockertest
RUN apt update
RUN apt install -y g++
RUN g++ -o source Source.cpp
CMD ["./source"]