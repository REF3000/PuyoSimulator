/************************************************************************/
/*                                                                      */
/*      �Ƃ��Ղ�AI                                                      */
/*      ���ݎ�Ŕ��Ή\�ȍő�̂��̂�I��                              */
/*                                                                      */
/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
using namespace Puyo;
using namespace std;

Plan AI_fire_max( Field field, Next next ){
	int  max_score = -1;      // �ő�_
	Plan best_plan = (-1,-1); // �ő�_�̎��̒u����

	Tumo tumo = next.get(0);
	for( int i=0; i<22; ++i ){
		Field fc   = field;
		Plan  plan = getPlan(i);
		setTumo( fc, tumo, plan );
		if( canFire(fc) ){
			Field memo = fc; // printField�p�̃���
			InfoFire info;
			fireField( fc, info );
			if( max_score<info.score ){ // ���_���X�V���ꂽ
				printField( memo );
				cout<<"score:"<<info.score<<endl;
				max_score = info.score;
				best_plan = plan;
			}
		}
	}

	if( max_score==-1 ) cout<<"���Εs��"<<endl;
	return best_plan;
}
