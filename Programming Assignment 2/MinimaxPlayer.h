#pragma once
/*
 * MinimaxPlayer.h
 *
 *  Created on: Apr 17, 2015
 *      Author: wong
 */

#ifndef MINIMAXPLAYER_H
#define MINIMAXPLAYER_H

#include "OthelloBoard.h"
#include "Player.h"
#include <vector>

using namespace std;

 /**
  * This class represents an AI player that uses the Minimax algorithm to play the game
  * intelligently.
  */
class MinimaxPlayer : public Player {
public:

	/**
	 * @param symb This is the symbol for the minimax player's pieces
	 */
	MinimaxPlayer(char symb);

	/**
	 * Destructor
	 */
	virtual ~MinimaxPlayer();

	// Functions made for cleaner development design and to make 
	// grading easier due to the naming of the functions
	int utility(OthelloBoard* board);
    vector<OthelloBoard*> successor(OthelloBoard* board, char symbol);
    void minimax(OthelloBoard* board, int& col, int& row);
    int minValue(OthelloBoard* board, int& col, int& row, char symbol);
    int maxValue(OthelloBoard* board, int& col, int& row, char symbol);

	/**
	 * @param b The board object for the current state of the board
	 * @param col Holds the return value for the column of the move
	 * @param row Holds the return value for the row of the move
	 */
	void get_move(OthelloBoard* b, int& col, int& row);

	/**
	 * @return A copy of the MinimaxPlayer object
	 * This is a virtual copy constructor
	 */
	MinimaxPlayer* clone();

private:

};


#endif