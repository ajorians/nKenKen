#include "Background.h"
#include "Replacements.h"

#define MOVE_SPEED	(1)

void CreateBackground(struct Background** ppBackground, struct SDL_Surface* pScreen, struct Config* pConfig, int nAllowMovement) {
   *ppBackground = malloc(sizeof(struct Background));
   struct Background* pBackground = *ppBackground;
   pBackground->m_pScreen = pScreen;
   pBackground->m_pConfig = pConfig;

   for(int i=0; i<NUM_SQUARES; i++) {
      struct Square* pSquare = &pBackground->m_ArrSquares[i];
      pSquare->x = rand() % SCREEN_WIDTH;
      pSquare->y = rand() % SCREEN_HEIGHT;
      int nSize = rand() %35 + 10;
      pSquare->size = nSize;
      pSquare->r = rand() % 255;
      pSquare->g = rand() % 255;
      pSquare->b = rand() % 255;
      pSquare->direction = rand() % 2;
      pSquare->move = MOVE_SPEED;
   }
}

void FreeBackground(struct Background** ppBackground) {
  struct Background* pBackground = *ppBackground;
  pBackground->m_pScreen = NULL;//Does not own
  pBackground->m_pConfig = NULL;//Does not own

  free(pBackground);
}

void DrawBackground(struct Background* pBackground) {
   if( GetDrawBackground(pBackground->m_pConfig) == 0 )
	   return;

   for(int i=0; i<NUM_SQUARES; i++) {
      struct Square* pSquare = &pBackground->m_ArrSquares[i];
      SDL_Rect r;
      r.x = pSquare->x;
      r.y = pSquare->y;
      r.w = pSquare->size;
      r.h = pSquare->size;
      SDL_FillRect(pBackground->m_pScreen, &r, SDL_MapRGB(pBackground->m_pScreen->format, pSquare->r, pSquare->g, pSquare->b));
      pSquare->move--;
      if( pSquare->move <= 0 ) {
	      if( pSquare->direction == 0 ) {
		      pSquare->x++;
	      }
	      else {
		      pSquare->y++;
	      }

	      pSquare->move = MOVE_SPEED;
      }

      if( pSquare->x > SCREEN_WIDTH ) {
	      pSquare->x = -pSquare->size;
      }

      if( pSquare->y > SCREEN_HEIGHT ) {
	      pSquare->y = -pSquare->size;
      }
   }
}

 
