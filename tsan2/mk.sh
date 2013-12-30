#!/bin/bash
rm -rf $1
svn co http://llvm.org/svn/llvm-project/llvm/trunk $1
cd $1
R=196352
(cd tools && svn co -r $R http://llvm.org/svn/llvm-project/cfe/trunk clang)
(cd projects && svn co -r $R http://llvm.org/svn/llvm-project/compiler-rt/trunk compiler-rt)
mkdir build
PWD=`pwd`
cd build
CC=gcc-4.5.4 CXX=g++-4.5.4 ../configure --prefix=$PWD/install --enable-assertions --enable-debug-runtime --enable-optimized
