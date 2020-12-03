#include "player.h"

#include <exception>
#include <string>

using namespace std;

size_t Player::get_points() const { return points; }

size_t Player::get_hand_size() const { return hand_size; }

const string& Player::get_name() const { return name; }

void Player::add_tiles(std::vector<TileKind, std::allocator<TileKind>> const& tiles) {
    for (auto tile : tiles) {
        this->tiles.add_tile(tile);
    }
}

size_t Player::count_tiles() const { return tiles.count_tiles(); }

// TODO : to be implemented

bool Player::has_tile(TileKind t) {
    try {
        tiles.lookup_tile(t.letter);
        return true;
    } catch (exception& e) {
        return false;
    }
}

void Player::remove_tiles(std::vector<TileKind, std::allocator<TileKind>> const& tiles) {
    for (auto tile : tiles) {
        this->tiles.remove_tile(tile);
    }
}

void Player::add_points(size_t points) { this->points += points; }

void Player::subtract_points(size_t points) { this->points -= points; }

unsigned int Player::get_hand_value() const { return tiles.total_points(); }
