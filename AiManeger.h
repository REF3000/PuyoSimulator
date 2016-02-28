#pragma once
#include "PuyoAI.h"

int   getAiNum();
char *getAiName( int id );
// Às‚µ‚ÄŒ‹‰Ê‚ğ•Ô‚·
Plan executeAi( int id, Puyo::Field field, Puyo::Next next );
