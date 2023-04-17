# How to Setup Eigen Library

There are three ways to set up `Eigen`:

1. install from package managers (for Mac and Ubuntu)
2. download library release (for all OSs)
3. use github submodule to download source code (for all OSs)

***We use the option 3 in this course***. 

Please clone the `Eigen` repository into your repository with the command

```bash
$ cd pba-<usename>
$ git submodule update --init external/eigen # clone the "eigen" repository
```

Make sure you have a header file `Dense` at

```
pba-<username>/external/eigen/Eigen/Dense
```

That's it. Eigen is a ***header-only library*** you do not need to install it.
