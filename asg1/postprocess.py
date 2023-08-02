import re

from collections import defaultdict
from math import ceil
from sys import argv


def postprocess(fnames, searches, iterations):
    total_comparisons = 0
    histogram = defaultdict(int)
    for fname in fnames:
        with open(fname, 'r') as fin:
            comparisons = 0
            lines = fin.readlines()
            for line in lines:
                match = re.search("found '[0-9]*'. Comparisons: [0-9]*", line)
                if match:
                    c = int(match.group().split(" ")[-1])
                    histogram[c] += 1
                    comparisons += c
            print(f"{fname}: comparisons / searches = {comparisons} / {searches} = {ceil(comparisons / searches):>3}")
        total_comparisons += comparisons
        
    print(f"total number of comparisons  : {total_comparisons}")
    print(f"average number of comparisons: {ceil((total_comparisons / searches) / iterations):>3}")
    histogram_list = list(histogram.items())
    histogram_list.sort(key=lambda x: x[0])
    accumulated_percentage = []
    second = 0
    for pair in histogram_list:
        first = pair[0]
        second += pair[1]
        accumulated_percentage.append((first, round(((second / searches) / iterations) * 100, 2)))
        print(f"({first}, {round(((second / searches) / iterations) * 100, 2)})", end='')
    print()


if __name__ == '__main__':
    fnames = argv[1].strip().split(" ")
    searches = int(argv[2])
    iterations = int(argv[3])
    postprocess(fnames, searches, iterations)
