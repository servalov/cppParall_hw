FROM ubuntu:latest
RUN apt update && apt install -y clang
RUN mkdir /usr/src/query_clients  
WORKDIR /usr/src/query_clients
COPY . /usr/src/query_clients
RUN clang++ -o source Source.cpp
CMD ["./source"]