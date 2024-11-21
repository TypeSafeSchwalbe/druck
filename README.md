# `druck`
A 3D software renderering library written in C++, for C++.

# Dependencies
This library depends on `raylib` *for creating a window and writing the render result to it (99% of usages in `src/engine.cpp`).*

The steps for installing `raylib` may differ across operating systems.
On Fedora Linux, `sudo dnf install raylib` did the job.

# Building
To build a project that makes use of `dampf`, include all files in the `src`-directory as source files to be compiled, and include `include` as an include path. 

# Examples
Build and run the example program in the `example` directory to see a few examples made with `dampf`.