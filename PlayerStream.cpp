// Name: Read Rafi
// Project 3

#include "PlayerStream.hpp"

VectorPlayerStream::VectorPlayerStream(const std::vector<Player>& players)
    : players_(players), current_idx_(0) {}

Player VectorPlayerStream::nextPlayer() {
    if (remaining() == 0) {
        throw std::runtime_error("No more players in stream."); 
    }
    return players_[current_idx_++];
}

size_t VectorPlayerStream::remaining() const {
    return players_.size() - current_idx_; 
}