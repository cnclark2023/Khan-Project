#include "human_player.h"

#include "exceptions.h"
#include "formatting.h"
#include "move.h"
#include "place_result.h"
#include "rang.h"
#include "tile_kind.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// This method is fully implemented.
inline string& to_upper(string& str) {
    transform(str.begin(), str.end(), str.begin(), ::toupper);
    return str;
}

Move HumanPlayer::get_move(const Board& board, const Dictionary& dictionary) const {
    // TODO: begin your implementation here.

    print_hand(cout);

    while (true) {

        cout << "Your move, " << get_name() << ":";
        string line;
        getline(cin, line);
        if (line != "") {
            Move m = parse_move(line);
            if (m.tiles.size()) {
                PlaceResult result = board.test_place(m);
                if (result.valid) {
                    bool valid = true;
                    for (auto word : result.words) {
                        if (!dictionary.is_word(word)) {
                            cout << "Error in move: invalid word is formed " << word << endl;
                            valid = false;
                        }
                    }
                    if (valid) {
                        return m;
                    }
                } else {
                    cout << "invalid move" << endl;
                }
            }
            if (m.kind == MoveKind::PASS || m.kind == MoveKind::EXCHANGE) {
                return m;
            }
        }
    }
}

vector<TileKind> HumanPlayer::parse_tiles(string& letters) const {
    // TODO: begin implementation here.
    vector<TileKind> tiles;
    try {
        for (size_t i = 0; i < letters.size(); i++) {
            tiles.push_back(this->tiles.lookup_tile(letters[i]));
        }
    } catch (exception& e) {
        cout << "Not enough tiles" << endl;
        tiles.clear();
    }
    return tiles;
}

Move HumanPlayer::parse_move(string& move_string) const {
    stringstream ss(move_string);
    string command;
    ss >> command;
    if (command == "PLACE") {
        int row, col;
        char direction;
        ss >> direction;  // >> direction;
        ss >> row;
        ss >> col;
        ss >> command;
        row--;
        col--;

        if (direction == '-') {
            vector<TileKind> tiles = parse_tiles(command);
            Move m(tiles, row, col, Direction::ACROSS);
            return m;
        } else if (direction == '|') {
            vector<TileKind> tiles = parse_tiles(command);
            Move m(tiles, row, col, Direction::DOWN);
            return m;
        } else {
            cout << "Invalid direction" << endl;
        }
    } else if (command == "EXCHANGE") {
        ss >> command;
        vector<TileKind> tiles = parse_tiles(command);
        Move m(tiles);
        return m;
    } else if (command == "PASS") {
        Move m(MoveKind::PASS);
        return m;
    } else {
        cout << "Invalid command";
    }

    Move m(MoveKind::ERROR);
    return m;
}

// This function is fully implemented.
void HumanPlayer::print_hand(ostream& out) const {
    const size_t tile_count = tiles.count_tiles();
    const size_t empty_tile_count = this->get_hand_size() - tile_count;
    const size_t empty_tile_width = empty_tile_count * (SQUARE_OUTER_WIDTH - 1);

    for (size_t i = 0; i < HAND_TOP_MARGIN - 2; ++i) {
        out << endl;
    }

    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_HEADING << "Your Hand: " << endl << endl;

    // Draw top line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;

    // Draw middle 3 lines
    for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
        out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD << repeat(SPACE, HAND_LEFT_MARGIN);
        for (auto it = tiles.cbegin(); it != tiles.cend(); ++it) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_PLAYER_HAND;

            // Print letter
            if (line == 1) {
                out << repeat(SPACE, 2) << FG_COLOR_LETTER << (char)toupper(it->letter) << repeat(SPACE, 2);

                // Print score in bottom right
            } else if (line == SQUARE_INNER_HEIGHT - 1) {
                out << FG_COLOR_SCORE << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << setw(2) << it->points;

            } else {
                out << repeat(SPACE, SQUARE_INNER_WIDTH);
            }
        }
        if (tiles.count_tiles() > 0) {
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
            out << repeat(SPACE, empty_tile_width) << BG_COLOR_OUTSIDE_BOARD << endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, HAND_LEFT_MARGIN) << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE;
    print_horizontal(tile_count, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << repeat(SPACE, empty_tile_width) << rang::style::reset << endl;
}
