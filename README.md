<h1 align="center">
  <img src=".logo.png"/><br/>
  <div style="border-bottom: #ffffff 4px solid"></div><br/>
  R-Type Networked
</h1>

# Overview

R-Type is a horizontally scrolling shooter arcade video game developed and released by Irem in 1987 and the first game in the R-Type series. The player controls a star ship, the R-9 "Arrowhead", in its efforts to destroy the Bydo, a powerful alien race bent on wiping out all of mankind.

Source: [Wikipedia](https://en.wikipedia.org/wiki/R-Type)


## Installation

Installation process uses [vcpkg](https://github.com/microsoft/vcpkg/tree/master) to install its depedencies.

We provided Installation scripts to help you install everything.

Simply run `install.sh` on Linux or `install.bat` on Windows.

To compile the programs `r-type_client` and `r-type_server`:
```bash
  $> cmake -S . -B Build/
  $> make -C Build/ -j
```
*Note: Make sure you are under the root directory before running the compilation commands.*
    
## Tech Stack

Both programs use **C++**

**Client:** [SFML](https://www.sfml-dev.org/documentation/2.6.0/index.php)

**Server:** [Asio](https://think-async.com/Asio/asio-1.28.0/doc/asio/overview/basics.html)

*Note: Has it is required to communicate with the server, the client also uses Asio.*


## Documentation

Documentation can be found under the [docs](https://github.com/EpitechPromo2026/B-CPP-500-PAR-5-1-rtype-julian.emery/tree/main/docs) directory.

- [Server binary protocol](https://github.com/EpitechPromo2026/B-CPP-500-PAR-5-1-rtype-julian.emery/blob/main/docs/SERVER.md)
- [Graphical implementation](https://github.com/EpitechPromo2026/B-CPP-500-PAR-5-1-rtype-julian.emery/blob/main/docs/CLIENT.md)


## License

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://github.com/EpitechPromo2026/B-CPP-500-PAR-5-1-rtype-julian.emery/blob/main/LICENSE)


## Authors

- [@Divengerss](https://github.com/Divengerss)
- [@M4KS0U](https://github.com/M4KS0U)
- [@JulianEmr](https://github.com/JulianEmr)
- [@JulesLevi](https://github.com/JulesLevi)
