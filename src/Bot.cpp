#include "Bot.h"
#include "Move.h"
#include "Table.h"
#include "Main.h"
#include "Piece.h"
#include "PieceHandlers.h"

#include <bits/stdc++.h>

const std::string Bot::BOT_NAME = "en-passant";
const std::map<Piece, int> Bot::piece_scores = {{NAP, 50}, {PAWN, 100}, {KNIGHT, 500}, {BISHOP, 600},{ROOK, 700}, {QUEEN, 1000}, {KING, 0}};
const std::map<Piece, int> Bot::capt_piece_scores = {{NAP, 0},{PAWN, 10}, {KNIGHT, 200}, {BISHOP, 200},{ROOK, 200}, {QUEEN, 700}, {KING, 0}};
const std::vector<float> Bot::placement = {1, 1.25, 1.75, 2.0, 2.0, 1.75, 1.25, 1};

const int Bot::winScore = INT_MAX;
const int Bot::drawScore = 2500;
const float Bot::protectedPiece = 1.5;
const float Bot::attackedPiece = 1.25;
const float Bot::attackedNAP = 1.5;
const int Bot::kingInCheck = 5000;
const float Bot::attacker_buf = 0.5; 
Bot::Bot() {
}

void Bot::recordMove(Move* move, PlaySide sideToMove) {
  Move::convertStrToIdx(*move);

  checkPromotedPawns(move, sideToMove, table);

  checkImportantPiecesThatMoved(move, table);

  checkCastling(table, move, sideToMove);
  
  checkEnPassant(table, move, sideToMove);

  uint8_t moved_piece = PieceHandlers::createPiece(NAP, NONE);

  // if move source exists(normal/promotion), the piece is removed and saved in moved_piece
  if (move->source_idx.has_value())
  {
    moved_piece = table.getPiece(move->source_idx.value());
    table.setPiece(move->source_idx.value(), PieceHandlers::createPiece(NAP, NONE));
  }

  if (move->destination_idx.has_value())
  {
    // check if it captured something
    if (PieceHandlers::getType(table.getPiece(move->destination_idx.value())) != NAP) {
      // piece = the piece that the player can add to their captured pieces
      // if the piece was a promoted pawn, the player that captured it received a pawn
      Piece piece = PieceHandlers::getType(table.getPiece(move->destination_idx.value()));

      std::vector<std::pair<int8_t, int8_t>> &promotedPieces = (sideToMove == BLACK) ? table.promotedPawnsWhite : table.promotedPawnsBlack;
      for (unsigned long i = 0; i < promotedPieces.size(); i++) {
        if (promotedPieces[i] == move->destination_idx.value()) {
          piece = PAWN;

          promotedPieces[i] = promotedPieces.back();
          promotedPieces.pop_back();

          break;
        }
      } 
      table.addToCaptured(table, piece, sideToMove);
    }

    table.setPiece(move->destination_idx.value(), moved_piece);
  }
  // drop in, remove from the array of pieces available
  if (!move->source_idx.has_value() && move->destination_idx.has_value() && move->replacement.has_value()) {

    std::vector<Piece> &capturedPieces = (sideToMove == BLACK) ? table.capturedByBlack : table.capturedByWhite;   
    
    for (unsigned long i = 0; i < capturedPieces.size(); i++) {
      if (capturedPieces[i] == move->replacement) {
        capturedPieces[i] = capturedPieces.back();
        capturedPieces.pop_back();
        break;
      }
    }
  }

  // in case of promotion/dropIn, the destination is replaced with "replacement"
  if (move->getReplacement().has_value())
    table.setPiece(move->destination_idx.value(), PieceHandlers::createPiece(move->getReplacement().value(), sideToMove));
    
  table.update_states();
  table.last_move = *move;
}

PlaySide get_opponent(PlaySide player) {
  if(player == WHITE)
    return BLACK;
  return WHITE;
}

bool Bot::isCheckMate(Table table, PlaySide playside) {
  std::vector<Move> poss_moves;
  table.generateAllPossibleMoves(playside, table.last_move, poss_moves);
  return poss_moves.size() == 0 && table.kingIsInCheck(playside);

}

bool Bot::isStaleMate(Table table, PlaySide playside) {
  std::vector<Move> poss_moves;
  table.generateAllPossibleMoves(playside, table.last_move, poss_moves);
  return poss_moves.size() == 0 && !table.kingIsInCheck(playside);
}

int get_score_by_position(Piece piece, int x, int y) {
  return (int)(Bot::placement[x] * Bot::placement[y] * Bot::piece_scores.at(piece));
}

