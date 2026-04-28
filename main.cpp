#include "Leaderboard.hpp"
#include "Player.hpp"
#include "PlayerStream.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cassert>

// Helper function to generate a vector of random players
std::vector<Player> generatePlayers(size_t count, int seed = 42) {
    std::vector<Player> players;
    players.reserve(count);
    std::mt19937 gen(seed); // Seeded for reproducibility
    std::uniform_int_distribution<size_t> level_dist(1, 10000);

    for (size_t i = 0; i < count; ++i) {
        players.emplace_back("Player_" + std::to_string(i), level_dist(gen));
    }
    return players;
}

// Helper function to verify if a result correctly contains the top k elements in ascending order
bool verifyTopK(const std::vector<Player>& original, const std::vector<Player>& result, size_t k) {
    if (result.size() != k) return false;
    
    std::vector<Player> sorted_original = original;
    std::sort(sorted_original.begin(), sorted_original.end(), std::greater<Player>());
    
    // The result should be the top k elements, but sorted in ascending order
    for (size_t i = 0; i < k; ++i) {
        if (result[k - 1 - i].level_ != sorted_original[i].level_) {
            return false;
        }
    }
    return true;
}

void testOfflineHeapRank() {
    std::cout << "--- Testing Offline::heapRank ---" << std::endl;
    auto players = generatePlayers(100);
    auto original = players; // Keep copy for verification
    
    RankingResult res = Offline::heapRank(players);
    size_t expected_k = 100 / 10;
    
    if (verifyTopK(original, res.top_, expected_k)) {
        std::cout << "[PASS] heapRank correctly found and sorted the top 10% (" << expected_k << " players)." << std::endl;
    } else {
        std::cout << "[FAIL] heapRank failed verification." << std::endl;
    }
    std::cout << "Elapsed time: " << res.elapsed_ << " ms\n" << std::endl;
}

void testOfflineQuickSelectRank() {
    std::cout << "--- Testing Offline::quickSelectRank ---" << std::endl;
    auto players = generatePlayers(100);
    auto original = players; 
    
    RankingResult res = Offline::quickSelectRank(players);
    size_t expected_k = 100 / 10;
    
    if (verifyTopK(original, res.top_, expected_k)) {
        std::cout << "[PASS] quickSelectRank correctly found and sorted the top 10% (" << expected_k << " players)." << std::endl;
    } else {
        std::cout << "[FAIL] quickSelectRank failed verification." << std::endl;
    }
    std::cout << "Elapsed time: " << res.elapsed_ << " ms\n" << std::endl;
}

void testEdgeCasesOffline() {
    std::cout << "--- Testing Offline Edge Cases (< 10 players) ---" << std::endl;
    auto players = generatePlayers(9); // 10% of 9 is 0
    
    RankingResult resHeap = Offline::heapRank(players);
    if (resHeap.top_.empty()) {
        std::cout << "[PASS] heapRank handles 0-sized top 10% correctly." << std::endl;
    } else {
        std::cout << "[FAIL] heapRank returned elements when it should have returned 0." << std::endl;
    }
    std::cout << std::endl;
}

void testOnlineRankIncoming() {
    std::cout << "--- Testing Online::rankIncoming ---" << std::endl;
    size_t total_players = 132;
    size_t interval = 50;
    
    auto players = generatePlayers(total_players);
    VectorPlayerStream stream(players);
    
    RankingResult res = Online::rankIncoming(stream, interval);
    
    // Verify sizes
    if (res.top_.size() == interval) {
        std::cout << "[PASS] rankIncoming returned exactly " << interval << " players." << std::endl;
    } else {
        std::cout << "[FAIL] rankIncoming returned " << res.top_.size() << " players, expected " << interval << "." << std::endl;
    }
    
    // Verify cutoffs exist
    bool has_50 = res.cutoffs_.count(50);
    bool has_100 = res.cutoffs_.count(100);
    bool has_132 = res.cutoffs_.count(132);
    
    if (has_50 && has_100 && has_132) {
        std::cout << "[PASS] rankIncoming recorded cutoffs for 50, 100, and 132." << std::endl;
        std::cout << "       Cutoff at 50:  Level " << res.cutoffs_[50] << "\n"
                  << "       Cutoff at 100: Level " << res.cutoffs_[100] << "\n"
                  << "       Cutoff at 132: Level " << res.cutoffs_[132] << std::endl;
    } else {
        std::cout << "[FAIL] rankIncoming missed one or more cutoffs." << std::endl;
    }
    
    std::cout << "Elapsed time: " << res.elapsed_ << " ms\n" << std::endl;
}

int main() {
    std::cout << "====================================\n";
    std::cout << "  Starting Leaderboard Test Suite   \n";
    std::cout << "====================================\n\n";

    testOfflineHeapRank();
    testOfflineQuickSelectRank();
    testEdgeCasesOffline();
    testOnlineRankIncoming();

    std::cout << "====================================\n";
    std::cout << "         Testing Complete           \n";
    std::cout << "====================================\n";

    return 0;
}