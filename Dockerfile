FROM ubuntu:22.04 AS lindbergh-build

RUN dpkg --add-architecture i386 \
    && apt update \
    && apt install -y --no-install-recommends \
        build-essential \
        gcc-multilib \
        freeglut3-dev:i386 \
        libsdl2-dev:i386 \
        libfaudio-dev:i386 \
    && apt clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /lindbergh-loader

COPY . .

# Explicitly set the output directory
RUN make && mkdir -p /output && cp -r ./build /output
