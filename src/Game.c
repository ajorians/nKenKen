#ifdef _TINSPIRE
#include <os.h>
#include <libndls.h>
#endif
#include "Game.h"
#include "Replacements.h"
#include "YouWinGraphic.h"

void CreateGame(struct Game** ppGame, const char* pstrLevelData, int nLevelNum, struct Config* pConfig, struct SDL_Surface* pScreen)
{
   *ppGame = malloc(sizeof(struct Game));
   struct Game* pGame = *ppGame;
   KenKenLibCreate(&(pGame->m_KenKen), pstrLevelData);
   pGame->m_nLevelNum = nLevelNum;
   pGame->m_pConfig = pConfig;
   pGame->m_bWon = IsKenKenGameOver(pGame->m_KenKen);

#ifdef _TINSPIRE
   pGame->m_pYouWinGraphic = nSDL_LoadImage(image_YouWin);
   SDL_SetColorKey(pGame->m_pYouWinGraphic, SDL_SRCCOLORKEY, SDL_MapRGB(pGame->m_pYouWinGraphic->format, 255, 255, 255));
#endif

   pGame->m_pScreen = pScreen;
   CreateBackground(&(pGame->m_pBackground), pGame->m_pScreen, pGame->m_pConfig, 1);
   pGame->m_pMetrics = NULL;
   CreateMetrics(&pGame->m_pMetrics, pGame->m_KenKen);

   int nWidth = GetKenKenWidth(pGame->m_KenKen);
   int nHeight = GetKenKenHeight(pGame->m_KenKen);
   int nNumPtrs = nWidth * nHeight;
   pGame->m_apPieces = malloc(nNumPtrs*sizeof(struct Piece));
   for(int x=0; x<nWidth; x++) {
      for(int y=0; y<nHeight; y++) {
         struct Piece* pPiece = &pGame->m_apPieces[x+y*nWidth];
         CreatePiece(pPiece, x, y, pGame->m_KenKen, pGame->m_pMetrics, pConfig);
      }
   }

   pGame->m_pSelector = NULL;
   CreateSelector(&pGame->m_pSelector, pGame->m_pScreen, pGame->m_pConfig, pGame->m_pMetrics, pGame->m_KenKen);

   pGame->m_pEquationHinter = NULL;
   CreateHinter(&pGame->m_pEquationHinter, pGame->m_pScreen, pGame->m_pConfig, pGame->m_KenKen);
   StartEquationHinting(pGame->m_pEquationHinter, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector), 1);

   pGame->m_bShouldQuit = 0;
}

void FreeGame(struct Game** ppGame)
{
   struct Game* pGame = *ppGame;

   int nWidth = GetKenKenWidth(pGame->m_KenKen);
   int nHeight = GetKenKenHeight(pGame->m_KenKen);
   for(int x=0; x<nWidth; x++) {
      for(int y=0; y<nHeight; y++) {
         struct Piece* pPiece = &pGame->m_apPieces[x+y*nWidth];
         FreePiece(pPiece);
      }
   }
   free(pGame->m_apPieces);

#ifdef _TINSPIRE
   SDL_FreeSurface(pGame->m_pYouWinGraphic);
#endif
   FreeSelector(&pGame->m_pSelector);
   FreeHinter(&pGame->m_pEquationHinter);
   FreeBackground(&pGame->m_pBackground);
   FreeMetrics(&pGame->m_pMetrics);

   pGame->m_pConfig = NULL;//Does not own
   pGame->m_pScreen = NULL;//Does not own

   free(pGame);
   *ppGame = NULL;
}

