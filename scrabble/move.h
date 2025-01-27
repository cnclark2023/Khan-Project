#ifndef MOVE_H
#define MOVE_H

#include "tile_kind.h"
#include <stdlib.h>
#include <vector>

enum class Direction {
    ACROSS,
    DOWN,
    NONE,
};

Direction operator!(Direction direction);

enum class MoveKind { PLACE, PASS, EXCHANGE, ERROR };

struct Move {
    MoveKind kind;
    std::vector<TileKind> tiles;
    size_t row;
    size_t column;
    Direction direction;

    Move() : kind(MoveKind::PASS) {}
    Move(MoveKind kind) : kind(kind) {}

    Move(std::vector<TileKind> tiles) : kind(MoveKind::EXCHANGE), tiles(tiles) {}
    Move(std::vector<TileKind> tiles, size_t row, size_t column, Direction direction)
            : kind(MoveKind::PLACE), tiles(tiles), row(row), column(column), direction(direction) {}
};

#endif
