// Name: Read Rafi
// Project 3

#include "Leaderboard.hpp"
#include <algorithm>
#include <chrono>

/**
 * @brief Constructor for RankingResult with top players, cutoffs, and elapsed time.
 *
 * @param top Vector of top-ranked Player objects, in sorted order.
 * @param cutoffs Map of player count thresholds to minimum level cutoffs.
 *   NOTE: This is only ever non-empty for Online::rankIncoming().
 *         This parameter & the corresponding member should be empty
 *         for all Offline algorithms.
 * @param elapsed Time taken to calculate the ranking, in seconds.
 */
RankingResult::RankingResult(const std::vector<Player>& top, const std::unordered_map<size_t, size_t>& cutoffs, double elapsed)
    : top_ { top }
    , cutoffs_ { cutoffs }
    , elapsed_ { elapsed }
{
}

namespace {
    // Helper function to quicksort the right-side partitions 
    void quickSort(std::vector<Player>& arr, int low, int high) {
        if (low < high) {
            Player pivot = arr[high];
            int i = low - 1;
            for (int j = low; j < high; j++) {
                if (arr[j] < pivot) {
                    i++;
                    std::swap(arr[i], arr[j]);
                }
            }
            std::swap(arr[i + 1], arr[high]);
            int pi = i + 1;

            quickSort(arr, low, pi - 1);
            quickSort(arr, pi + 1, high);
        }
    }

    // Hybrid QuickSelect/QuickSort helper
    void quickSelectAndSort(std::vector<Player>& arr, int left, int right, int target_idx) {
        if (left <= right) {
            Player pivot = arr[right];
            int i = left - 1;
            for (int j = left; j < right; j++) {
                if (arr[j] < pivot) {
                    i++;
                    std::swap(arr[i], arr[j]);
                }
            }
            std::swap(arr[i + 1], arr[right]);
            int pi = i + 1;

            if (pi == target_idx) {
                // Pivot is exactly at the target boundary; sort the top 10% side
                quickSort(arr, pi + 1, right);
            } else if (pi < target_idx) {
                // Target is further right; continue selecting
                quickSelectAndSort(arr, pi + 1, right, target_idx);
            } else {
                // Pivot is inside the top 10%. Everything right of it is guaranteed top 10%.
                quickSort(arr, pi + 1, right);
                // Continue selecting left to find the rest of the top 10% boundary.
                quickSelectAndSort(arr, left, pi - 1, target_idx);
            }
        }
    }
}

RankingResult Offline::heapRank(std::vector<Player>& players) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t k = players.size() / 10;
    
    if (k == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        return RankingResult({}, {}, std::chrono::duration<double, std::milli>(end - start).count());
    }

    std::make_heap(players.begin(), players.end()); // Form initial max-heap

    // Early-stopping heapsort
    for (size_t i = 0; i < k; ++i) {
        std::pop_heap(players.begin(), players.end() - i); 
    }

    // The top k players are now sorted ascending at the end of the array
    std::vector<Player> top(players.end() - k, players.end());

    auto end = std::chrono::high_resolution_clock::now();
    return RankingResult(top, {}, std::chrono::duration<double, std::milli>(end - start).count());
}

RankingResult Offline::quickSelectRank(std::vector<Player>& players) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t k = players.size() / 10;

    if (k == 0) {
        auto end = std::chrono::high_resolution_clock::now();
        return RankingResult({}, {}, std::chrono::duration<double, std::milli>(end - start).count());
    }

    int target_idx = players.size() - k;
    quickSelectAndSort(players, 0, players.size() - 1, target_idx);

    std::vector<Player> top(players.begin() + target_idx, players.end());

    auto end = std::chrono::high_resolution_clock::now();
    return RankingResult(top, {}, std::chrono::duration<double, std::milli>(end - start).count());
}

void Online::replaceMin(PlayerIt first, PlayerIt last, Player& target) {
    if (first == last) return;
    
    *first = std::move(target); // Replace root with target
    size_t size = std::distance(first, last);
    size_t index = 0;

    // Percolate down the min-heap
    while (true) {
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;
        size_t smallest = index;

        if (left < size && *(first + left) < *(first + smallest)) smallest = left;
        if (right < size && *(first + right) < *(first + smallest)) smallest = right;

        if (smallest != index) {
            std::swap(*(first + index), *(first + smallest));
            index = smallest;
        } else {
            break;
        }
    }
}

RankingResult Online::rankIncoming(PlayerStream& stream, const size_t& reporting_interval) {
    std::vector<Player> heap;
    std::unordered_map<size_t, size_t> cutoffs;
    double elapsed_ms = 0.0;
    size_t count = 0;

    if (reporting_interval == 0) return RankingResult();
    heap.reserve(reporting_interval);

    while (stream.remaining() > 0) {
        Player p = stream.nextPlayer();
        count++;

        auto start = std::chrono::high_resolution_clock::now();

        if (heap.size() < reporting_interval) {
            heap.push_back(p);
            if (heap.size() == reporting_interval) {
                // Convert to min-heap once full
                std::make_heap(heap.begin(), heap.end(), std::greater<Player>()); 
            }
        } else {
            if (p > heap.front()) {
                replaceMin(heap.begin(), heap.end(), p);
            }
        }

        if (count % reporting_interval == 0) {
            cutoffs[count] = heap.front().level_; // Record cutoff
        }

        auto end = std::chrono::high_resolution_clock::now();
        elapsed_ms += std::chrono::duration<double, std::milli>(end - start).count();
    }

    auto start_final = std::chrono::high_resolution_clock::now();
    
    // Record final cutoff even if it is not a multiple of the interval
    if (count > 0 && count % reporting_interval != 0 && !heap.empty()) {
        if (heap.size() < reporting_interval) {
            auto min_it = std::min_element(heap.begin(), heap.end());
            cutoffs[count] = min_it->level_;
        } else {
            cutoffs[count] = heap.front().level_;
        }
    }

    std::sort(heap.begin(), heap.end()); // Sorted least to greatest
    auto end_final = std::chrono::high_resolution_clock::now();
    elapsed_ms += std::chrono::duration<double, std::milli>(end_final - start_final).count();

    return RankingResult(heap, cutoffs, elapsed_ms);
}