#include "Table.h"

#include <bits/stdc++.h>

#include "Piece.h"

uint8_t Table::getPiece(int x, int y) {
    return table[x][y];
}

void Table::setPiece(int x, int y, uint8_t piece) {
    table[x][y] = piece;
}