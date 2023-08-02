from random import sample
from sys import argv


def generate_input_file(n, searches, iterations):
    
    for i in range(iterations):
        with open(f"test_{n}_{i}", 'w') as fout:
            num_keys_to_insert = sample([j for j in range(n)], k=n)
            for key in num_keys_to_insert:
                fout.write(f"I {key}\n")
            for k in range(searches // n):
                num_keys_to_search = sample([j for j in range(n)], k=n)
                for key in num_keys_to_search:
                    fout.write(f"S {key}\n")


if __name__ == '__main__':
    n = int(argv[1])
    searches = int(argv[2])
    iterations = int(argv[3])
    generate_input_file(n, searches, iterations)
