#include "board.h"

#include "board_square.h"
#include "exceptions.h"
#include "formatting.h"
#include <fstream>
#include <iomanip>

using namespace std;

bool Board::Position::operator==(const Board::Position& other) const {
    return this->row == other.row && this->column == other.column;
}

bool Board::Position::operator!=(const Board::Position& other) const {
    return this->row != other.row || this->column != other.column;
}

Board::Position Board::Position::translate(Direction direction) const { return this->translate(direction, 1); }

Board::Position Board::Position::translate(Direction direction, ssize_t distance) const {
    if (direction == Direction::DOWN) {
        return Board::Position(this->row + distance, this->column);
    } else {
        return Board::Position(this->row, this->column + distance);
    }
}

Board Board::read(const string& file_path) {

    ifstream file(file_path);
    if (!file) {
        throw FileException("cannot open board file!");
    }

    size_t rows;
    size_t columns;
    size_t starting_row;
    size_t starting_column;
    file >> rows >> columns >> starting_row >> starting_column;
    Board board(rows, columns, starting_row, starting_column);
    std::string line;

    // TODO: complete implementation of reading in board from file here.
    std::vector<BoardSquare> rank;
    while ((file >> line)) {
        for (char c : line) {
            int lm = 1, wm = 1;
            if (c == '.') {
            } else if (c == '2') {
                lm = 2;
            } else if (c == '3') {
                lm = 3;
            } else if (c == 'd') {
                wm = 2;
            } else if (c == 't') {
                wm = 3;
            }
            if (c != '\r' && c != '\n') {
                BoardSquare square(lm, wm);
                rank.push_back(square);
            }
        }
        board.squares.push_back(rank);
        rank.clear();
    }
    return board;
}

size_t Board::get_move_index() const { return this->move_index; }

