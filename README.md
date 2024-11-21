# `druck`
A 3D software renderering library written in C++, for C++.

# Dependencies
This library depends on `raylib` version 4.21 *for loading images, creating a window and displaying the final render result (most of the usages being in [`src/window.cpp`](./src/window.cpp)).*
It also depends on [`nlohmann/json`](https://github.com/nlohmann/json).

The steps for installing `raylib` may differ across operating systems.
On Fedora Linux, `sudo dnf install raylib` did the job.

# Building
To build a project that makes use of `druck`, include all files in the `src`-directory as source files to be compiled, and include `include` as an include path. 

# Examples
Build and run [the example program](./example/) in the `example` directory to see a few examples made with `druck`.