FROM debian:trixie-backports AS build

RUN apt-get update && apt-get install -y \
    build-essential \
    libtbb-dev \
    libasio-dev \
    libsqlite3-dev \
    openssl \
    libspdlog-dev \
    cmake/trixie-backports

WORKDIR /build

COPY . .

RUN mkdir build && cd build && cmake .. && make

FROM debian:trixie-backports

WORKDIR /app

RUN apt-get update && apt-get install -y \
    libtbb12 \
    libsqlite3-0 \
    openssl \
    libfmt10 \
    && rm -rf /var/lib/apt/lists/*
    
COPY --from=build /build/build/MinKV /app/MinKV

CMD ["/app/MinKV"]