PlaceResult Board::test_place(const Move& move) const {
    // TODO: complete implementation here
    size_t row = move.row;
    size_t col = move.column;
    int points = 0;
    int currentWordPoints = 0;
    int word_multiplier = 1;
    TileKind empty(0, 0);

    vector<string> words;
    string word = "";
    bool foundStart = false;
    size_t prevRow, nextRow;
    size_t prevCol, nextCol;
    string hookWord = "";
    bool hooked = false;

    for (auto tile : move.tiles) {
        if (row == start.row && col == start.row) {
            foundStart = true;
        }

        word += tile.letter;
        hookWord = "";
        hookWord += tile.letter;

        // find the position on the board
        Position position(row, col);

        if (!is_in_bounds(position)) {
            return PlaceResult("Getting out of bounds");
        }

        BoardSquare square = squares[row][col];

        if (in_bounds_and_has_tile(position)) {
            if (word.size() == 1) {
                return PlaceResult("Already occupied");
            }
            hooked = true;
            if (move.direction == Direction::ACROSS) {
                col++;
            } else {
                row++;
            }
            points += square.get_tile_kind().points;
            Position position(row, col);
            BoardSquare square = squares[row][col];
        }

        square.set_tile_kind(tile);
        if (move.direction == Direction::ACROSS) {
            int wordPoints = square.get_points();
            // move upwards to find hooks
            for (prevRow = row - 1; prevRow > 0; prevRow--) {
                BoardSquare square = squares.at(prevRow).at(col);
                if (square.has_tile()) {
                    hookWord = square.get_tile_kind().letter + hookWord;
                    wordPoints += square.get_tile_kind().points;
                } else {
                    break;
                }
            }
            // move downwards to find hooks
            for (nextRow = row + 1; nextRow < rows; nextRow++) {
                BoardSquare square = squares.at(nextRow).at(col);
                if (square.has_tile()) {
                    hookWord = hookWord + square.get_tile_kind().letter;
                    wordPoints += square.get_tile_kind().points;
                } else {
                    break;
                }
            }
            if (hookWord.size() != 1) {
                words.push_back(hookWord);
                hookWord = "";
                hookWord += tile.letter;
                hooked = true;
                points += (wordPoints * square.word_multiplier);
            }

        } else {
            int wordPoints = square.get_points();
            for (prevCol = col - 1; prevCol > 0; prevCol--) {
                BoardSquare square = squares.at(row).at(prevCol);
                if (square.has_tile()) {
                    hookWord = square.get_tile_kind().letter + hookWord;
                    wordPoints += square.get_tile_kind().points;
                } else {
                    break;
                }
            }
            // move downwards to find hooks
            for (nextCol = col + 1; nextCol < columns; nextCol++) {
                BoardSquare square = squares.at(row).at(nextCol);
                if (square.has_tile()) {
                    hookWord = hookWord + square.get_tile_kind().letter;
                    wordPoints += square.get_tile_kind().points;
                } else {
                    break;
                }
            }

            if (hookWord.size() != 1) {
                words.push_back(hookWord);
                hooked = true;
                points += (wordPoints * square.word_multiplier);
            }
        }
        currentWordPoints += square.get_points();
        word_multiplier = word_multiplier * square.word_multiplier;

        if (move.direction == Direction::ACROSS) {
            col++;
        } else {
            row++;
        }
    }
    if (move_index == 0 && foundStart == false) {
        return PlaceResult("Not Covering Start");
    }

    // once the placement has been done a further check needed
    if (move.direction == Direction::ACROSS) {
        hookWord = word;

        for (prevCol = move.column - 1; prevCol > 0; prevCol--) {
            BoardSquare square = squares.at(row).at(prevCol);
            if (square.has_tile()) {
                hookWord = square.get_tile_kind().letter + hookWord;
                currentWordPoints += square.get_tile_kind().points;
            } else {
                break;
            }
        }

        for (nextCol = move.column + word.size(); nextCol < columns; nextCol++) {
            BoardSquare square = squares.at(row).at(nextCol);
            if (square.has_tile()) {
                hookWord = hookWord + square.get_tile_kind().letter;
                currentWordPoints += square.get_tile_kind().points;
            } else {
                break;
            }
        }
        words.push_back(hookWord);
    } else {
        hookWord = word;
        for (prevRow = move.row - 1; prevRow > 0; prevRow--) {
            BoardSquare square = squares.at(prevRow).at(col);
            if (square.has_tile()) {
                hookWord = square.get_tile_kind().letter + hookWord;
                points += square.get_tile_kind().points;
            } else {
                break;
            }
        }
        // move downwards to find hooks
        for (nextRow = move.row + word.size() + 1; nextRow < rows; nextRow++) {
            BoardSquare square = squares.at(nextRow).at(col);
            if (square.has_tile()) {
                hookWord = hookWord + square.get_tile_kind().letter;
                points += square.get_tile_kind().points;
            } else {
                break;
            }
        }
    }
    if (hooked || hookWord.size() > word.size() || move_index == 0) {
        points = points + (currentWordPoints * word_multiplier);
        return PlaceResult(words, points);
    } else {
        return PlaceResult("not hooked");
    }
}

PlaceResult Board::place(const Move& move) {
    PlaceResult r = test_place(move);
    int row = move.row;
    int col = move.column;

    if (r.valid) {
        for (auto tile : move.tiles) {
            Position pos(row, col);
            BoardSquare square = squares.at(row).at(col);
            square.set_tile_kind(tile);
            squares[row][col] = square;

            if (move.direction == Direction::ACROSS) {
                col++;
            } else {
                row++;
            }
        }
    }
    move_index++;
    return r;
}

// The rest of this file is provided for you. No need to make changes.

BoardSquare& Board::at(const Board::Position& position) { return this->squares.at(position.row).at(position.column); }

const BoardSquare& Board::at(const Board::Position& position) const {
    return this->squares.at(position.row).at(position.column);
}

bool Board::is_in_bounds(const Board::Position& position) const {
    return position.row < this->rows && position.column < this->columns;
}

bool Board::in_bounds_and_has_tile(const Position& position) const {
    return is_in_bounds(position) && at(position).has_tile();
}