int evaluate(Table &table, PlaySide sideToMove) {
  // check for checkmate and stalemate
  // need a new parameter, last_move;
  if(Bot::isCheckMate(table, sideToMove))
    return Bot::winScore;

  if(Bot::isStaleMate(table, sideToMove))
    return Bot::drawScore;
  
  int score = 0;
  
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      // procces slots attacked by me
      if(PieceHandlers::isAttackedBy(table.table[i][j], sideToMove)) {
          // enemy's king is in check
          if(PieceHandlers::getType(table.table[i][j]) == KING
            && PieceHandlers::getColor(table.table[i][j] != sideToMove))
            score += Bot::kingInCheck;
          // enemy's atacked piece
          else if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] != sideToMove))
            score += Bot::attackedPiece * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j);
          // friendly protected piece
          else if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] == sideToMove))
            score += Bot::protectedPiece * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j);
          // empty slot, attacked by me
          else if(PieceHandlers::getType(table.table[i][j]) == NAP)
            score += Bot::attackedNAP * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j);
      // process slots attacked by enemy
      } else if(PieceHandlers::isAttackedBy(table.table[i][j], get_opponent(sideToMove))) {
        // my king is in check
        if(PieceHandlers::getType(table.table[i][j]) == KING
            && PieceHandlers::getColor(table.table[i][j] == sideToMove))
            score -= (int)(Bot::attacker_buf * Bot::kingInCheck);
          // my atacked piece
          else if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] == sideToMove))
            score -= (int)(Bot::attacker_buf * Bot::attackedPiece * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j));
          // enemy protected piece (maybe we change this a lil' bit)
          else if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] == sideToMove))
            score -= (int)(Bot::attacker_buf * Bot::protectedPiece * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j));
          // empty slot, attacked by me
          else if(PieceHandlers::getType(table.table[i][j]) == NAP)
            score -= (int)(Bot::attacker_buf * Bot::attackedNAP * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j));
      // process not attacked/ protected pieces
      } else {
        // my piece
        if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] == sideToMove))
            score += get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j);
        // enemy's piece
        else if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] != sideToMove))
            score -= (int)(Bot::attacker_buf * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j));
        // not interested in NAP slots
      }
    }
  }

  // calculate captured pieces score
  int cscore1 = 0, cscore2 = 0;

  for(auto piece : table.capturedByWhite)
    cscore1 += Bot::capt_piece_scores.at(piece);

  for(auto piece : table.capturedByBlack)
    cscore2 += Bot::capt_piece_scores.at(piece);

  if(sideToMove == WHITE) {
    score += cscore1;
    score -= (int)(Bot::attacker_buf * cscore2);
  } else {
    score += cscore2;
    score -= (int)(Bot::attacker_buf * cscore1);
  }

  return score;
}

bool game_over(Table &table, PlaySide playside) {
  return Bot::isStaleMate(table, playside) || Bot::isCheckMate(table, playside) ;
}

#define INF (1 << 30)

std::pair<int, Move> Bot::alphabeta_negamax(Table &table, int depth, PlaySide sideToMove, int alpha, int beta) {
    // STEP 1: game over or maximum recursion depth was reached
    if (game_over(table, sideToMove) || depth == 0) {
       Move dummy;
       return std::pair(evaluate(table, sideToMove), dummy);
    }
 
    // STEP 2: generate all possible moves for player
    // Note: sort moves descending by score (if possible) for maximizing the number of cut-off actions
    // (or generete the moves already sorted by a custom criterion)
    std::vector<Move> allMoves;
    table.generateAllPossibleMoves(sideToMove, table.last_move, allMoves);
 
 
    // STEP 3: try to apply each move - compute best score
    int best_score = -INF;
    Move best_move;
    for (auto move : allMoves) {
        // STEP 3.1: do move
        // TODO: make a deep copy of the table
        recordMove(&move, sideToMove);
 
        // STEP 3.2: play for the opponent
        auto [score, _] = alphabeta_negamax(table, depth - 1, get_opponent(sideToMove), -beta, -alpha);
        score = -score;
        // opponent allows player to obtain this score if player will do current move.
        // player chooses this move only if it has a better score.
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
 
        // STEP 3.3: update alpha (found a better move?)
        if (best_score > alpha) {
            alpha = best_score;
        }
 
        // STEP 3.4: cut-off
        // * already found the best possible score (alpha == beta)
        // OR
        // * on this branch we can obtain a score (alpha) better than the
        // maximum allowed score by the opponent => drop the branch because
        // opponent also plays optimal
        if (alpha >= beta) {
            break;
        }
 
        // STEP 3.4: undo move
        // undo_move(state, move);
    }
 
    // STEP 4: return best allowed score
    // [optional] also return the best move
    return std::pair(best_score, best_move);
}

Move Bot::calculateNextMove(PlaySide sideToMove) {
  std::vector<Move> allMoves;
  auto [_, move] = alphabeta_negamax(table, 5, sideToMove, -INF, INF);
  return move;
}

std::string Bot::getBotName() { return Bot::BOT_NAME; }

