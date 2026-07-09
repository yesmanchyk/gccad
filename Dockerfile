FROM --platform=linux/arm64 arm64v8/gcc:16.1.0

RUN apt-get update
RUN apt-get install -y wget screen flex bison dejagnu gperf neovim

# docker build -t gccb .
# docker run --rm -it -v `pwd`/..:/src gccb

