/*
 * MinimaxPlayer.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: wong
 */
#include <iostream>
#include <assert.h>
#include "MinimaxPlayer.h"
#include <climits>

const int MAX_COL = 4;
const int MAX_ROW = 4;

MinimaxPlayer::MinimaxPlayer(char symb) : Player(symb) {

}

MinimaxPlayer::~MinimaxPlayer() {

}

int MinimaxPlayer::utility(OthelloBoard* board){
    if (symbol == 'O') {
        return 
		board->count_score(board->get_p2_symbol()) - 
		board->count_score(board->get_p1_symbol());
    } 
	else if (symbol == 'X') { 
        return 
		board->count_score(board->get_p1_symbol()) - 
		board->count_score(board->get_p2_symbol());
    }
	else {
		cerr << "== Error Thrown in 'int MinimaxPlayer::utility(OthelloBoard* board)': Incorrect Player Symbol trying to be read!" << endl;
	}
}

vector<OthelloBoard*> MinimaxPlayer::successor(OthelloBoard* board, char symbol){
    vector<OthelloBoard*> successorBoard;
    for (int colIndex = 0; colIndex < MAX_COL; colIndex++) {
        for (int rowIndex = 0; rowIndex < MAX_ROW; rowIndex++) {
            if (board->is_legal_move(colIndex,rowIndex,symbol)) {
                successorBoard.push_back(new OthelloBoard(*board));
                successorBoard.back()->play_move(colIndex,rowIndex,symbol);
                successorBoard.back()->set_row(rowIndex);
                successorBoard.back()->set_col(colIndex);
            }
        }
    }
    return successorBoard;
}

void MinimaxPlayer::minimax(OthelloBoard* board, int& col, int& row){
    if (symbol == board->get_p1_symbol() || symbol == board->get_p2_symbol()) {
        maxValue(board,col,row, symbol);
    } 
	else {
		cerr << "== Error Thrown in 'void MinimaxPlayer::minimaxDecision(OthelloBoard* board, int& col, int& row)': Incorrect Player Symbol trying to be read!" << endl;
	}
}

int MinimaxPlayer::maxValue(OthelloBoard* board, int& col, int& row, char symbol) {
    int max = INT_MIN;
    int max_col, max_row;
    vector<OthelloBoard*> successorBoard;
    
    if (symbol == 'O' || symbol == 'X') {
        successorBoard = successor(board, symbol);
    } 
	else {
		cerr << "== Error Thrown in 'int MinimaxPlayer::maxValue(OthelloBoard* board, int& col, int& row, char symbol)': Incorrect Player Symbol trying to be read!" << endl;
	}
    
    if (successorBoard.size() == 0) {
        return utility(board);
    }

    for(int i = 0; i < successorBoard.size(); i++) {
        if (max < minValue(successorBoard[i], col, row, symbol)) {
            max = minValue(successorBoard[i], col, row, symbol);
            max_col = successorBoard[i]->get_col();
            max_row = successorBoard[i]->get_row();
        }
    }
    col = max_col;
    row = max_row;
    return max;
}

int MinimaxPlayer::minValue(OthelloBoard* board, int& col, int& row, char symbol) {
    int min = INT_MAX;
    int min_col, min_row;
    vector<OthelloBoard*> successorBoard;
	
    if (symbol == 'O') {
        successorBoard = successor(board, 'X');
    } 
	else if (symbol == 'X') {
        successorBoard = successor(board, 'O');
    }
	else {
		cerr << "== Error Thrown in 'int MinimaxPlayer::minValue(OthelloBoard* board, int& col, int& row, char symbol)': Incorrect Player Symbol trying to be read!" << endl;
	}
    
    if (successorBoard.size() == 0) {
        return utility(board);
    }

    for(int i = 0; i < successorBoard.size(); i++) {
        if (min > minValue(successorBoard[i], col, row, symbol)) {
            min = minValue(successorBoard[i], col, row, symbol);
            min_col = successorBoard[i]->get_col();
            min_row = successorBoard[i]->get_row();
        }
    }
    col = min_col;
    row = min_row;
    return min;
}


void MinimaxPlayer::get_move(OthelloBoard* b, int& col, int& row) {
    minimax(b, col, row);
}

MinimaxPlayer* MinimaxPlayer::clone() {
	MinimaxPlayer* result = new MinimaxPlayer(symbol);
    return result;
}