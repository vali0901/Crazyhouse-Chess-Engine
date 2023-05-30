#include "Move.h"

#include <bits/stdc++.h>

#include "Piece.h"
Move::Move() {
  
}

Move::Move(std::optional<std::string> _source,
           std::optional<std::string> _destination,
           std::optional<Piece> _replacement)
    : source_str(_source), destination_str(_destination), replacement(_replacement) {}

Move::Move(std::optional<std::pair<int8_t, int8_t>> _source,
           std::optional<std::pair<int8_t, int8_t>> _destination,
           std::optional<Piece> _replacement)
    : source_idx(_source), destination_idx(_destination), replacement(_replacement) {}

std::optional<std::string> Move::getSource() { return source_str; }

std::optional<std::string> Move::getDestination() { return destination_str; }

std::optional<Piece> Move::getReplacement() { return replacement; }

void Move::convertStrToIdx(Move &move) {
  if (move.destination_str.has_value())
    move.destination_idx = {'8' - move.destination_str.value()[1], move.destination_str.value()[0] - 'a'};

  if (move.source_str.has_value())
    move.source_idx = std::pair('8' - move.source_str.value()[1], move.source_str.value()[0] - 'a');
}

void Move::convertIdxToStr(Move &move) {
  if (move.destination_idx.has_value())
  {
    move.destination_str = "";
    move.destination_str->push_back(move.destination_idx->second + 'a');
    move.destination_str->push_back('8' - move.destination_idx->first);
  }

  if (move.source_idx.has_value())
  {
    move.source_str = "";
    move.source_str->push_back(move.source_idx->second + 'a');
    move.source_str->push_back('8' - move.source_idx->first); 
  }
    
}

bool Move::isNormal()
{
  return (this->source_str.has_value() && this->destination_str.has_value() && !this->replacement.has_value()) ||
          (this->source_idx.has_value() && this->destination_idx.has_value() && !this->replacement.has_value());
}

bool Move::isPromotion()
{
  return (this->source_str.has_value() && this->destination_str.has_value() && this->replacement.has_value()) ||
          (this->source_idx.has_value() && this->destination_idx.has_value() && this->replacement.has_value());
}

bool Move::isDropIn()
{
  return (!this->source_str.has_value() && this->destination_str.has_value() && this->replacement.has_value()) ||
          (!this->source_idx.has_value() && this->destination_idx.has_value() && this->replacement.has_value());
}

Move Move::moveTo(std::optional<std::string> source,
                   std::optional<std::string> destination)
{
  return Move(source, destination, {});
}

Move Move::moveTo(std::optional<std::pair<int8_t, int8_t>> source,
                   std::optional<std::pair<int8_t, int8_t>> destination) {
  return Move(source, destination, {});
}

Move Move::promote(std::optional<std::string> source,
                    std::optional<std::string> destination,
                    std::optional<Piece> replacement)
{
  return Move(source, destination, replacement);
}

Move Move::promote(std::optional<std::pair<int8_t, int8_t>> source,
                    std::optional<std::pair<int8_t, int8_t>> destination,
                    std::optional<Piece> replacement)
{
  return Move(source, destination, replacement);
}

Move Move::dropIn(std::optional<std::string> destination,
                   std::optional<Piece> replacement)
{
  return Move({}, destination, replacement);
}

Move Move::dropIn(std::optional<std::pair<int8_t, int8_t>> destination,
                   std::optional<Piece> replacement)
{
  return Move({}, destination, replacement);
}

Move Move::resign() { return Move("", "", {}); }
