#pragma once
#include "PuyoAI.h"

int   getAiNum();
char *getAiName( int id );
// 実行して結果を返す
Plan executeAi( int id, Puyo::Field field, Puyo::Next next );
