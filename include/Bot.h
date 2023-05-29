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
  static bool isCheckMate(Table table, PlaySide playside, std::vector<Move> poss_moves);
  static bool isStaleMate(Table table, PlaySide playside, std::vector<Move> poss_moves);
  static const std::map <Piece, int> piece_scores;
  static const std::map<Piece, int> capt_piece_scores;
  static const std::vector<float> placement;
  static const int kingInCheck;
  static const int winScore;
  static const int drawScore;
  static const float protectedPiece;
  static const float attackedPiece;
  static const float attackedNAP;
  static const float attacker_buf;
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
  std::pair<int, Move> alphabeta_negamax(int depth, PlaySide sideToMove, int alpha, int beta);
  static std::string getBotName();
};

bool checkCastling(Table &table, Move *move, PlaySide &sideToMove);
bool checkEnPassant(Table &table, Move *move, PlaySide &sideToMove);
void checkPromotedPawns(Move *move, PlaySide sideToMove, Table &table);
void checkImportantPiecesThatMoved(Move *move, Table &table);
std::pair<int, Move> alphabeta_negamax(int depth, PlaySide sideToMove, int alpha, int beta);
#endif
