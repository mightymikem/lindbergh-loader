FROM debian:bullseye AS lindbergh-build

RUN dpkg --add-architecture i386 \
    && apt-get update \
    && apt-get install -y --no-install-recommends \
        build-essential \
        gcc-multilib \
        freeglut3-dev:i386 \
        libsdl2-dev:i386 \
        libfaudio-dev:i386 \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /lindbergh-loader
COPY . .

RUN make
