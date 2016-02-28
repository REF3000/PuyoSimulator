/************************************************************************/
/*                                                                      */
/*      とこぷよAI                                                      */
/*      ランダムに選ぶだけ                                              */
/*                                                                      */
/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
#include <random>
using namespace Puyo;
using namespace std;

// ランダムに返すだけのAI（AIか?）
Plan AI_random( Field field, Next next ){
	random_device rnd;
	int pos, dir;
	while( 1 ){
		pos = rnd()%6 + 1;
		dir = rnd()%4;
		if( pos==1 && dir==3 ) continue;
		if( pos==5 && dir==1 ) continue;
		break;
	}
	return Plan( pos, dir );
}