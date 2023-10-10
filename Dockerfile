FROM gcc:latest

WORKDIR /app

COPY srcs/Server/CMakeLists.txt /app/
COPY srcs/Server /app/
COPY includes/ /app/include/

RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    git \
    curl \
    zip \
    unzip \
    tar

RUN git clone https://github.com/microsoft/vcpkg.git /app/vcpkg
RUN /app/vcpkg/bootstrap-vcpkg.sh

RUN /app/vcpkg/vcpkg install asio

ARG TARGET_PLATFORM
ENV PATH_ASIO=""
RUN if [ "$TARGET_PLATFORM" = "win32" ]; then \
        export PATH_ASIO="/app/vcpkg/packages/asio_x64-windows/include"; \
    elif [ "$TARGET_PLATFORM" = "unix" ]; then \
        export PATH_ASIO="/app/vcpkg/packages/asio_x64-linux/include"; \
    elif [ "$TARGET_PLATFORM" = "apple" ]; then \
        export PATH_ASIO="/app/vcpkg/packages/asio_x64-osx/include"; \
    fi

RUN cmake .

RUN make

CMD ["./r-type_server"]
