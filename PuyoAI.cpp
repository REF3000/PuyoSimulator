#include "PuyoAI.h"

#include <iostream>
using namespace std;
using namespace Puyo;

Plan static plan_table[22] = {
	Plan( 1, 2 ),
	Plan( 2, 2 ),
	Plan( 3, 2 ),
	Plan( 4, 2 ),
	Plan( 5, 2 ),
	Plan( 6, 2 ),
	Plan( 1, 1 ),
	Plan( 2, 1 ),
	Plan( 3, 1 ),
	Plan( 4, 1 ),
	Plan( 5, 1 ), // �����܂œ��F�ł��g����͈� 11��
	Plan( 1, 0 ),
	Plan( 2, 0 ), 
	Plan( 3, 0 ),
	Plan( 4, 0 ),
	Plan( 5, 0 ),
	Plan( 6, 0 ),
	Plan( 2, 3 ),
	Plan( 3, 3 ),
	Plan( 4, 3 ),
	Plan( 5, 3 ),
	Plan( 6, 3 ),
};
// Plan��񋓂���Ƃ��g��
Plan getPlan( int pattern_id ){
	return plan_table[pattern_id];
}



// �t�B�[���h��x��̍�����Ԃ�
int countHeight( const Field &field, int x ){
	for( int y=1; y<=14; ++y )
		if( field.get(x,y)==BRANK ) return y-1;
	return 14;
}
// �t�B�[���h��Plan�Ńc����u�� TODO:�܂킵����̎���
int setTumo( Field &field, const Tumo &tumo, const Plan &plan ){
	// ��ԍ����Ƃ���ɐݒu���ė���������
	const int dx[] = { 0, 1, 0, -1 }; // �q�Ղ�̎��Ղ悩��̋��� x
	const int dy[] = { 1, 0, -1, 0 }; // �q�Ղ�̎��Ղ悩��̋��� y
	int axis_x = plan.pos;                
	int axis_y = 13;
	int sub_x  = axis_x + dx[plan.dir];
	int sub_y  = axis_y + dy[plan.dir];
	
	if( field.get(axis_x,axis_y)!=BRANK ) return -1; // �u���Ȃ�
	if( field.get(sub_x, sub_y )!=BRANK ) return -1; // �u���Ȃ�

	field.set( axis_x, axis_y, tumo.first  );
	field.set( sub_x,  sub_y,  tumo.second );
	fallPuyo( field );
	if( sub_y==14 )     // 14�i�ڂ͏���
		field.set( sub_x,  sub_y, BRANK );
	return 0;
}
// �t�B�[���h���v�����g����
void printField( const Field &field ){
	for( int y=13; y>=1; --y ){
		cout<<"��";
		for( int x=1;  x<=6; ++x ){
			switch( field.get(x,y) ){
			case Puyo::BRANK:  cout<<"�@"; break;
			case Puyo::RED:    cout<<"��"; break;
			case Puyo::BLUE:   cout<<"��"; break;
			case Puyo::YELLOW: cout<<"��"; break;
			case Puyo::GREEN:  cout<<"��"; break;
			case Puyo::OJAMA:  cout<<"��"; break;
			}
		}
		cout<<"��"<<endl;
	}
	cout<<"����������������"<<endl;
}
// Field�����Ή\��
bool canFire( const Field &field ){
	for( int x=1; x<=6; ++x ){
	for( int y=1; y<=13; ++y ){
		if( field.get(x,y)==BRANK ) break;
		if( countConnection( field, x, y )>=4 ) return true;
	}}
	return false;
}

// ���΂�1�X�e�b�v�i�߂�
void stepFire( Field &field, InfoStepFire &info ){
	info.color_num = 0;
	info.connection.clear();
	info.num = 0;
	unsigned char color_flag=0x0;
	for( int x=1; x<=6; ++x ){
	for( int y=1; y<=13; ++y ){
		eKIND kind = field.get(x,y);
		if( kind==BRANK ) continue;
		if( countConnection( field, x, y )<4 ) continue;
		int num = deleteConnection( field, x, y );
		info.num+=num;
		info.connection.push_back( num );
		color_flag |= (1<<(kind-1));
	}}
	for( int i=0; i<4; ++i ){
		if( color_flag&0x1 ) info.color_num ++;
		color_flag>>=1;
	}
}

// ���΂��đS�X�e�b�v�i�߂�
void fireField( Field &field, InfoFire &info ){
	const int A[] = {0,8,16,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512}; // �A���{�[�i�X�\
	const int B[] = {0,3,6,12,24};      // �F���{�[�i�X�\
	const int C[] = {0,2,3,4,5,6,7,10}; // �A���{�[�i�X�\

	info.score = 0;
	for( int i=0; i<20; ++i ){ // �ő�ꉞ20�A��
		InfoStepFire sinfo;
		stepFire( field, sinfo );
		if( sinfo.num<=0 ) break; // �A����������
		info.step_infos.push_back( sinfo );
		// step�ł̓��_���v�Z
		int base  = sinfo.num*10; // ��{�_
		int bonas = 0;            // �{��
		bonas += A[i];
		bonas += B[sinfo.color_num-1];
		for( size_t j=0; j<sinfo.connection.size(); ++j ){
			const int c = sinfo.connection[j];
			bonas += (c>=11)?10:C[c-4];
		}
		if( bonas==0 ) bonas = 1;
		info.score += base*bonas;
		fallPuyo( field );
	}
	info.chain_num = info.step_infos.size();
}

// �󒆂̂Ղ��ݒu����
void fallPuyo( Field &field ){
	for( int x=1; x<=6; ++x ){
		int p=1;
		for( int y=1; y<=14; ++y ){
			if( field.get(x,y)!=BRANK ){
				if( p==y ){ ++p; continue; }
				field.set(x,p++,field.get(x,y));
				field.set(x,y,BRANK);
			}
		}
	}
}

// delete�n
static int doDeleteConnection( Field &field, int x, int y, eKIND kind ){
	const eKIND field_kind = field.get(x,y);
	if( field_kind==OJAMA ){
		field.set(x,y,BRANK);
		return 0;
	}
	if( field_kind!=kind ) return 0;
	if( y>=13 ) return 0;
	field.set(x,y,BRANK);
	return doDeleteConnection( field, x, y-1, kind ) +
		   doDeleteConnection( field, x+1, y, kind ) +
		   doDeleteConnection( field, x, y+1, kind ) +
		   doDeleteConnection( field, x-1, y, kind ) + 1;
}
int deleteConnection( Field &field, int x, int y ){
	return doDeleteConnection( field, x, y, field.get(x,y) );
}

// count�n
static int doCountConnection( const Field &field, int x, int y, eKIND kind, char flag[8][16] ){
	if( flag[x][y] ) return 0;
	flag[x][y] = 1;
	if( y>=13 ) return 0;
	if( field.get(x,y)!=kind ) return 0;
	return doCountConnection( field, x, y-1, kind, flag ) +
		   doCountConnection( field, x+1, y, kind, flag ) +
		   doCountConnection( field, x, y+1, kind, flag ) +
		   doCountConnection( field, x-1, y, kind, flag ) + 1;
}
int countConnection( const Field &field, int x, int y ){
	if( field.get(x,y)==BRANK ) return 0;
	if( field.get(x,y)==OJAMA ) return 0;
	char flag[8][16]={0};
	return doCountConnection( field, x, y, field.get(x,y), flag );
}