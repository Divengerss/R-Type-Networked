@echo off

cd \vcpkg

bootstrap-vcpkg.bat
vcpkg install sfml

cd ..