void DrawBoard(struct Game* pGame)
{
   DrawBackground(pGame->m_pBackground);
#if 1
   int nWidth = GetKenKenWidth(pGame->m_KenKen);
   int nHeight = GetKenKenHeight(pGame->m_KenKen);

   //Draw pieces
   for(int x=0; x<nWidth; x++) {
      for(int y=0; y<nHeight; y++) {
         struct Piece* pPiece = &pGame->m_apPieces[x+y*nWidth];
         PieceDraw(pPiece, pGame->m_pScreen);
      }
   }

   //Draw selector
   DrawSelector(pGame->m_pSelector);

   //Draw hinter
   DrawHinter(pGame->m_pEquationHinter);

   if( pGame->m_bWon == 1 ) {
      SDL_Rect rectYouWin;
      rectYouWin.x = (SCREEN_WIDTH - pGame->m_pYouWinGraphic->w)/2;
      rectYouWin.y = (SCREEN_HEIGHT - pGame->m_pYouWinGraphic->h)/2;
      rectYouWin.w = pGame->m_pYouWinGraphic->w;
      rectYouWin.h = pGame->m_pYouWinGraphic->h;
      SDL_BlitSurface(pGame->m_pYouWinGraphic, NULL, pGame->m_pScreen, &rectYouWin);
   }
#endif
   
   SDL_UpdateRect(pGame->m_pScreen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void UpdateGameWon(struct Game* pGame)
{
   if( pGame->m_bWon && pGame->m_nLevelNum > 0 && pGame->m_nLevelNum <= 56 ) {
#ifdef _TINSPIRE
      SetBeatLevel(pGame->m_pConfig, pGame->m_nLevelNum-1/*To 0-base*/, 1);
#endif
   }
}

int GamePollEvents(struct Game* pGame)
{
   SDL_Event event;
   while( SDL_PollEvent( &event ) ) {
      switch( event.type ) {
         case SDL_KEYDOWN:
            switch( event.key.keysym.sym) {
              case SDLK_ESCAPE:
                  return 0;
                  break;

               case SDLK_UP:
		  if( pGame->m_bWon != 1 ) {
                     Move(pGame->m_pSelector, Up);
                     StartEquationHinting(pGame->m_pEquationHinter, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector), 0);
		  }
                  break;

	       case SDLK_DOWN:
		  if( pGame->m_bWon != 1 ) {
                     Move(pGame->m_pSelector, Down);
                     StartEquationHinting(pGame->m_pEquationHinter, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector), 0);
		  }
                  break;

               case SDLK_LEFT:
		  if( pGame->m_bWon != 1 ) {
                     Move(pGame->m_pSelector, Left);
                     StartEquationHinting(pGame->m_pEquationHinter, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector), 0);
		  }
                  break;

               case SDLK_RIGHT:
		  if( pGame->m_bWon != 1 ) {
                     Move(pGame->m_pSelector, Right);
                     StartEquationHinting(pGame->m_pEquationHinter, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector), 0);
		  }
                  break;

               case SDLK_RETURN:
               case SDLK_LCTRL:
               case SDLK_RCTRL:
		  if( pGame->m_bWon != 1 ) {
                     //ToggleCrossCellValue(pGame->m_Cross, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector));

		     pGame->m_bWon = IsKenKenGameOver(pGame->m_KenKen);
		     UpdateGameWon(pGame);
		  }
                  break;

               case SDLK_0:
               case SDLK_1:
               case SDLK_2:
               case SDLK_3:
               case SDLK_4:
               case SDLK_5:
               case SDLK_6:
               case SDLK_7:
               case SDLK_8:
               case SDLK_9:
                  if (pGame->m_bWon != 1) {
                     SetKenKenSpotValue(pGame->m_KenKen, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector), event.key.keysym.sym - SDLK_0);
                     StartEquationHinting(pGame->m_pEquationHinter, GetCurrentX(pGame->m_pSelector), GetCurrentY(pGame->m_pSelector), 1);
                     pGame->m_bWon = IsKenKenGameOver(pGame->m_KenKen);
                     UpdateGameWon(pGame);
                  }
                  break;

               default:
                  break;
            }
          default:
             break;
      }
   }
   return 1;
}

int GameLoop(struct Game* pGame)
{
   if( GamePollEvents(pGame) == 0 )
      return 0;

   DrawBoard(pGame);

   ProcessHinter(pGame->m_pEquationHinter);

   SDL_Delay(30);

   return 1;
}

int GameShouldQuit(struct Game* pGame)
{
   return pGame->m_bShouldQuit;
}

