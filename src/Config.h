#ifndef CONFIG_H
#define CONFIG_H

#ifdef _TINSPIRE
#include "ArchiveLib/ArchiveLib.h"
#endif

struct Config
{
#ifdef _TINSPIRE
   ArchiveLib m_Archive;
#endif
   int m_nBeatLevels[250];
   int m_nDrawBackground;
   int m_nEquationHint;
};

void CreateConfig(struct Config** ppConfig);
void FreeConfig(struct Config** ppConfig);
void SetBeatLevel(struct Config* pConfig, int nLevelNum, int nBeat);
int GetBeatLevel(struct Config* pConfig, int nLevelNum);
int GetDrawBackground(struct Config* pConfig);
void SetDrawBackground(struct Config* pConfig, int nOn);

int GetEquationHint(struct Config* pConfig);
void SetEquationHint(struct Config* pConfig, int nOn);

#endif
