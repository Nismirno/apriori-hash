# apriori-hash
Apriori algorithm using hash tree.

Require: cmake >= 3.10.2, boost >= 1.36

First clone and build
```
git clone git@github.com:Nismirno/apriori-hash.git
or 
git clone https://github.com/Nismirno/apriori-hash.git
cd apriori-hash
mkdir build
cd build
cmake ..
make
```

By default algorithm is running on test.data

To run 
```
Example:
./apriori --filename path-to-file --support 25 --nRules 20 --order asc
```
Support is required to be in percents

Rules are ordered by support of XY set

Available options of ordering: `asc` and `desc`
