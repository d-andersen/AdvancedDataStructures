## DM803 Advanced Data Structures Spring 2022 - Exam Project Part 1

Implementation in C++ of the [skip list](https://en.wikipedia.org/wiki/Skip_list) and [scapegoat tree](https://en.wikipedia.org/wiki/Scapegoat_tree) data structures.

The report for this assignment is in the `doc` folder.

#### How to build and run

A makefile is included and the default target builds the programs for both the skip list and the scapegoat tree as `skip_list` and `scapegoat_tree` respectively.

Running

`make test`

will build both programs and run each in turn using the example input from the project description as a quick test that everything compiled and works as expected.

In addition, there are python scripts for each data structure to generate input test files, with each script generating two uniformly random samples of the integers $0, \ldots, n - 1$, the first of which is used for $n$ insert operations followed by $n$ search operations using the second sample.

Also included is a small shell script `test.sh`, taking the following options:

```
-h              Print this help
-p <float>      Value of p for Skip List. Defaults to 0.36788
-a <float>      Value of alpha for Scapegoat Tree. Defaults to 0.55
-n <integer>    Number of keys to insert. Defaults to 1024
-k <integer>    Number of times to run search for $n$ keys. Defaults to 1
```

This script generates $k$ test files, and for each file, two uniformly random samples of the integers $0, ..., n - 1$ are generated, the first of which is used for $n$ insert operations followed by $n$ search operations using the second sample. The script then runs the `skip_list` and `scapegoat_tree` programs on each of the corresponding $k$ input files, followed by a post processing step using `postprocess.py`, which outputs the results of the test to `stdout`.
