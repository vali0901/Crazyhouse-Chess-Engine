#include "Bot.h"
#include "Move.h"
#include "Table.h"
#include "Main.h"
#include "Piece.h"
#include "PieceHandlers.h"
#include <malloc.h>
#include <boost/functional/hash.hpp>

#include <bits/stdc++.h>

const std::string Bot::BOT_NAME = "en-passant";
const std::map<Piece, int> Bot::piece_scores = {{NAP, 50}, {PAWN, 100}, {KNIGHT, 500}, {BISHOP, 600},{ROOK, 700}, {QUEEN, 1000}, {KING, 0}};
const std::map<Piece, int> Bot::capt_piece_scores = {{NAP, 0},{PAWN, 10}, {KNIGHT, 200}, {BISHOP, 200},{ROOK, 200}, {QUEEN, 700}, {KING, 0}};
const std::vector<float> Bot::placement = {1, 1.1, 1.3, 1.5, 1.5, 1.3, 1.1, 1};

int a;
int b;

const int Bot::maxLenHT = (1 << 29);
const Bot::KeyHasher Bot::keyHasher;
std::vector<int> Bot::tablescoreHT;

const int Bot::winScore = INT_MAX;
const int Bot::drawScore = 2500;
const float Bot::protectedPiece = 1.75;
const float Bot::attackedPiece = 1.25;
const float Bot::protected_attackedPiece = 0.8;
const float Bot::attackedNAP = 1.5;
const int Bot::kingInCheck = 500;
const float Bot::attacker_buf = 0.75; 

std::size_t  Bot :: KeyHasher :: operator()(const Piece& k) const {
      using boost::hash_value;
      using boost::hash_combine;

      std::size_t seed = 0;

      hash_combine(seed,hash_value((size_t) k));
      
      return seed;
}

std::size_t  Bot :: KeyHasher :: operator()(const Move& k, Piece *src, Piece *dest) const {
      using boost::hash_value;
      using boost::hash_combine;

      std::size_t seed = 0;

      hash_combine(seed, this->operator()(k));
      if(src != NULL)
        hash_combine(seed, this->operator()(*src));
      if(dest != NULL)
        hash_combine(seed, this->operator()(*dest));
      
      return seed;
}

std::size_t  Bot :: KeyHasher :: operator()(const Move& k) const {
      using boost::hash_value;
      using boost::hash_combine;

      std::size_t seed = 0;

      if(k.source_idx.has_value())
        hash_combine(seed, hash_value(k.source_idx.value()));
      if(k.destination_idx.has_value())
        hash_combine(seed, hash_value(k.destination_idx.value()));
      if(k.replacement.has_value())
        hash_combine(seed, this->operator()(k.replacement.value()));
      
      return seed;
}  

std::size_t  Bot :: KeyHasher :: operator()(const std::vector<Move> &k) const {
      using boost::hash_value;
      using boost::hash_combine;

      std::size_t seed = 0;

      for(auto move : k) {
        hash_combine(seed, this->operator()(move));
      }
      hash_combine(seed,hash_value(k.size()));
      
      return seed;
}

std::size_t  Bot :: KeyHasher :: operator()(const Table &k) const {
      using boost::hash_value;
      using boost::hash_combine;

      std::size_t seed = 0;

      // combine rows
      // other idea : combine all rows using xor and then use that value
      for(int i = 0; i < 8; i++) {
        std::size_t row;
        memcpy(&row, k.table[i], 8);
        hash_combine(seed, row);
      }

      hash_combine(seed, this->operator()(k.last_move));
      hash_combine(seed, k.rocinfo);
      hash_combine(seed, std::pair<std::pair<int8_t, int8_t>,std::pair<int8_t, int8_t>>({k.wKx, k.wKy}, {k.bKx, k.bKy}));

      for(auto p : k.capturedByWhite) {
        hash_combine(seed, this->operator()(p));
      }

      for(auto p : k.capturedByBlack) {
        hash_combine(seed, this->operator()(p));
      }

      for(auto whatever : k.promotedPawnsWhite) {
        hash_combine(seed, hash_value(whatever));
      }

      for(auto whatever : k.capturedByBlack) {
        hash_combine(seed, hash_value(whatever));
      }
      
      return seed;
}

