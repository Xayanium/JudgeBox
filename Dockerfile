FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y ca-certificates

RUN sed -i 's@http://.*archive.ubuntu.com@http://mirrors.tuna.tsinghua.edu.cn@g' /etc/apt/sources.list && \
    sed -i 's@http://.*security.ubuntu.com@http://mirrors.tuna.tsinghua.edu.cn@g' /etc/apt/sources.list && \
    apt-get clean && \
    apt-get update

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y \
    build-essential \
    cmake \
    make \
    gcc \
    g++ \
    git \
    curl \
    wget \
    pkg-config \
    software-properties-common \
    #vim \
    && rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

RUN pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple && \
    pip config set global.trusted-host pypi.tuna.tsinghua.edu.cn

RUN apt-get update && apt-get install -y \
    openjdk-17-jdk \
    && rm -rf /var/lib/apt/lists/*

RUN wget https://mirrors.aliyun.com/golang/go1.22.9.linux-amd64.tar.gz \
    && tar -C /usr/local -xzf go1.22.9.linux-amd64.tar.gz \
    && rm go1.22.9.linux-amd64.tar.gz

ENV PATH=$PATH:/usr/local/go/bin
ENV GOPATH=/go
ENV PATH=$PATH:$GOPATH/bin

WORKDIR /app

COPY . /app

RUN mkdir -p /app/problem
RUN mkdir -p /app/build

RUN if [ -f "requirements.txt" ]; then pip3 install --break-system-packages -r requirements.txt; fi

WORKDIR /app/build
RUN if [ -f "CMakeLists.txt"]; then \
    cd /app/build && \
    cmake .. && \
    make; \
    fi

WORKDIR /app
CMD ["python3", "main.py"]
#RUN /bin/bash
