#ifndef TABLE_H
#define TABLE_H

#include <bits/stdc++.h>

#include "Piece.h"
#include "PlaySide.h"
#define TABLE_SZ 8

class Table {
 public:
	Table();
	// contructor of a table with a random layout
	Table(int random);
	Table(int custom, int youchoose);
  	uint8_t getPiece(int x, int y);
  	void setPiece(int x, int y, uint8_t new_piece);
  	void update_states();
	// temporary public
	uint8_t table[8][8];
 private:
  	
};

#endif