Bot::Bot() {
  tablescoreHT = std::vector<int>(Bot::maxLenHT, INT_MIN);
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

bool Bot::isCheckMate(Table table, PlaySide playside, std::vector<Move> poss_moves) {
  return poss_moves.size() == 0 && table.kingIsInCheck(playside);
}

bool Bot::isStaleMate(Table table, PlaySide playside, std::vector<Move> poss_moves) {
  return poss_moves.size() == 0 && !table.kingIsInCheck(playside);
}

int get_score_by_position(Piece piece, int x, int y) {
  return (int)(Bot::placement[x] * Bot::placement[y] * Bot::piece_scores.at(piece));
}

int evaluate(Table &table, PlaySide sideToMove) {
  // check for checkmate and stalemate
  // need a new parameter, last_move;
  // if(Bot::isCheckMate(table, sideToMove))
  //   return Bot::winScore;

  // if(Bot::isStaleMate(table, sideToMove))
  //   return Bot::drawScore;
  
  // size_t hash = Bot::keyHasher(table) % Bot :: maxLenHT;

  // // search in HT;
  // if(Bot:: tablescoreHT[hash] != INT_MIN) {
  //   a++;
  //   return Bot:: tablescoreHT[hash];
  // }

  int score = 0;
  
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      // process both attacked and protected slots
      if(PieceHandlers::isAttackedBy(table.table[i][j], sideToMove) && PieceHandlers::isAttackedBy(table.table[i][j], get_opponent(sideToMove))) {
          // enemy's king is in check
          if(PieceHandlers::getType(table.table[i][j]) == KING
            && PieceHandlers::getColor(table.table[i][j] != sideToMove))
            score += Bot::kingInCheck;
          // enemy's piece
          else if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] != sideToMove))
            score += Bot::attacker_buf * Bot::protected_attackedPiece * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j);
          // friendly piece
          else if(PieceHandlers::getType(table.table[i][j]) != NAP
            && PieceHandlers::getColor(table.table[i][j] == sideToMove))
            score += Bot::protected_attackedPiece * get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j);
          // empty slot, attacked by both
          else if(PieceHandlers::getType(table.table[i][j]) == NAP)
            score += get_score_by_position(PieceHandlers::getType(table.table[i][j]), i, j);
      
      // procces slots attacked by me
      } if(PieceHandlers::isAttackedBy(table.table[i][j], sideToMove)) {
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

  // // save in HT
  // Bot::tablescoreHT[hash] = score;
  // b++;
  return score;
}

bool game_over(Table &table, PlaySide playside, std::vector<Move> all_moves) {
  return Bot::isStaleMate(table, playside, all_moves) || Bot::isCheckMate(table, playside, all_moves);
}

#define INF (1 << 30)

