# escape=`
FROM mcr.microsoft.com/windows/servercore:ltsc2019

WORKDIR C:\app

COPY srcs/Server/CMakeLists.txt C:\app\
COPY srcs/Server C:\app\
COPY includes\ C:\app\include\

RUN powershell -Command `
    Install-WindowsFeature -Name Web-Server; `
    Invoke-WebRequest -Uri https://github.com/microsoft/vcpkg/archive/master.zip -OutFile vcpkg.zip; `
    Expand-Archive -Path vcpkg.zip -DestinationPath .; `
    mv .\vcpkg-master .\vcpkg

RUN C:\app\vcpkg\bootstrap-vcpkg.bat

RUN C:\app\vcpkg\vcpkg.exe install asio

ARG TARGET_PLATFORM
ENV PATH_ASIO=""
RUN if ($env:TARGET_PLATFORM -eq "win32") { `
        $env:PATH_ASIO = "C:\app\vcpkg\installed\x64-windows\include"; `
    } elseif ($env:TARGET_PLATFORM -eq "unix") { `
        $env:PATH_ASIO = "C:\app\vcpkg\installed\x64-linux\include"; `
    } elseif ($env:TARGET_PLATFORM -eq "apple") { `
        $env:PATH_ASIO = "C:\app\vcpkg\installed\x64-osx\include"; `
    }

RUN cmake .

RUN msbuild.exe /p:Configuration=Release

CMD ["C:\app\r-type_server.exe"]
