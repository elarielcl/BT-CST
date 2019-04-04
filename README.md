# BT-CST
# Description
This repository contains the implementation of the algorithms and structures described at [https://arxiv.org/abs/1902.03274](https://arxiv.org/abs/1902.03274). In addition to the paper's proposal, this code contains some new approaches to appear in the thesis of Manuel Cáceres.
# Installation Guide
First clone the repo with its submodule:
```
 git clone --recurse-submodules https://github.com/elarielcl/BT-CST.git
 ```
 This project uses the [sdsl](https://github.com/elarielcl/sdsl-lite) library, so previous to compile it you should install the [sdsl](https://github.com/elarielcl/sdsl-lite). To install it:
 ```
 cd BlockTrees/external/sdsl-lite
 ./install.sh .
 ```
 
This project is a CMake project. To build this project with some runnables you should do

```
cd ../..
mkdir build
cd build
cmake ..
make
```

Finally when you want to create a new executable, you need to add it to the experiments/CMakeLists.txt as part of the project_EXECUTABLES environment variable, where the experiments are located.
# Contact
Any error, improvement or suggestion you can write me to `elarielcl` in Gmail. 
