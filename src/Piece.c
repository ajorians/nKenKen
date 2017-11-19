#include "Piece.h"

#include "Font.h"

#define PIECE_BORDER_SIZE	(4)

#define USE_DIFFERENT_FONT

#ifdef USE_DIFFERENT_FONT
static Font* g_pFontBig = NULL;
static Font* g_pFontUsed = NULL;
#endif
static Font* g_pFont = NULL;

void CreatePiece(struct Piece* pPiece, int x, int y, KenKenLib kenken, struct Metrics* pMetrics, struct Config* pConfig)
{
   pPiece->m_nX = x;
   pPiece->m_nY = y;
   pPiece->m_KenKen = kenken;
   pPiece->m_pMetrics = pMetrics;
   pPiece->m_pConfig = pConfig;

   if( g_pFont == NULL ) {
#ifdef USE_DIFFERENT_FONT
      g_pFontBig = LoadFont("ARIAL.TTF", NSDL_FONT_THIN, 255/*R*/, 255/*G*/, 255/*B*/, 12);
      g_pFontUsed = LoadFont("ARIAL.TTF", NSDL_FONT_THIN, 255/*R*/, 0/*G*/, 0/*B*/, 12);
#endif
      g_pFont = LoadFont("ARIAL.TTF", NSDL_FONT_THIN, 127/*R*/, 127/*G*/, 127/*B*/, 12);
   }
}

void FreePiece(struct Piece* pPiece)
{
   pPiece->m_KenKen = NULL;//Does not own
   pPiece->m_pMetrics = NULL;//Does not own
   pPiece->m_pConfig = NULL;//Does not own

   if( g_pFont != NULL ) {
#ifdef USE_DIFFERENT_FONT
      FreeFont(g_pFontBig);
      g_pFontBig = NULL;
      FreeFont(g_pFontUsed);
      g_pFontUsed = NULL;
#endif

      FreeFont(g_pFont);
      g_pFont = NULL;
   }
}

int IsDisplayEquationSpot(KenKenLib kenken, int x, int y)
{
   if (KenKenSpotShareSameEquation(kenken, x, y, x - 1, y) == KENKENLIB_SHARE_EQUATION)
      return 0;
   if (KenKenSpotShareSameEquation(kenken, x, y, x, y-1) == KENKENLIB_SHARE_EQUATION)
      return 0;

   //Odd but possible
   if (KenKenSpotShareSameEquation(kenken, x, y, x+1, y - 1) == KENKENLIB_SHARE_EQUATION)
      return 0;

   return 1;
}

void PieceDraw(struct Piece* pPiece, struct SDL_Surface* pScreen)
{
   SDL_Rect rect;
   rect.w = GetPieceWidth(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);
   rect.h = GetPieceHeight(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);
   rect.x = GetPieceLeft(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);
   rect.y = GetPieceTop(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);

   int r = 255, g = 255, b = 255;

   SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, 0, 0, 0));

   int leftTakeOff = 1, rightTakeOff = 0, topTakeOff = 1, bottomTakeOff = 0;
   if (pPiece->m_nX == 0)
      leftTakeOff = 3;
   if (pPiece->m_nY == 0)
      topTakeOff = 3;
   if (pPiece->m_nX == GetKenKenWidth(pPiece->m_KenKen)-1 )
      rightTakeOff = 3;
   if (pPiece->m_nY == GetKenKenHeight(pPiece->m_KenKen)-1 )
      bottomTakeOff = 3;

   if (KenKenSpotShareSameEquation(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY, pPiece->m_nX-1, pPiece->m_nY) == KENKENLIB_NOT_SHARE_EQUATION )
      leftTakeOff += 2;
   if (KenKenSpotShareSameEquation(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY, pPiece->m_nX, pPiece->m_nY-1) == KENKENLIB_NOT_SHARE_EQUATION)
      topTakeOff += 2;
   if (KenKenSpotShareSameEquation(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY, pPiece->m_nX + 1, pPiece->m_nY) == KENKENLIB_NOT_SHARE_EQUATION)
      rightTakeOff += 2;
   if (KenKenSpotShareSameEquation(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY, pPiece->m_nX, pPiece->m_nY + 1) == KENKENLIB_NOT_SHARE_EQUATION)
      bottomTakeOff += 2;
   rect.w -= (leftTakeOff + rightTakeOff);
   rect.h -= (topTakeOff + bottomTakeOff);
   rect.x += leftTakeOff;
   rect.y += topTakeOff;

   SDL_FillRect(pScreen, &rect, SDL_MapRGB(pScreen->format, r, g, b));

   if (IsDisplayEquationSpot(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY) == 1)
   {
      static char buffer[2];
      buffer[1] = '\0';

      int top = GetPieceTextTop(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);
      int left = GetPieceTextLeft(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);

      int nNumber = GetKenKenEquationValue(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY);

      if (nNumber > -1)
      {
         buffer[0] = nNumber + '0';

         Font* pFont = g_pFont;
         int nR = 0, nG = 0, nB = 0;
         //#ifdef USE_DIFFERENT_FONT
         //#ifdef _TINSPIRE
         //          && GetLockHint(pPiece->m_pConfig) == 1 
         //#endif
         //         ) {
         //         pFont = g_pFontUsed;
         //         nR = 255, nG = 0, nB = 0;
         //      }
         //      else if( i == 0 ) {
         //         pFont = g_pFontBig;
         //         nR = 255, nG = 255, n = 255;
         //      }
         //#endif

         DrawText(pScreen, pFont, left, top, buffer, nR, nG, nB);
      }
   }
}

 
