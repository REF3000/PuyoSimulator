#pragma once
#include "PuyoAI.h"

int   getAiNum();
char *getAiName( int id );
// ���s���Č��ʂ�Ԃ�
Plan executeAi( int id, Puyo::Field field, Puyo::Next next );
