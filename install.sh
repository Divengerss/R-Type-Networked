#!/bin/bash

./bootstrap-vcpkg.sh
./vcpkg/vcpkg integrate install
./vcpkg/vcpkg install sfml asio
