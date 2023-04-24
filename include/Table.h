#ifndef TABLE_H
#define TABLE_H

#include <bits/stdc++.h>

#include "Piece.h"
#include "PlaySide.h"
#include "Move.h"

#define TABLE_SZ 8

class Table {
 public:
	Table();
	// contructor of a table with a random layout
	Table(int random);
	Table(int custom, int youchoose);
  	uint8_t getPiece(int x, int y);
  	void setPiece(int x, int y, uint8_t new_piece);
	bool kingIsInCheck(PlaySide color);
  	void update_states();
	std::vector<Move> generateAllPossibleMoves(PlaySide turn, Move last_move);
	// temporary public
	uint8_t table[8][8];
	uint8_t wKing, bKing;
	int8_t wKx, wKy, bKx, bKy;
 private:
  	
};

#endif