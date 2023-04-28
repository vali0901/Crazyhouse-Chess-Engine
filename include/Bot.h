#ifndef BOT_H
#define BOT_H
#include <bits/stdc++.h>

#include "Move.h"
#include "PlaySide.h"
#include "Table.h"
#include "PieceHandlers.h"

class Bot {
 private:
  static const std::string BOT_NAME;
 public:
  /* Declare custom fields below */
  Table table;
  // PieceHandlers pieceHandler;
  /* Declare custom fields above */
  Bot();

  /**
   * Record move (either by enemy in normal mode, or by either side
   * in force mode) in custom structures
   * @param move received move
   * @param sideToMode side to move
   */
  void recordMove(Move* move, PlaySide sideToMove);

  /**
   * Calculates next move, in response to enemyMove
   * @param enemyMove the enemy's last move
   *                  null if this is the opening move, or previous
   *                  move has been recorded in force mode
   * @return your move
   */
  Move calculateNextMove(PlaySide sideToMove);

  static std::string getBotName();
};

bool checkCastling(Table &table, Move *move, PlaySide &sideToMove);
bool checkEnPassant(Table &table, Move *move, PlaySide &sideToMove);
void printPiecesAvailable(Table &table);
void checkPromotedPawns(Move *move, PlaySide sideToMove, Table &table);
void checkImportantPiecesThatMoved(Move *move, Table &table);
#endif
