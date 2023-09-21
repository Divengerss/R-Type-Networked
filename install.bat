@echo off

cd \vcpkg

bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install sfml

cd ..
