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
	uint8_t getPiece(std::pair<int, int>);
	void setPiece(std::pair<int, int> coords, uint8_t piece);
	bool kingIsInCheck(PlaySide color);
	void pieceHasMoved(uint8_t mask);
  	void update_states();
	void generateAllPossibleMoves(PlaySide turn, Move last_move, std::vector<Move> &moves);
    // void addToCaptured(table.getPiece(move->destination_idx.value()), sideToMove);
	void addToCaptured(Table &table, Piece piece, PlaySide playside);
	// temporary public
	uint8_t table[8][8];
	Move last_move;
	uint8_t rocinfo;
	int8_t wKx, wKy, bKx, bKy;
	std::vector<Piece> capturedByWhite, capturedByBlack;
 private:
  	
};

#endif