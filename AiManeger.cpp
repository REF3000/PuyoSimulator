/************************************************************************/
/* AI追加方法                                                           */
/* <変数を変更>                                                         */
/*  1. AI_NUM   : AIの総数に設定                                        */
/*  2. AI_NAMES : AIの名前を設定                                        */
/* <関数を変更>                                                         */
/*   executeAi(...) にて                                                */
/*  3. externで外部の関数を宣言                                         */
/*  4. switchのcaseを追加（他のを参考に）                               */
/************************************************************************/
#include "AiManeger.h"
using namespace Puyo;
/*----------------------------------------------------------------------*/
/*     1.  AI_NUM                                                       */
/*----------------------------------------------------------------------*/
static int  AI_NUM = 9;
/*----------------------------------------------------------------------*/
/*     2.  AI_NAMES                                                     */
/*----------------------------------------------------------------------*/
static char AI_NAMES[][64]={
	"AItest",
	"最大点発火（現在手）",
	"最大点発火（2手先）",
	"ランダムに選ぶだけ",
	"シンプル",
	"test2",
	"debug",
	"pm",
};
/*----------------------------------------------------------------------*/
/*     3.  extern宣言                                                   */
/*----------------------------------------------------------------------*/
extern Plan AI_fire_max( Field field, Next next );
extern Plan AI_fire_max2( Field field, Next next );
extern Plan AI_random( Field field, Next next );
extern Plan AI_test( Field field, Next next );
extern Plan AI_test2( Field field, Next next );
extern Plan AI_simple( Field field, Next next );
extern Plan AI_debug( Field field, Next next );
extern Plan AI_pattern_match( Field field, Next next );
/*----------------------------------------------------------------------*/
/*     4.  executeAi()                                                  */
/*----------------------------------------------------------------------*/
Plan executeAi( int id, Field field, Next next ){
	switch( id ){
	case 0: return AI_test( field, next );
	case 1: return AI_fire_max( field, next );
	case 2: return AI_fire_max2( field, next );
	case 3: return AI_random( field, next );
	case 4: return AI_simple( field, next );
	case 5: return AI_test2( field, next );
	case 6: return AI_debug( field, next );
	case 7: return AI_pattern_match( field, next );
	}
	return Plan( -1,-1 );
}








//----------------------------------------
int getAiNum(){
	return AI_NUM;
}
char *getAiName( int id ){
	if( AI_NUM<=id ) return "-";
	return AI_NAMES[id];
}



