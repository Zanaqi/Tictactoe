extern char mark;           // player mark
extern int difficulty;      // 1 - easy, 2 - medium, 3 - hard
extern int current_player;  // 1 and 2 for two player, 3 for single player
extern char square[9];      // game state, reinitialised after every game has ended / user quit
extern int starting_player; // starting player for the game, either 1 or 2, change after every round
extern int player1_score;   // score tracker for player 1
extern int player2_score;   // score tracker for player 2

int checkwin();
