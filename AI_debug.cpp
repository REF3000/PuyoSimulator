/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
using namespace Puyo;
using namespace std;

// �t�B�[���h��]������i���΂��s���j
// �]������
// 1.���Γ_�T���@�Ƃ肠����1�u���Ĕ��Ώo���邩�H
// 2.�`�̕]��    U���^�������炵��
// 3.�A���̕]��  �����ς��A�����悤
static int eval_field( Field &field, Next &next ){
	// �t�B�[���h�����͉����Ǝg���̂Œ��ׂĂ���
	int height[8];
	for( int i=1; i<=6; ++i ) height[i] = countHeight( field, i );

	// 1.���Γ_�T��
	// �Ƃ肠����1�u���Ă݂�
	// �]���_�F�A�����d�� �A���㔼�ɂ������������悤�� �c���̘A��������
	int fire_score = 0;
	for( int x=1; x<=6; ++x ){
	const int Y = height[x]+1;
	if( Y>12 ) continue;
	for( int i=1; i<=4; ++i ){
		field.set( x, Y, (eKIND)i );         // 1�u���Ă݂�
		if( countConnection(field,x,Y)>=4 ){ // 4�A���ȏサ���甭�΂���
			 printField( field );
			Field    new_field = field;
			InfoFire info;
			fireField( new_field, info );
			// ��{�_ �A����*100
			int score = info.chain_num * 100;
			// (n�A���ڂɏ�������)*n �����Z
			for( int i=0; i<info.chain_num; ++i )
				score += info.step_infos[i].num * (i+1);
			// �c���̘A���������Z
			for( int x=1; x<=6; ++x ){
				int h = countHeight( new_field, x );
				for( int y=1; y<=h; ++y ){
					score += countConnection( new_field, x, y );
				}
			}
			fire_score = max( fire_score, score ); // �ߋ��ō��Ȃ烁��
			 cout<<score<<endl;
		}
		field.set( x, height[x]+1, BRANK ); // �u�����Ղ�͏���
	}}

	// �t�B�[���h�`�̉��
	int max_height = 0;
	int min_height = 14;
	for( int x=1; x<=6; ++x ){
		max_height = max( max_height, height[x] ); 
		min_height = min( min_height, height[x] );
	}
	int shape_score = 10 - (max_height-min_height);


	return fire_score;
}

// �S�Ẵp�^�[����T������
static Plan plan_first; // ����̒u��������
static int max_score;   // �ō����_����
static Plan plan_best;  // �I�΂ꂵ�u����

static void search( Field &field, Next &next, int level, int lim ){
	const Tumo TUMO = next.get(level);
	const int  LOOP = (TUMO.first==TUMO.second) ? 11 : 22; // ���F�Ȃ甼��
	for( int i=0; i<LOOP; ++i ){
		// 1��u�����V�����t�B�[���h�̍쐬
		Field      new_field = field;
		const Plan PLAN      = getPlan(i);
		if( setTumo( new_field, TUMO, PLAN ) == -1 ) continue;
		// �����������
		if( level==0 ) plan_first = PLAN;
		// ���΂���
		InfoFire info;
		fireField( new_field, info );
		// �����m�F
		if( new_field.get(3,12)!=BRANK ) continue;
		// �t�B�[���h��]�����ĉߋ��ō��Ȃ烁��
		int score = eval_field( new_field, next ); // �t�B�[���h�`�̕]��
		if( score>max_score ){
			max_score = score;
			plan_best = plan_first;
		}
		// ����Ȃ�[�݂�
		if( level<lim ) search( new_field, next, level+1, lim );
	}	
}

Plan AI_debug( Field field, Next next ){
	max_score = -1;
	plan_best = (-1,-1);
	eval_field( field, next );
	//search( field, next, 0, 2 );
	if( max_score==-1 ) cout<<"���Εs��"<<endl;
	return plan_best;
}