int evalMove(Table &t, Move &m1, PlaySide myside) {
  int score1 = 0;
  uint8_t dest_piece = t.table[m1.destination_idx->first][m1.destination_idx->second];
  if(m1.isDropIn()) {
    // std::cout << (int)m1.replacement.value() << " dropIN\n";
    // std::cout.flush();
    int basic_piece_score = get_score_by_position(m1.replacement.value(), m1.destination_idx->first, m1.destination_idx->second);
    // std::cout << "dropIN ok\n";
    // std::cout.flush();
    if(PieceHandlers::isAttackedBy(dest_piece, myside) && PieceHandlers::isAttackedBy(dest_piece, get_opponent(myside)))
      score1 = basic_piece_score * Bot::protected_attackedPiece;
    else if(PieceHandlers::isAttackedBy(dest_piece, myside))
      score1 = basic_piece_score * Bot::protectedPiece;
    else if(PieceHandlers::isAttackedBy(dest_piece, get_opponent(myside)))
      score1 = basic_piece_score / 2;
    return score1;
  } else if(m1.isPromotion()) {
    // std::cout << (int)m1.replacement.value() << " promotion\n";
    // std::cout.flush();
    int basic_piece_score = Bot::piece_scores.at(m1.replacement.value());
    // std::cout << "promotionOK\n";
    // std::cout.flush();
    if(PieceHandlers::isAttackedBy(dest_piece, myside) && PieceHandlers::isAttackedBy(dest_piece, get_opponent(myside)))
      score1 = basic_piece_score * Bot::protected_attackedPiece;
    else if(PieceHandlers::isAttackedBy(dest_piece, myside))
      score1 = basic_piece_score * Bot::protectedPiece;
    else if(PieceHandlers::isAttackedBy(dest_piece, get_opponent(myside)))
      score1 = basic_piece_score / 2;
    return score1;
  } else {
    uint8_t sourc_piece = t.table[m1.source_idx->first][m1.source_idx->second];
    // std::cout << (int)sourc_piece << " Normal\n";
    // std::cout.flush();
    int basic_piece_score = get_score_by_position(PieceHandlers::getType(sourc_piece), m1.destination_idx->first, m1.destination_idx->second);
    // std::cout << "NormalOK\n";
    // std::cout.flush();
    if(PieceHandlers::isAttackedBy(dest_piece, myside) && PieceHandlers::isAttackedBy(dest_piece, get_opponent(myside)))
      score1 = basic_piece_score * Bot::protected_attackedPiece;
    else if(PieceHandlers::isAttackedBy(dest_piece, myside))
      score1 = basic_piece_score * Bot::protectedPiece;
    else if(PieceHandlers::isAttackedBy(dest_piece, get_opponent(myside)))
      score1 = basic_piece_score * 2;

    // capture move
    if(PieceHandlers::getType(dest_piece) != NAP)
      score1 += get_score_by_position(PieceHandlers::getType(dest_piece), m1.destination_idx->first, m1.destination_idx->second);
    
    return score1;
  }

  return 0;
}

std::pair<int, Move> Bot::alphabeta_negamax(int depth, PlaySide sideToMove, int alpha, int beta) {
    // STEP 1: game over or maximum recursion depth was reached
    
    if(depth == 0) {
      Move dummy;
      return std::pair(evaluate(table, sideToMove), dummy);
    }

    // malloc_stats();
    // std::cout.flush();
    // STEP 2: generate all possible moves for player
    // Note: sort moves descending by score (if possible) for maximizing the number of cut-off actions
    // (or generete the moves already sorted by a custom criterion)
    std::vector<Move> allMoves;
    table.generateAllPossibleMoves(sideToMove, table.last_move, allMoves);

    if (game_over(table, sideToMove, allMoves)) {
       Move dummy;
       return std::pair(evaluate(table, sideToMove), dummy);
    }

    // std::cout << depth << " ceplm1 " << allMoves.size() << "\n";
    // std::cout.flush();
    // std::sort(allMoves.begin(), allMoves.end(), [&t = table, s = sideToMove](Move &m1, Move &m2) {
    //   return evalMove(t, m1, s) > evalMove(t, m2, s);
    // });

    // std::cout << "ceplm2\n";
    // std::cout.flush();

     
    // STEP 3: try to apply each move - compute best score
    int best_score = -INF;
    Move best_move = allMoves[0];
    Table ogTable = table;
    for (auto move : allMoves) {
        // STEP 3.1: do move
        table = ogTable;
        recordMove(&move, sideToMove);
 
        // STEP 3.2: play for the opponent
        auto [score, _] = alphabeta_negamax(depth - 1, get_opponent(sideToMove), -beta, -alpha);
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
            table = ogTable;
            break;
        }
 
        // STEP 3.4: undo move
        table = ogTable;
    }
 
    // STEP 4: return best allowed score
    // [optional] also return the best move
    return std::pair(best_score, best_move);
}

Move Bot::calculateNextMove(PlaySide sideToMove) {
  auto [_, move] = alphabeta_negamax(4, sideToMove, -INF, INF);
  Move::convertIdxToStr(move);
  recordMove(&move, sideToMove);
  // std::cout << a << " " << b << "\n";
  // std::cout.flush();
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
   if (move->source_idx.has_value() && move->destination_idx.has_value()) {
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
