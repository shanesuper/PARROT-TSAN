#!/bin/bash
n=10
models=( "origin" "llvm" "llvm-parrot" )
for model in "${models[@]}"
do
./histogram.sh $model $n
./linear.sh $model $n
./pca.sh $model $n
./string.sh $model $n
./word_count.sh $model $n
./matrix.sh $model $n
done
