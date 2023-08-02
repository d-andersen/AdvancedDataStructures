from random import sample, randint, random
from sys import argv


def generate_input_file(n, runs, assoc_prob=0.3333):
    for i in range(runs):
        size = 0
        with open(f"pplist_test_{n}_{i}", 'w') as fout:
            num_keys_to_insert = sample([j for j in range(n)], k=n)
            for key in num_keys_to_insert:
                index = randint(0, size)
                s = f"I {key} {index}"
                r = random()
                if r < assoc_prob:
                    s += f" {key}"
                s += "\n"
                fout.write(s)
                size += 1
            fout.write("f\n")


if __name__ == '__main__':
    n = int(argv[1])
    runs = int(argv[2])
    if len(argv) == 4:
        assoc_prob = float(argv[3])
    generate_input_file(n, runs)
