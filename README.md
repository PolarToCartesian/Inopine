# Inopiné Engine

<p align=justify>
  
***Inopiné*** is a cross-platform, and header-only graphics engine built in c++20 that strives to make the development of interactive graphical applications simpler by serving as a wrapper of low-level apis. It doesn't include any third-party library since the creator likes to reinvent the wheel (although I don't recommend this approach). C++ has been the standard for many years in fields where high performance and low-latency are critical. It grants the programmer direct and verbose low-level access to hardware components like memory and I/O while lacking most heavy-weight and high level abstractions of modern languages that compromise performance for ease-of-use (features like automatic garbage collection, weakly-typed types, and interpretation that we can't afford), putting the burden of efficiency on him.  Although, C++ used to be very similar to C, it has grown into a much more complex language that offers "zero cost" abstractions of low-level concepts and quality of life features like namespaces and templates (the latter enables meta-programming and prevents code duplication).

</p>

## Features

You can view the table of contents in Inopine's [header file](Include/Inopine/Inopine.hpp).

## Getting Started

**Prerequisites:**

+ ***Linux Packages (Ubuntu/Debian)***: `sudo apt-get update && sudo apt-get install git cmake make libx11-dev g++-10 valgrind`. Note that you can swap `g++-10` for your favorite compiler as long as it supports C++20/C++2a.
+ ***Windows Downloads***: [***Git***](https://git-scm.com/downloads), [***CMake***](https://cmake.org/download/), [***Visual Studio***](https://visualstudio.microsoft.com/).

<br>

**Building & Running:**

+ Run `git clone https://github.com/PolarToCartesian/Inopine` in the directory in which you want the repository to be located.
+ Then run, `cd Inopine/ && mkdir build && cmake ../` to generate the build files.
+ On ***Linux***,  run `make && ./Inopine` to build & run the sample application
+ On ***Windows***, open Inopine.sln in visual studio.

## Technical Help/Specifications

+ The [***Intel Intrinsics Guide***](https://software.intel.com/sites/landingpage/IntrinsicsGuide/).

## Contributions

Many Thanks To:

+ [***Red-Rapious***](https://github.com/Red-Rapious) for the amazing logo!

## License

This project is licensed under [***this license***](LICENSE.md).
