#ifndef TABLE_H
#define TABLE_H

#include <bits/stdc++.h>

#include "Piece.h"

#define TABLE_SZ 8

class Table {
 public:
  Piece getPiece(int x, int y);
  void setPiece(int x, int y, Piece new_piece);
 private:
  std::vector<std::vector<Piece>> table;
};

#endif