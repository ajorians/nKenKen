#ifndef BACKGROUND_H
#define BACKGROUND_H

#ifdef _TINSPIRE
#include <os.h>
#endif
#include "SDL/SDL.h"
#include "Config.h"

#define NUM_SQUARES (35)

struct Square {
   int x;
   int y;
   int size;
   int r;
   int g;
   int b;
   int direction;
   int move;
};

struct Background
{
   struct SDL_Surface* m_pScreen;//Does not own
   struct Config*      m_pConfig;//Does not own
   struct Square m_ArrSquares[NUM_SQUARES];
};

void CreateBackground(struct Background** ppBackground, struct SDL_Surface* pScreen, struct Config* pConfig, int nAllowMovement);
void FreeBackground(struct Background** ppBackground);
void DrawBackground(struct Background* pBackground);

#endif

