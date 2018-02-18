#include "Piece.h"

#include "Font.h"
#include "Replacements.h"

#define PIECE_BORDER_SIZE	(4)

static Font* g_pFont = NULL;
static Font* g_pFontError = NULL;

void CreatePiece(struct Piece* pPiece, int x, int y, KenKenLib kenken, struct Metrics* pMetrics, struct Config* pConfig)
{
   pPiece->m_nX = x;
   pPiece->m_nY = y;
   pPiece->m_KenKen = kenken;
   pPiece->m_pMetrics = pMetrics;
   pPiece->m_pConfig = pConfig;

   if( g_pFont == NULL ) {
      g_pFont = LoadFont("arial.ttf", NSDL_FONT_THIN, 0/*R*/, 0/*G*/, 0/*B*/, 12);
   }

   if (g_pFontError == NULL) {
      g_pFontError = LoadFont("arial.ttf", NSDL_FONT_THIN, 255/*R*/, 0/*G*/, 0/*B*/, 12);
   }
}

void FreePiece(struct Piece* pPiece)
{
   pPiece->m_KenKen = NULL;//Does not own
   pPiece->m_pMetrics = NULL;//Does not own
   pPiece->m_pConfig = NULL;//Does not own

   if( g_pFont != NULL ) {
      FreeFont(g_pFont);
      g_pFont = NULL;
   }

   if (g_pFontError != NULL) {
      FreeFont(g_pFontError);
      g_pFontError = NULL;
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

   static char buffer[5];

   if (IsDisplayEquationSpot(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY) == 1)
   {
      int top = GetPieceTextTop(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);
      int left = GetPieceTextLeft(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);

      int nNumber = GetKenKenEquationValue(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY);

      if (nNumber > -1)
      {
         IntToA(buffer, 4, nNumber);

         enum MathOperation eOperation = GetKenKenEquationType(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY);
         if( eOperation == Add )
            StringAppend(buffer, 4, "+");
         else if (eOperation == Subtract)
            StringAppend(buffer, 4, "-");
         if (eOperation == Multiply)
            StringAppend(buffer, 4, "x");
         if (eOperation == Divide)
            StringAppend(buffer, 4, "/");

         Font* pFont = g_pFont;
         int nR = 0, nG = 0, nB = 0;

         DrawText(pScreen, pFont, left, top, buffer, nR, nG, nB);
      }
   }

   int nSpotValue = GetKenKenSpotValue(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY);
   if (nSpotValue > 0)
   {
      IntToA(buffer, 4, nSpotValue);

      int top = GetPieceValueTextTop(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);
      int left = GetPieceValueTextLeft(pPiece->m_pMetrics, pPiece->m_nX, pPiece->m_nY);

      Font* pFont = g_pFont;
      int nR = 0, nG = 0, nB = 0;
      if (IsKenKenUniqueValueForRowColumn(pPiece->m_KenKen, pPiece->m_nX, pPiece->m_nY) == KENKENLIB_NOT_UNIQUE_VALUE)
      {
         pFont = g_pFontError;
         nR = 255;
      }

      DrawText(pScreen, pFont, left, top, buffer, nR, nG, nB);
   }
}

 
