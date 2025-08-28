#!/bin/bash
mkdir build
cd build
cmake ..
make
mv ./TradeMatchingEngine ../
cd -
