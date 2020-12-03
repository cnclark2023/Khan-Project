# Khan-Project
C++ Scrabble Game with Error Handling 

Game Overview:
This is game is to be played between 1-8 human players. It will be played through the console with std::cin and std::cout for interaction. All of the generic quantities (language, dictionary, how many of what kinds of tiles, points per tile, board size and layout, etc) are configured via files of a standard format.

Make Instructions:
Note: GTest should be configured
1)Type "make" in terminal
2) Type "./scrabble config/config.txt  < input/example.txt" to display board
3) Type "./scrabble config/config.txt" for interactive gameplay

Example of Game Play Commands:
PASS: pass the turn.
EXCHANGE aa?: put two tiles of the letter a and a blank tile back into the bag and draw three new tiles.
PLACE | 3 3 CAKE: put the tiles C, A, K, E down in order, starting at row 3, column 3, going down.
PLACE - 5 1 POE?MON: imagining that the previous move (CAKE) has been executed, and no other relevant tiles are around: place a horizontal word starting in row 5, column 1 (left border of the board), placing the tile P there, O in row 5, column 2, E in row 5, column 4, placing a blank tile that is interpreted as an M in row 5, column 5, and so on. This forms the word POKEMON with the K that was already there at row 5, column 3.
