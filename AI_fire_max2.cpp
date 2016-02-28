/************************************************************************/
/*                                                                      */
/*      とこぷよAI                                                      */
/*      2手先まで読み、発火可能な最大のものを選ぶ                       */
/*                                                                      */
/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
using namespace Puyo;
using namespace std;


// 全てのパターンを探索する
// 発火出来る最大点数のものを選ぶ
static Plan plan_first; // 初手の置き方メモ
static int max_score;   // 最大点
static Plan plan_best;  // 選ばれた置き方
static Field field_best; 

static void search( Field &field, Next &next, int level, int lim ){
	const Tumo tumo = next.get(level);
	const int loop = (tumo.first==tumo.second) ? 11 : 22; // 同色なら半分
	for( int i=0; i<loop; ++i ){
		Field new_field = field;
		Plan  plan = getPlan(i);
		if( setTumo( new_field, tumo, plan )==-1 ) continue;
		if( level==0 ) plan_first = plan; // 初手をメモる
		if( canFire(new_field) ){
			Field fmemo = new_field;
			InfoFire info;
			fireField( new_field, info );
			if( max_score<info.score ){ // 得点が更新された
				max_score = info.score;
				plan_best = plan_first;
				field_best = fmemo;
			}
		}
		if( new_field.get(3,12)!=BRANK ) continue;
		if( level<lim ) search( new_field, next, level+1, lim );
	}	
}

Plan AI_fire_max2( Field field, Next next ){
	max_score = -1;
	plan_best = (-1,-1);
	search( field, next, 0, 2 );
	if( max_score==-1 ){
		cout<<"発火不可"<<endl;
	} else {
		printField( field_best );
		cout<<max_score<<endl;
	}
	return plan_best;
}
