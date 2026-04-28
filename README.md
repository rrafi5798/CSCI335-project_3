# Project 3: Ranking Up
**CSCI 33500 - Software Analysis & Design III**

## Overview
[cite_start]This project implements a leaderboard system designed to efficiently rank and retrieve the top 10% of players from various data sources[cite: 3]. [cite_start]It explores both offline algorithms (where all data is available upfront) and online algorithms (where data is processed as a continuous stream)[cite: 8, 26, 27].

## Build and Execution Steps
[cite_start]This project includes a `Makefile` for streamlined compilation[cite: 1]. 

1. **Compile the project:**
   Run the following command in the terminal to compile the source code into an executable:
   `make`

2. **Run the executable:**
   `./main`

3. **Clean build files:**
   To remove object files and the executable (useful before a fresh build):
   `make clean`

4. **Rebuild entirely:**
   `make rebuild`

## Algorithm Breakdown & Common Pitfalls

### 1. Offline Heapsort (`heapRank`)
* [cite_start]**Approach:** Transforms the input vector into a max-heap in-place and extracts the maximum element $k$ times (where $k$ is 10% of the total players)[cite: 9, 10, 11].
* **Common Mistakes:**
    * *Using extra memory:* Falling back on `std::priority_queue` or allocating a second vector to hold the heap. [cite_start]The algorithm must use `<algorithm>` operations (`std::make_heap`, `std::pop_heap`) to modify the vector in-place[cite: 5, 6, 12, 13].
    * [cite_start]*Sorting the whole array:* Heapsort should early-stop once the top 10% is found to optimize time complexity[cite: 11].
    * *Off-by-one errors:* Not rounding down properly when calculating the 10% cutoff. 

### 2. Offline Quickselect/Quicksort Hybrid (`quickSelectRank`)
* **Approach:** Uses Quickselect to partition the vector around the 10% boundary. [cite_start]Once the boundary is found, it uses Quicksort exclusively on the top 10% partition to sort them in ascending order[cite: 16, 18, 19].
* **Common Mistakes:**
    * *Poor Pivot Selection:* Choosing the first or last element as a pivot on already-sorted data can degrade the time complexity to $O(N^2)$.
    * *Over-sorting:* Recursively calling Quicksort on the bottom 90% of the data, which wastes processing time.
    * *Boundary logic:* Mishandling the target index, causing the top 10% partition to include elements it shouldn't.

### 3. Online Stream Processing (`rankIncoming`)
* [cite_start]**Approach:** Processes a stream of players one by one, maintaining a min-heap of size `reporting_interval`[cite: 51, 52]. [cite_start]If a new player is larger than the root of the min-heap, `replaceMin` swaps them and percolates down[cite: 43, 53].
* **Common Mistakes:**
    * *Timing I/O operations:* Including the time it takes to fetch `stream.nextPlayer()` in the `elapsed_` metric. [cite_start]The timer should only track the internal sorting and heap adjustments[cite: 20].
    * [cite_start]*Queueing instead of vectoring:* Using `std::priority_queue` instead of a vector with iterator-based heap operations[cite: 56, 57].
    * [cite_start]*Missed cutoffs:* Forgetting to record the final cutoff level when the stream runs out of players, especially if the total count isn't a perfect multiple of the `reporting_interval`[cite: 17, 18].