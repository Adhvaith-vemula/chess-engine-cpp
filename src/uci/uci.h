#pragma once

#include "board/board.h"
#include <string>

std::string moveToStr(Move m);
Move parseMove(const Board& b, const std::string& s);
void uciLoop();
