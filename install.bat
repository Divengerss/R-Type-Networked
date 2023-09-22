@echo off

cd vcpkg

call bootstrap-vcpkg.bat
call vcpkg integrate install
call vcpkg install sfml asio

cd ..
