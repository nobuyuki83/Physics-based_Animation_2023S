# How to Set Up GLFW Library

GLFW is a library to open a window with OpenGL graphics visualization. 

There are three ways to set up GLFW:  
1. using package manager (for Mac and Ubuntu)
2. download pre-build library (Mac and Windows)
3. build the source code (for all OSs)

***This class support the option 1 and 3.*** Below, we discuss these options in detail for each OS

----

## Mac 

### Install from Package Manager

for Mac, install `glfw` using package manager `brew` as

```bash
$ brew install glfw
```

### Install from Source Code

Alternatively, you can build `glfw` from source code and put the library under `external/glfwlib` with

```bash
$ git submodule update --init external/glfw
$ cd external/glfw
$ cmake .
$ cmake --build . --config Release
$ cmake --install . --prefix ../glfwlib
```

Make sure you have a header file `glfw3.h` at

```
pba-<username>/external/glfwlib/include/GLFW/glfw3.h
```

----
## Ubuntu

### Install from Package Manager

For ubuntu, install `glfw` using `apt-get` as

```bash
$ sudo apt-get install -y libx11-dev xorg-dev \
                          libglu1-mesa libglu1-mesa-dev \
                          libgl1-mesa-glx libgl1-mesa-dev
$ sudo apt install -y libglfw3 libglfw3-dev
$ sudo apt install -y libglew-dev
```

### Build from Source Code

Alternatively, you can build `glfw` from source code and put the library under `external/glfwlib` with

```bash
$ git submodule update --init external/glfw
$ cd external/glfw
$ cmake .
$ cmake --build . --config Release
$ cmake --install . --prefix ../glfwlib
```

Make sure you have a header file `glfw3.h` at

```
pba-<username>/external/glfwlib/include/GLFW/glfw3.h
```

----

## Windows

### Build from Source Code

Alternatively, you can build `glfw` from source code and put the library under `external/glfwlib` with

```bash
$ git submodule update --init external/glfw
$ cd external/glfw
$ cmake .
$ cmake --build . --config Release
$ cmake --install . --prefix ../glfwlib
```

Make sure you have a header file `glfw3.h` at

```
pba-<username>/external/glfwlib/include/GLFW/glfw3.h
```