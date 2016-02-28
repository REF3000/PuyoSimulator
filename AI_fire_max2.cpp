/************************************************************************/
/*                                                                      */
/*      �Ƃ��Ղ�AI                                                      */
/*      2���܂œǂ݁A���Ή\�ȍő�̂��̂�I��                       */
/*                                                                      */
/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
using namespace Puyo;
using namespace std;


// �S�Ẵp�^�[����T������
// ���Ώo����ő�_���̂��̂�I��
static Plan plan_first; // ����̒u��������
static int max_score;   // �ő�_
static Plan plan_best;  // �I�΂ꂽ�u����
static Field field_best; 

static void search( Field &field, Next &next, int level, int lim ){
	const Tumo tumo = next.get(level);
	const int loop = (tumo.first==tumo.second) ? 11 : 22; // ���F�Ȃ甼��
	for( int i=0; i<loop; ++i ){
		Field new_field = field;
		Plan  plan = getPlan(i);
		if( setTumo( new_field, tumo, plan )==-1 ) continue;
		if( level==0 ) plan_first = plan; // �����������
		if( canFire(new_field) ){
			Field fmemo = new_field;
			InfoFire info;
			fireField( new_field, info );
			if( max_score<info.score ){ // ���_���X�V���ꂽ
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
		cout<<"���Εs��"<<endl;
	} else {
		printField( field_best );
		cout<<max_score<<endl;
	}
	return plan_best;
}
