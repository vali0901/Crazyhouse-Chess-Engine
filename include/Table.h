#ifndef TABLE_H
#define TABLE_H

#include <bits/stdc++.h>

#include "Piece.h"
#include "PlaySide.h"
#define TABLE_SZ 8

class Table {
 public:
  uint8_t getPiece(int x, int y);
  void setPiece(int x, int y, uint8_t new_piece);
 private:
  uint8_t table[64];
};

#endif