#!/bin/bash

Help()
{
    echo "Randomized binary tree test script."
    echo
    echo "Syntax: test.sh [-h|n|k]"
    echo "options:"
    echo "h     Print this Help."
    echo "n     Number of keys to insert. Defaults to 1024."
    echo "k     Number of times to run search for n keys. Defaults to 1."
    echo
}
n=1024
k=1
p=0.3333

while getopts ":hn:k:p:" option; do
   case ${option} in
        h)
            Help
            exit;;
        n)  # number of keys to insert
            n=$OPTARG;;
        k)  # number of times to run search for n keys
            k=$OPTARG;;
        p)  # probability of generating an associated node
            p=$OPTARG;;
        \?) # invalid option
            echo "Error: Invalid option"
            Help
            exit;;
   esac
done

make -j 16

rtree_output_files=""
for (( i=0; i<k; i++ ))
do
    rtree_output_files+="out_rtree_${n}_${i} "
done

python rtree_generate_input_files.py $n $k
echo "Generated ${k} input files with ${n} keys for rtree"

echo "Testing randomized search tree."
for (( i=0; i<k; i++ ))
do
   ./rtree_test < "rtree_test_${n}_${i}" > "out_rtree_${n}_${i}"
done

python rtree_postprocess.py "${rtree_output_files}" "$n" "$k"


pplist_output_files=""
for (( i=0; i<k; i++ ))
do
    pplist_output_files+="out_pplist_${n}_${i} "
done

python pplist_generate_input_files.py $n $k $p
echo "Generated ${k} input files with ${n} keys for pplist"

echo "Testing partially persistent list."
for (( i=0; i<k; i++ ))
do
   ./pplist_test < "pplist_test_${n}_${i}" > "out_pplist_${n}_${i}"
done

python pplist_postprocess.py "${pplist_output_files}" "$n" "$k"
