# Project 3: Ranking Up
**CSCI 33500 - Software Analysis & Design III**

## I. Completed Parts of the Assignment
I completed all parts of the project, implemented Offline and Online algorithms. 
  * `heapRank`: Implemented an in-place, early-stopping heapsort to find the top 10% of players.
  * `quickSelectRank`: Implemented an in-place Quickselect/Quicksort hybrid to partition and sort the top 10% of players.
  * `VectorPlayerStream`: Fully implemented the `PlayerStream` interface to sequentially yield players.
  * `replaceMin`: Implemented an iterator-based heap replacement and percolate-down helper function.
  * `rankIncoming`: Implemented the continuous stream processor that tracks the top players using a fixed-size min-heap and records periodic cutoffs.

## II. Bugs Encountered
No bugs were encountered during final testing.

## III. Instructions to Run the Program
Use makefile
1. **Compile the project:**
make
./main