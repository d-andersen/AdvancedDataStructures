import re

from collections import defaultdict
from itertools import islice
from math import ceil
from sys import argv


def postprocess(fnames, elements, runs):
    keys = ["pnodes", "anodes", "eps", "pplist_mem", "pnodes_mem", "anodes_mem", "eps_mem", "total_mem"]
    d = dict.fromkeys(keys, 0)

    for fname in fnames:
        with open(fname, 'r') as fin:
            lines = list(islice(fin, elements, None))

            d['pnodes'] += int(lines[2].split(' ')[-1])
            d['anodes'] += int(lines[3].split(' ')[-1])
            d['eps'] += int(lines[5].split(' ')[-1])
            d['pplist_mem'] += int(lines[8].split(' ')[-2])
            d['pnodes_mem'] += int(lines[9].split(' ')[-2])
            d['anodes_mem'] += int(lines[10].split(' ')[-2])
            d['eps_mem'] += int(lines[11].split(' ')[-2])
            d['total_mem'] += int(lines[12].split(' ')[-2])

    for k, v in d.items():
        d[k] = ceil(v / runs)

    total_nodes = d['pnodes'] + d['anodes']
    print(f"({d['eps']}, {total_nodes}, {d['total_mem']}) {((total_nodes / elements) - 1) * 100:.2f}")


if __name__ == '__main__':
    fnames = argv[1].strip().split(" ")
    elements = int(argv[2])
    runs = int(argv[3])
    postprocess(fnames, elements, runs)
