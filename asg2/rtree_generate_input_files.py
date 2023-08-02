from random import sample
from sys import argv


def generate_input_file(n, runs):
    
    for i in range(runs):
        with open(f"rtree_test_{n}_{i}", 'w') as fout:
            num_keys_to_insert = sample([j for j in range(n)], k=n)
            for key in num_keys_to_insert:
                fout.write(f"I {key}\n")
            num_keys_to_search = sample([j for j in range(n)], k=n)
            for key in num_keys_to_search:
                fout.write(f"S {key}\n")


if __name__ == '__main__':
    n = int(argv[1])
    runs = int(argv[2])
    generate_input_file(n, runs)
