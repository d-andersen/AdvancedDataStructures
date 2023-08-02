## DM803 Advanced Data Structures Spring 2022 - Exam Project Part 2

Implementation in C++ of the [randomized binary search tree](https://en.wikipedia.org/wiki/Treap#Randomized_binary_search_tree) and [partially persistent list](https://en.wikipedia.org/wiki/Persistent_data_structure) data structures.

The report for this assignment is in the `doc` folder.

#### How to build and run

A makefile is included and the default target builds the programs for both the randomized search tree and the partially persistent list as `rtree_test` and `pplist_test` respectively. There are python scripts for each data structure to generate input test files, with each script generating two uniformly random samples of the integers $0, \ldots, n - 1$, the first of which is used for $n$ insert operations followed by $n$ search operations using the second sample.

Also included is a small shell script `test.sh`, taking the following options:

```
-h              Print this help
-n <integer>    Number of keys to insert. Defaults to 1024
-k <integer>    Number of times to run search for n keys. Defaults to 1
```

The `test.sh` script runs the `rtree_test` and `pplist_test` programs on each of the $k$ generated input files followed by a post processing step, which outputs the results of the test to `stdout` unless redirected.
