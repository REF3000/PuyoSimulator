/************************************************************************/
/*                                                                      */
/*      とこぷよAI                                                      */
/*      現在手で発火可能な最大のものを選ぶ                              */
/*                                                                      */
/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
using namespace Puyo;
using namespace std;

Plan AI_fire_max( Field field, Next next ){
	int  max_score = -1;      // 最大点
	Plan best_plan = (-1,-1); // 最大点の時の置き方

	Tumo tumo = next.get(0);
	for( int i=0; i<22; ++i ){
		Field fc   = field;
		Plan  plan = getPlan(i);
		setTumo( fc, tumo, plan );
		if( canFire(fc) ){
			Field memo = fc; // printField用のメモ
			InfoFire info;
			fireField( fc, info );
			if( max_score<info.score ){ // 得点が更新された
				printField( memo );
				cout<<"score:"<<info.score<<endl;
				max_score = info.score;
				best_plan = plan;
			}
		}
	}

	if( max_score==-1 ) cout<<"発火不可"<<endl;
	return best_plan;
}
