#ifndef PIECE_H
#define PIECE_H

#ifdef _TINSPIRE
#include <os.h>
#endif
#include "SDL/SDL.h"
#include <KenKenLib/KenKenLib.h>
#include "Metrics.h"
#include "Config.h"

struct Piece
{
   int m_nX;
   int m_nY;
   KenKenLib m_KenKen;//Does not own
   struct Metrics* m_pMetrics;//Does not own
   struct Config* m_pConfig;//Does not own
};

void CreatePiece(struct Piece* pPiece, int x, int y, KenKenLib kenken, struct Metrics* pMetrics, struct Config* pConfig);
void FreePiece(struct Piece* pPiece);
void PieceDraw(struct Piece* pPiece, struct SDL_Surface* pScreen);

#endif

