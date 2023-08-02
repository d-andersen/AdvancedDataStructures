import re

from collections import defaultdict
from math import ceil
from sys import argv


def postprocess(fnames, searches, runs):
    total_complexity = 0
    histogram = defaultdict(int)
    for fname in fnames:
        with open(fname, 'r') as fin:
            search_depth = 0
            lines = fin.readlines()
            for line in lines:
                match = re.search("found '[0-9]*'. Search depth: [0-9]*", line)
                if match:
                    c = int(match.group().split(" ")[-1])
                    histogram[c] += 1
                    search_depth += c
        total_complexity += search_depth

    print(f"average complexity: {ceil((total_complexity / searches) / runs):>3}")
    histogram_list = list(histogram.items())
    histogram_list.sort(key=lambda x: x[0])
    accumulated_percentage = []
    second = 0
    for pair in histogram_list:
        first = pair[0]
        second += pair[1]
        accumulated_percentage.append((first, round(((second / searches) / runs) * 100, 2)))
        print(f"({first}, {round(((second / searches) / runs) * 100, 2)})", end='')
    print()


if __name__ == '__main__':
    fnames = argv[1].strip().split(" ")
    searches = int(argv[2])
    runs = int(argv[3])
    postprocess(fnames, searches, runs)
