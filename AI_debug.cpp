/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
using namespace Puyo;
using namespace std;

// フィールドを評価する（発火も行う）
// 評価項目
// 1.発火点探索　とりあえず1個置いて発火出来るか？
// 2.形の評価    U字型が強いらしい
// 3.連結の評価  いっぱい連結しよう
static int eval_field( Field &field, Next &next ){
	// フィールド高さは何かと使うので調べておく
	int height[8];
	for( int i=1; i<=6; ++i ) height[i] = countHeight( field, i );

	// 1.発火点探索
	// とりあえず1個置いてみる
	// 評価点：連鎖数重視 連鎖後半にたくさん消えるように 残しの連結も見る
	int fire_score = 0;
	for( int x=1; x<=6; ++x ){
	const int Y = height[x]+1;
	if( Y>12 ) continue;
	for( int i=1; i<=4; ++i ){
		field.set( x, Y, (eKIND)i );         // 1個置いてみて
		if( countConnection(field,x,Y)>=4 ){ // 4連結以上したら発火する
			 printField( field );
			Field    new_field = field;
			InfoFire info;
			fireField( new_field, info );
			// 基本点 連鎖数*100
			int score = info.chain_num * 100;
			// (n連鎖目に消えた数)*n を加算
			for( int i=0; i<info.chain_num; ++i )
				score += info.step_infos[i].num * (i+1);
			// 残しの連結数を加算
			for( int x=1; x<=6; ++x ){
				int h = countHeight( new_field, x );
				for( int y=1; y<=h; ++y ){
					score += countConnection( new_field, x, y );
				}
			}
			fire_score = max( fire_score, score ); // 過去最高ならメモ
			 cout<<score<<endl;
		}
		field.set( x, height[x]+1, BRANK ); // 置いたぷよは消す
	}}

	// フィールド形の解析
	int max_height = 0;
	int min_height = 14;
	for( int x=1; x<=6; ++x ){
		max_height = max( max_height, height[x] ); 
		min_height = min( min_height, height[x] );
	}
	int shape_score = 10 - (max_height-min_height);


	return fire_score;
}

// 全てのパターンを探索する
static Plan plan_first; // 初手の置き方メモ
static int max_score;   // 最高得点メモ
static Plan plan_best;  // 選ばれし置き方

static void search( Field &field, Next &next, int level, int lim ){
	const Tumo TUMO = next.get(level);
	const int  LOOP = (TUMO.first==TUMO.second) ? 11 : 22; // 同色なら半分
	for( int i=0; i<LOOP; ++i ){
		// 1手置いた新しいフィールドの作成
		Field      new_field = field;
		const Plan PLAN      = getPlan(i);
		if( setTumo( new_field, TUMO, PLAN ) == -1 ) continue;
		// 初手をメモる
		if( level==0 ) plan_first = PLAN;
		// 発火する
		InfoFire info;
		fireField( new_field, info );
		// 窒息確認
		if( new_field.get(3,12)!=BRANK ) continue;
		// フィールドを評価して過去最高ならメモ
		int score = eval_field( new_field, next ); // フィールド形の評価
		if( score>max_score ){
			max_score = score;
			plan_best = plan_first;
		}
		// さらなる深みへ
		if( level<lim ) search( new_field, next, level+1, lim );
	}	
}

Plan AI_debug( Field field, Next next ){
	max_score = -1;
	plan_best = (-1,-1);
	eval_field( field, next );
	//search( field, next, 0, 2 );
	if( max_score==-1 ) cout<<"発火不可"<<endl;
	return plan_best;
}