bool checkEnPassant(Table &table, Move *move, PlaySide &sideToMove)
{
  // checks if the last move forwarded the piece 2 positions on the same column, if that piece is a pawn
  // and if the current move is a normal move(has src + dst) and it moves a pawn as well
  if (table.last_move.destination_idx.has_value() &&
      table.last_move.source_idx.has_value() &&
      abs(table.last_move.destination_idx.value().first - table.last_move.source_idx.value().first) == 2 &&
      PieceHandlers::getType(table.getPiece(table.last_move.destination_idx.value())) == PAWN &&
      move->source_idx.has_value() &&
      move->destination_idx.has_value() &&
      PieceHandlers::getType(table.getPiece(move->source_idx.value())) == PAWN)
  {
    
    int x_src = move->source_idx.value().first;
    int y_src = move->source_idx.value().second;

    int x_dst = move->destination_idx.value().first;
    int y_dst = move->destination_idx.value().second;

    int x_last = table.last_move.destination_idx.value().first;
    int y_last = table.last_move.destination_idx.value().second;

    // if before the move is made, the two pawns are not on the same line, at the distance
    // of 1 column apart, there's no way it's an en-passant
    if (x_src == x_last && abs(y_src - y_last) == 1 && abs(y_dst - y_src) == 1)
    {
      // offset = the row behind last_move piece, despite it's colour
      int offset = 0;
      
      if (sideToMove == BLACK)
        offset = +1;
      else
        offset = -1;

      if (x_dst == x_last + offset && PieceHandlers::getType(table.getPiece(x_dst, y_dst)) == NAP)
      {
        table.addToCaptured(table, PieceHandlers::getType(table.getPiece(table.last_move.destination_idx.value())), sideToMove);

        table.setPiece(table.last_move.destination_idx.value(), PieceHandlers::createPiece(NAP, NONE));
        return true;
      }
    }
  }
  return false;
}

bool checkCastling(Table &table, Move *move, PlaySide &sideToMove)
{
  // check if castle: the only way a king can move 2 squares is by castling
  if (move->source_idx.has_value() && move->destination_idx.has_value() && 
      PieceHandlers::getType(table.getPiece((move->source_idx.value()))) == KING &&
      abs(move->source_idx.value().second - move->destination_idx.value().second) == 2)
  {
    int kx_src = move->source_idx.value().first;

    int ky_src = move->source_idx.value().second;
    int ky_dst = move->destination_idx.value().second;
    
    uint8_t moved_rook;
    table.pieceHasMoved(sideToMove == WHITE ? 0b00001111 : 0b11110000);

    if (ky_dst > ky_src) // the right rook should be moved
    {
      moved_rook = table.getPiece(kx_src, 7);
      table.setPiece(kx_src, 7, PieceHandlers::createPiece(NAP, NONE));
      table.setPiece(kx_src, ky_src + 1, moved_rook);
    } else { // the left rook is moved

      moved_rook = table.getPiece(kx_src, 0);
      table.setPiece(kx_src, 0, PieceHandlers::createPiece(NAP, NONE));
      table.setPiece(kx_src, ky_src - 1, moved_rook);
    }
    return true;
  }
  return false;
}

void checkPromotedPawns(Move *move, PlaySide sideToMove, Table &table) {
  // fprintf(f_out, "in checkPromotedPawns\n");
  if (move->source_idx.has_value() && move->destination_idx.has_value() && move->replacement.has_value()) {
    if (sideToMove == BLACK)
      table.promotedPawnsBlack.push_back(move->destination_idx.value());
    else
      table.promotedPawnsWhite.push_back(move->destination_idx.value());
  }
  if (move->source_idx.has_value() && move->destination_idx.has_value() && !move->replacement.has_value()) {
    std::vector<std::pair<int8_t, int8_t>> &promotedPawns = sideToMove == BLACK ? table.promotedPawnsBlack : table.promotedPawnsWhite;
    for (auto& pos : promotedPawns) {
      if (pos == move->source_idx.value()) {
        pos = move->destination_idx.value();
      }
    }
  }
}

void checkImportantPiecesThatMoved(Move *move, Table &table) {
   if (!move->isDropIn()) {
    if(PieceHandlers::getType(table.getPiece(move->source_idx.value())) == KING) {
    if(PieceHandlers::getColor(table.getPiece(move->source_idx.value())) == WHITE) {
      table.wKx = move->destination_idx->first;
      table.wKy = move->destination_idx->second;
      table.pieceHasMoved(0b11011111);
    } else {
      table.bKx = move->destination_idx->first;
      table.bKy = move->destination_idx->second;
      table.pieceHasMoved(0b11111101);
    }
  } else if (PieceHandlers::getType(table.getPiece(move->source_idx.value())) == ROOK) {
    // left rook has moved
    if(move->source_idx.value() == std::pair((int8_t)0, (int8_t)0) || move->source_idx.value() == std::pair((int8_t)7, (int8_t)0))
      table.pieceHasMoved(PieceHandlers::getColor(table.getPiece((move->source_idx.value()))) == WHITE ? 0b10111111 : 0b11111011 );
    // right rook has moved
    if(move->source_idx.value() == std::pair((int8_t)0, (int8_t)7) || move->source_idx.value() == std::pair((int8_t)7, (int8_t)7))
      table.pieceHasMoved(PieceHandlers::getColor(table.getPiece((move->source_idx.value()))) == WHITE ? 0b11101111 : 0b11111110 );
    }
  }
}
