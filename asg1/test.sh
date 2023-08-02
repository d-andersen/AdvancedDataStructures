#!/bin/bash

usage()
{
    echo "Skip list and scapegoat tree test script"
    echo
    echo "options:"
    echo "-h              Print this Help"
    echo "-c              Run make clean"
    echo "-p <float>      Value of p for Skip List. Defaults to 0.36788"
    echo "-a <float>      Value of alpha for Scapegoat Tree. Defaults to 0.55"
    echo "-n <integer>    Number of keys to insert. Defaults to 1024"
    echo "-k <integer>    Number of times to run search for n keys. Defaults to 1"
    exit 1;
}

p=0.36788
alpha=0.55
n=1024
k=1

while getopts "hp:a:n:k:c" option; do
   case ${option} in
        h)
            usage
            ;;
        p)  # value of p for Skip List
            p=${OPTARG}
            ;;
        a)  # value of alpha for Scapegoat Tree
            alpha=${OPTARG}
            ;;
        n)  # number of keys to insert
            n=${OPTARG}
            ;;
        k)  # number of times to run search for n keys
            k=${OPTARG}
            ;;
        c)  # number of times to run search for n keys
            make clean
            exit 0
            ;;
        *) # invalid option
            echo "Error: Invalid option"
            usage
            ;;
   esac
done

echo "p = ${p}"
echo "alpha = ${alpha}"
echo "n = ${n}"
echo "k = ${k}"

make

skip_list_output_files=""
scapegoat_tree_output_files=""
for (( i=0; i<${k}; i++ ))
do
    skip_list_output_files+="out_skip_list_${p}_${n}_${i} "
    scapegoat_tree_output_files+="out_scapegoat_tree_${alpha}_${n}_${i} "
done

python generate_input_files.py ${n} ${n} ${k}
echo "Generated ${k} input files with ${n} keys"

echo "Testing Skip List with p = ${p}. Results:"
for (( i=0; i<${k}; i++ ))
do
    ./skip_list $p < "test_${n}_${i}" > "out_skip_list_${p}_${n}_${i}"
done

python postprocess.py "${skip_list_output_files}" ${n} ${k}

echo -e "\nTesting Scapegoat Tree with alpha = ${alpha}. Results:"
for (( i=0; i<${k}; i++ ))
do
    ./scapegoat_tree $alpha < "test_${n}_${i}" > "out_scapegoat_tree_${alpha}_${n}_${i}"
done

python postprocess.py "${scapegoat_tree_output_files}" ${n} ${k}
