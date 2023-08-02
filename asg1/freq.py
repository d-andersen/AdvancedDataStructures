import os
import re

from collections import defaultdict
from math import ceil

A = [0.55, 0.6, 0.65, 0.7, 0.75]

N = [2**10, 2**15]

K = [0, 1, 2]

source = "./out/"

for a in A:
    for n in N:
        histogram = defaultdict(int)
        for k in K:
            
            with open(os.path.join(source, f"out_scapegoat_tree_{a}_{n}_{k}.txt"), 'r') as fin:
                lines = fin.readlines()
                for line in lines:
                    match = re.search("subtree size: [0-9]*", line)
                    if match:
                        c = int(match.group().split(" ")[-1])
                        histogram[c] += 1

        histogram_list = list(histogram.items())
        histogram_list.sort(key=lambda x: x[0])
        print(a, n)
        for pair in histogram_list:
            print(f"({pair[0]}, {ceil(pair[1] / len(K))})", end="")
        print()
