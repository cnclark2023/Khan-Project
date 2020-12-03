#include "scrabble.h"

#include "formatting.h"
#include <iomanip>
#include <iostream>
#include <map>

using namespace std;

// Given to you. this does not need to be changed
Scrabble::Scrabble(const ScrabbleConfig& config)
        : hand_size(config.hand_size),
          minimum_word_length(config.minimum_word_length),
          tile_bag(TileBag::read(config.tile_bag_file_path, config.seed)),
          board(Board::read(config.board_file_path)),
          dictionary(Dictionary::read(config.dictionary_file_path)) {}

void Scrabble::add_players() {
    size_t num;
    string name;

    cout << "Please enter number of players: ";
    cin >> num;
    cout << num << "players confirmed" << endl;
    for (size_t i = 0; i < num; i++) {
        cout << "Please enter name for player " << i + 1 << " : ";
        cin >> name;
        cout << "Player " << i + 1 << "named \"" << name << "\" has been added" << endl;
        HumanPlayer* p = new HumanPlayer(name, hand_size);
        shared_ptr<HumanPlayer> p2(p);
        players.push_back(p2);
    }
}
// Game Loop should cycle through players and get and execute that players move
// until the game is over.
void Scrabble::game_loop() {
    // TODO: implement this.

    while (!tile_bag.get_kinds().empty()) {
        size_t passes = 0;
        for (auto player : players) {
            board.print(cout);
            vector<TileKind> tiles
                    = tile_bag.remove_random_tiles(player.get()->get_hand_size() - player.get()->count_tiles());
            player.get()->add_tiles(tiles);
            Move m = player.get()->get_move(board, dictionary);
            if (m.kind == MoveKind::PLACE) {
                if (m.tiles.size()) {
                    PlaceResult r = board.place(m);
                    player.get()->remove_tiles(m.tiles);
                    player.get()->add_points(r.points);
                    cout << "You gained " << r.points << endl;
                }
            } else if (m.kind == MoveKind::PASS) {
                passes++;
            }

            cout << "current score: " << player.get()->get_points() << endl;
            cout << "Press [enter] to continue." << endl;
            cin.ignore();
        }
        if (passes == players.size()) {
            break;
        }
    }

    // Useful cout expressions with fancy colors. Expressions in curly braces, indicate values you supply.
    // cout << "You gained " << SCORE_COLOR << {points} << rang::style::reset << " points!" << endl;
    // cout << "Your current score: " << SCORE_COLOR << {points} << rang::style::reset << endl;
    // cout << endl << "Press [enter] to continue.";
}

// Performs final score subtraction. Players lose points for each tile in their
// hand. The player who cleared their hand receives all the points lost by the
// other players.
void Scrabble::final_subtraction(vector<shared_ptr<Player>>& plrs) {

    Player* finisher = nullptr;

    for (auto player : plrs) {
        int score = player.get()->get_hand_value();
        if (score == 0) {
            finisher = player.get();
        }
    }

    for (auto player : plrs) {
        int score = player.get()->get_hand_value();
        if (score != 0) {
            if (player.get()->get_points() > 0) {
                player.get()->add_points(-score);
            }
            if (finisher != nullptr) {
                finisher->add_points(score);
            }
        }
    }
}

// You should not need to change this function.
void Scrabble::print_result() {
    // Determine highest score
    size_t max_points = 0;
    for (auto player : this->players) {
        if (player->get_points() > max_points) {
            max_points = player->get_points();
        }
    }

    // Determine the winner(s) indexes
    vector<shared_ptr<Player>> winners;
    for (auto player : this->players) {
        if (player->get_points() >= max_points) {
            winners.push_back(player);
        }
    }

    cout << (winners.size() == 1 ? "Winner:" : "Winners: ");
    for (auto player : winners) {
        cout << SPACE << PLAYER_NAME_COLOR << player->get_name();
    }
    cout << rang::style::reset << endl;

    // now print score table
    cout << "Scores: " << endl;
    cout << "---------------------------------" << endl;

    // Justify all integers printed to have the same amount of character as the high score, left-padding with spaces
    cout << setw(static_cast<uint32_t>(floor(log10(max_points) + 1)));

    for (auto player : this->players) {
        cout << SCORE_COLOR << player->get_points() << rang::style::reset << " | " << PLAYER_NAME_COLOR
             << player->get_name() << rang::style::reset << endl;
    }
}

// You should not need to change this.
void Scrabble::main() {
    add_players();
    game_loop();
    final_subtraction(this->players);
    print_result();
}