void Board::print(ostream& out) const {
    // Draw horizontal number labels
    for (size_t i = 0; i < BOARD_TOP_MARGIN - 2; ++i) {
        out << std::endl;
    }
    out << FG_COLOR_LABEL << repeat(SPACE, BOARD_LEFT_MARGIN);
    const size_t right_number_space = (SQUARE_OUTER_WIDTH - 3) / 2;
    const size_t left_number_space = (SQUARE_OUTER_WIDTH - 3) - right_number_space;
    for (size_t column = 0; column < this->columns; ++column) {
        out << repeat(SPACE, left_number_space) << std::setw(2) << column + 1 << repeat(SPACE, right_number_space);
    }
    out << std::endl;

    // Draw top line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_TOP_LEFT, T_DOWN, L_TOP_RIGHT, out);
    out << endl;

    // Draw inner board
    for (size_t row = 0; row < this->rows; ++row) {
        if (row > 0) {
            out << repeat(SPACE, BOARD_LEFT_MARGIN);
            print_horizontal(this->columns, T_RIGHT, PLUS, T_LEFT, out);
            out << endl;
        }

        // Draw insides of squares
        for (size_t line = 0; line < SQUARE_INNER_HEIGHT; ++line) {
            out << FG_COLOR_LABEL << BG_COLOR_OUTSIDE_BOARD;

            // Output column number of left padding
            if (line == 1) {
                out << repeat(SPACE, BOARD_LEFT_MARGIN - 3);
                out << std::setw(2) << row + 1;
                out << SPACE;
            } else {
                out << repeat(SPACE, BOARD_LEFT_MARGIN);
            }

            // Iterate columns
            for (size_t column = 0; column < this->columns; ++column) {
                out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL;
                const BoardSquare& square = this->squares.at(row).at(column);
                bool is_start = this->start.row == row && this->start.column == column;

                // Figure out background color
                if (square.word_multiplier == 2) {
                    out << BG_COLOR_WORD_MULTIPLIER_2X;
                } else if (square.word_multiplier == 3) {
                    out << BG_COLOR_WORD_MULTIPLIER_3X;
                } else if (square.letter_multiplier == 2) {
                    out << BG_COLOR_LETTER_MULTIPLIER_2X;
                } else if (square.letter_multiplier == 3) {
                    out << BG_COLOR_LETTER_MULTIPLIER_3X;
                } else if (is_start) {
                    out << BG_COLOR_START_SQUARE;
                }

                // Text
                if (line == 0 && is_start) {
                    out << "  \u2605  ";
                } else if (line == 0 && square.word_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'W' << std::setw(1)
                        << square.word_multiplier;
                } else if (line == 0 && square.letter_multiplier > 1) {
                    out << FG_COLOR_MULTIPLIER << repeat(SPACE, SQUARE_INNER_WIDTH - 2) << 'L' << std::setw(1)
                        << square.letter_multiplier;
                } else if (line == 1 && square.has_tile()) {
                    char l = square.get_tile_kind().letter == TileKind::BLANK_LETTER ? square.get_tile_kind().assigned
                                                                                     : ' ';
                    out << repeat(SPACE, 2) << FG_COLOR_LETTER << square.get_tile_kind().letter << l
                        << repeat(SPACE, 1);
                } else if (line == SQUARE_INNER_HEIGHT - 1 && square.has_tile()) {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH - 1) << FG_COLOR_SCORE << square.get_points();
                } else {
                    out << repeat(SPACE, SQUARE_INNER_WIDTH);
                }
            }

            // Add vertical line
            out << FG_COLOR_LINE << BG_COLOR_NORMAL_SQUARE << I_VERTICAL << BG_COLOR_OUTSIDE_BOARD << std::endl;
        }
    }

    // Draw bottom line
    out << repeat(SPACE, BOARD_LEFT_MARGIN);
    print_horizontal(this->columns, L_BOTTOM_LEFT, T_UP, L_BOTTOM_RIGHT, out);
    out << endl << rang::style::reset << std::endl;
}
