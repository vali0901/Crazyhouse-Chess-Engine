#ifndef TABLE_H
#define TABLE_H

#include <bits/stdc++.h>

#include "Piece.h"
#include "PlaySide.h"
#define TABLE_SZ 8

class Table {
 public:
  std::pair<Piece, PlaySide> getPiece(int x, int y);
  void setPiece(int x, int y, Piece new_piece, PlaySide playSide);
 private:
  std::vector<std::vector<std::pair<Piece, PlaySide>>> table;
};

#endif