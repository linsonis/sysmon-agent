FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    make \
    libncurses-dev \
    libncursesw5-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir build && cd build && cmake .. && make -j$(nproc)

FROM ubuntu:22.04
RUN apt-get update && apt-get install -y \
    libncursesw5 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/sysmon /usr/local/bin/sysmon
COPY deploy/config.yaml /etc/sysmon/config.yaml

EXPOSE 9100
ENTRYPOINT ["/usr/local/bin/sysmon", "--service"]
