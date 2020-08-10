<span>This repository contains all the variants of BlockTrees described in the experimental study available [here](https://users.dcc.uchile.cl/~gnavarro/mem/algoritmos/tesisManuel.pdf). If you are looking for the cannonical versions used in the journal article, which is also more reusable and uses less contruction space, you can go to [https://github.com/elarielcl/MinimalistBlockTrees/](https://github.com/elarielcl/MinimalistBT-CST/)</span>

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
 cd BT-CST/external/sdsl-lite
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


# Usage Guide
 On the experiments folder you can find some examples of executables using the implemented Structures.
 ## Example Block Tree Compressed Suffix Tree
 Let's suppose we want to build our Compressed Suffix Tree using the Block Tree Compressed Topology, BT-CST so we do:
 ```
 ...
 int r = 2; //The arity of the BlockTree
 int mll = 128; // The max length that a BlockTree's leaf could represent
 
 PBTRLCSACST * cst = new PBTRLCSACST(input_string, PBTRLCSACST.PAPER, r, mll, c);
 ...
 cst->suffix_link((cst->lca(pbtrlcsacst->select_leaf(l1), pbtrlcsacst->select_leaf(l2))) 
 ...
 ```
 in case you want to build the BT-CST-LCSA-NONE version instead you can replace the the construction line by
 ```
PBTLCSACST * cst = new PBTLCSACST(input_string, PBTLCSACST.PAPER, r, mll, c);
 ```
 # Contact
 Any error, improvement or suggestion you can write me to `elarielcl` in Gmail. 
