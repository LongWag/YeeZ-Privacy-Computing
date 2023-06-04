xgboost: eXtreme Gradient Boosting 
=======
A General purpose gradient boosting (tree) library.

Authors:
* Tianqi Chen, project creater
* Kailong Chen, contributes regression module

Turorial and Documentation: https://github.com/tqchen/xgboost/wiki
 
Features
=======
* Sparse feature format:
  - Sparse feature format allows easy handling of missing values, and improve computation efficiency.
* Push the limit on single machine:
  - Efficient implementation that optimizes memory and computation.
* Layout of gradient boosting algorithm to support generic tasks, see project wiki.

Supported key components
=======
* Gradient boosting models: 
    - regression tree (GBRT)
    - linear model/lasso
* Objectives to support tasks: 
    - regression
    - classification
* OpenMP implementation

Planned components
=======
* More objective to support tasks: 
    - ranking
    - matrix factorization
    - structured prediction

File extension convention
=======
* .h are interface, utils and data structures, with detailed comment; 
* .cpp are implementations that will be compiled, with less comment; 
* .hpp are implementations that will be included by .cpp, with less comment
