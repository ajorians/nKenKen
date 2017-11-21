#ifndef GAME_H
#define GAME_H

#include <KenKenLib/KenKenLib.h>
#include <SDL/SDL.h>
#include "Background.h"
#include "Config.h"
#include "Metrics.h"
#include "Piece.h"
#include "Selector.h"
#include "EquationHinter.h"

struct Game
{
   KenKenLib m_KenKen;
   int m_nLevelNum;
   struct Config* m_pConfig;//Does not own
   int m_bWon;
   struct Metrics* m_pMetrics;
   struct Piece* m_apPieces;
   struct Selector* m_pSelector;
   struct EquationHinter* m_pEquationHinter;
   int m_bShouldQuit;
   struct SDL_Surface* m_pYouWinGraphic;
   struct SDL_Surface* m_pScreen;//Does not own
   struct Background* m_pBackground;
};

void CreateGame(struct Game** ppGame, const char* pstrLevelData, int nLevelNum, struct Config* pConfig, struct SDL_Surface* pScreen);
void FreeGame(struct Game** ppGame);
int GameLoop(struct Game* pGame);
int GameShouldQuit(struct Game* pMenu);

#endif
