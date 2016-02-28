/************************************************************************/
/*                                                                      */
/*      �V�~�����[�^Scene                                               */
/*                                                                      */
/************************************************************************/
#include "SceneSimu.h"
#include "DxLib.h"
#include "input.h"
#include "ComboBoxDx.h"
#include "ButtonDx.h"
#include "AiManeger.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <direct.h>
#include <shlwapi.h>                // PathFileExists
#pragma comment(lib, "ShLwApi.Lib") // PathFileExists
using namespace std;
using namespace Puyo;

/*----------------------------------------------------------------------*/
/*      �萔                                                            */
/*----------------------------------------------------------------------*/
const int FIELD_LEFT = 50;     // �t�B�[���h�̍���X���W
const int FIELD_TOP  = 18;     // �t�B�[���h�̍���Y���W
const int CELL_W = 32;         // �Z���̕�
const int CELL_H = 32;         // �Z���̍���
const int NEXT_LEFT = 280;     // �l�N�X�g�̈��
const int NEXT_TOP  = 18+32;   // �l�N�X�g�̈��
const int SELECT_LEFT = 280;   // �F�I��̈��
const int SELECT_TOP  = 435;   // �F�I��̈��
const int HISTORY_MAX = 1024;  // �����̍ő�T�C�Y
const int COMBO_LEFT = 280;    // AI�I���R���{�{�b�N�X��
const int COMBO_TOP  = 250;    // AI�I���R���{�{�b�N�X��

/*----------------------------------------------------------------------*/
/*      �ϐ�                                                            */
/*----------------------------------------------------------------------*/
Puyo::Field m_field;           // �t�B�[���h
Puyo::Next m_next;             // �l�N�X�g

static int  m_next_size = 2;   // �l�N�X�g�\���̈�̃T�C�Y

static eKIND m_select_kind = BRANK; // �I�𒆂̐F
static int  m_select_color_pos;     // �F�I��̈��ł̑I���ʒu
static int m_select_x;              // �}�E�X�őI�����Ă���t�B�[���h�̈ʒuX
static int m_select_y;              // �}�E�X�őI�����Ă���t�B�[���h�̈ʒuY
static int  m_select_next_x = -1;   // �}�E�X�őI�����Ă���l�N�X�g�̈ʒuX
static int  m_select_next_y = -1;   // �}�E�X�őI�����Ă���l�N�X�g�̈ʒuY

static int m_operate_x;        // ���쒆�Ղ�x
static int m_operate_y;        // ���쒆�Ղ�y
static int m_operate_dir;      // ���쒆�Ղ�dir
int sub_x( int x, int dir ){
	switch( dir ){
	case 0: return x;
	case 1: return x+1;
	case 2: return x;
	case 3: return x-1;
	} return -1;
}
int sub_y( int y, int dir ){
	switch( dir ){
	case 0: return y+1;
	case 1: return y;
	case 2: return y-1;
	case 3: return y;
	} return -1;
}
#define m_operate_sub_x (sub_x(m_operate_x,m_operate_dir)) // ���쒆�]���Ղ�x
#define m_operate_sub_y (sub_y(m_operate_y,m_operate_dir)) // ���쒆�]���Ղ�y

static int m_turn;             // ������ڂ�
static int m_score;            // ���̍��v���_

enum eCHAIN_MODE{ NOT_CHAIN_MODE, WAITING_FIRE, WAITING_FALL };
static int  m_chain_mode;        // �A�����Ƃ����̏��
static int  m_chain_flame_lim;   // ���̏�ԑJ�ڂ܂ŉ��t���[����
static int  m_chain_num;         // �����A���ڂ�
static int  m_chain_score;       // �A�����̍��v���_

ComboBoxDx m_combo( COMBO_LEFT, COMBO_TOP ); // �R���{�{�b�N�X�iAI�ꗗ�p�j
ButtonDx m_next_m(280,120,1);
ButtonDx m_next_p(298,120,2);

struct History{ // �����Ǘ��p
	int   pos[HISTORY_MAX];
	int   dir[HISTORY_MAX];
	Field field[HISTORY_MAX];
	int   score[HISTORY_MAX];
	int   size;

	History();
	void init();
};
static History m_history;

static bool help_flag = false;   // help��\�����邩�t���O

static int   m_support_score;     //
static int   m_support_chain_num; // 
static Field m_support_field;     // 

void update_support_info();

/*----------------------------------------------------------------------*/
/*      �������֐� SceneSimuInit()                                      */
/*----------------------------------------------------------------------*/
void SceneSimuInit(){
	m_field.init();
	m_next.init();
	m_turn = 0;
	m_operate_x = 3;
	m_operate_y = 13;
	m_operate_dir = 0;
	m_chain_mode = NOT_CHAIN_MODE;
	m_chain_flame_lim = 0;
	m_chain_num = 0;
	m_chain_score = 0;
	m_score = 0;
	vector<string> ai_names;
	for( int i=0; i<getAiNum(); ++i )
		ai_names.push_back( getAiName(i) );
	m_combo.setElements( ai_names );
	m_history.init();
	update_support_info();
}

/*----------------------------------------------------------------------*/
/*      �X�V�֐� SceneSimuUpdate()                                      */
/*----------------------------------------------------------------------*/
void check_DD();
void update_select_pos();
void select_click();
void move_operate_right();
void move_operate_left();
void rotate_operate_left();
void rotate_operate_right();
void go_next_turn();
void set_operate_puyo();
void update_chain_info();
void undo();
void redo();
void save();
void load( char *path );
void ai_think_move();
void reset();
void debug();

void SceneSimuUpdate(){
	if( m_chain_flame_lim>0 ) --m_chain_flame_lim;

	// D&D�̃`�F�b�N
	check_DD();

	// �t�B�[���h��̑I���ӏ����X�V
	update_select_pos();
	// �R���{�{�b�N�X�̑I���󋵂��X�V
	m_combo.update();
	// �{�^���̑I���󋵂��X�V
	m_next_p.update();
	m_next_m.update();
	if( m_next_p.getState() ) m_next_size++;
	if( m_next_m.getState() && m_next_size>0 ) m_next_size--;

	// �펞��������L�[���͏���
	if( GetStateKey(KEY_INPUT_0)     == 1 || GetStateKey(KEY_INPUT_NUMPAD0)==1 ) m_select_kind = BRANK;
	if( GetStateKey(KEY_INPUT_1)     == 1 || GetStateKey(KEY_INPUT_NUMPAD1)==1 ) m_select_kind = RED;
	if( GetStateKey(KEY_INPUT_2)     == 1 || GetStateKey(KEY_INPUT_NUMPAD2)==1 ) m_select_kind = BLUE;
	if( GetStateKey(KEY_INPUT_3)     == 1 || GetStateKey(KEY_INPUT_NUMPAD3)==1 ) m_select_kind = YELLOW;
	if( GetStateKey(KEY_INPUT_4)     == 1 || GetStateKey(KEY_INPUT_NUMPAD4)==1 ) m_select_kind = GREEN;
	if( GetStateKey(KEY_INPUT_5)     == 1 || GetStateKey(KEY_INPUT_NUMPAD5)==1 ) m_select_kind = OJAMA;
	if( GetStateKey(KEY_INPUT_T)     == 1 ) SceneSimuInit();
	if( GetStateKey(KEY_INPUT_R)     == 1 ) reset(), update_support_info();
	if( GetStateKey(KEY_INPUT_S)     == 1 ) save();
	if( GetStateKey(KEY_INPUT_F1)    == 1 ) help_flag = !help_flag;
	if( GetStateKey(KEY_INPUT_UP)    == 1 ) undo(), m_chain_mode = NOT_CHAIN_MODE; // TODO:������ƕs�\��
	if( GetStateKey(KEY_INPUT_ADD)   == 1 ) m_next_size++;
	if( GetStateKey(KEY_INPUT_MINUS) == 1 || GetStateKey(KEY_INPUT_SUBTRACT)==1 )
		if( m_next_size>0 ) m_next_size--;
	
	// ��Ԃɂ���ď����̐U�蕪��
	switch( m_chain_mode ){
	case NOT_CHAIN_MODE: // ��A����
		// �}�E�X��������Ă���
		if( GetStateMouse(MOUSE_INPUT_LEFT)==1 ) select_click();

		if( GetStateKey(KEY_INPUT_DOWN)  == 1 ) set_operate_puyo();
		if( GetStateKey(KEY_INPUT_RIGHT) == 1 ) move_operate_right();
		if( GetStateKey(KEY_INPUT_LEFT)  == 1 ) move_operate_left(); 
		if( GetStateKey(KEY_INPUT_Z)     == 1 ) rotate_operate_left();
		if( GetStateKey(KEY_INPUT_X)     == 1 ) rotate_operate_right();
		if( GetStateKey(KEY_INPUT_C)     == 1 ) ai_think_move();
		if( GetStateKey(KEY_INPUT_F)     == 1 ) redo();
		if( GetStateKey(KEY_INPUT_D)     == 1 ) debug();
		break;
	case WAITING_FIRE: // ���Αҋ@��
		if( m_chain_flame_lim==0 ){
			update_chain_info();
			m_chain_mode = WAITING_FALL;
			m_chain_flame_lim = 20;
		}
		break;
	case WAITING_FALL: // �����ҋ@��
		if( m_chain_flame_lim==0 ){
			fallPuyo( m_field );
			if( canFire( m_field ) ){
				m_chain_mode = WAITING_FIRE;
				m_chain_flame_lim = 20;
			} else {
				m_score += m_chain_score;
				go_next_turn();
			}
		}
		break;
	}

}

/*----------------------------------------------------------------------*/
/*      �`��֐� SceneSimuDraw()                                        */
/*----------------------------------------------------------------------*/
static void drawPuyo( int x, int y, int w, int h, Puyo::eKIND color ){
	int PUYO_COLOR = 0x0;
	switch( color ){
	case Puyo::BRANK  : return;
	case Puyo::RED    : PUYO_COLOR = 0xff0000; break;
	case Puyo::GREEN  : PUYO_COLOR = 0x00ff00; break;
	case Puyo::BLUE   : PUYO_COLOR = 0x0000ff; break;
	case Puyo::YELLOW : PUYO_COLOR = 0xffff00; break;
	case Puyo::OJAMA  : PUYO_COLOR = 0xffffff; break;
	}
	DrawOval( x, y, w/2, h/2, PUYO_COLOR, true );
}
static void drawHelp(){
	const int LEFT   = 25;
	const int TOP    = 50;
	const int RIGHT  = 640 - LEFT;
	const int BOTTOM = 480 - TOP;
	SetDrawBlendMode( DX_BLENDMODE_ALPHA, 128 );
	DrawBox( LEFT,   TOP,   RIGHT,   BOTTOM,   0xffffff, true ); // �w�i
	DrawBox( LEFT+5, TOP+5, RIGHT-5, BOTTOM-5, 0xffffff, true ); // �L�[����ꗗ�̔w�i
	SetDrawBlendMode( DX_BLENDMODE_NOBLEND, 0 );

	const int X1 = 80;
	const int X2 = 170;
	DrawString( 60,  70, "����ꗗ", 0x0 ) ;
	DrawString( X1,  90, "[��][��] ", 0x0 ) ; DrawString( X2,  90, ": �ړ�", 0x0 ) ;
	DrawString( X1, 110, "[��]     ", 0x0 ) ; DrawString( X2, 110, ": �ݒu", 0x0 ) ;
	DrawString( X1, 130, "[Z]      ", 0x0 ) ; DrawString( X2, 130, ": ����]", 0x0 ) ;
	DrawString( X1, 150, "[X]      ", 0x0 ) ; DrawString( X2, 150, ": �E��]", 0x0 ) ;
	DrawString( X1, 170, "[��][P]  ", 0x0 ) ; DrawString( X2, 170, ": 1��߂�", 0x0 ) ;
	DrawString( X1, 190, "[F]      ", 0x0 ) ; DrawString( X2, 190, ": 1��i��", 0x0 ) ;
	DrawString( X1, 210, "[0-5]    ", 0x0 ) ; DrawString( X2, 210, ": �I��F�̐؂�ւ�", 0x0 ) ;
	DrawString( X1, 230, "[R]      ", 0x0 ) ; DrawString( X2, 230, ": ���g���C", 0x0 ) ;
	DrawString( X1, 250, "[T]      ", 0x0 ) ; DrawString( X2, 250, ": ������", 0x0 ) ;
	DrawString( X1, 270, "[S]      ", 0x0 ) ; DrawString( X2, 270, ": �Z�[�u", 0x0 ) ;
	DrawString( X1, 290, "[C]      ", 0x0 ) ; DrawString( X2, 290, ": AI���s", 0x0 ) ;
	DrawString( X1, 310, "[-][+]   ", 0x0 ) ; DrawString( X2, 310, ": NEXT�\��������", 0x0 ) ;

	DrawString( X1, 340, "[F1]     ", 0x0 ) ; DrawString( X2, 340, ": Help�\��/��\��", 0x0 ) ;
	DrawString( X1, 370, "�V�~�����[�^���.rep�t�@�C����D&D�Ń��[�h", 0x0 ) ;
	DrawString( X1, 390, "AI�̊J�����@��readme.txt�Q��", 0x0 ) ;
}
void SceneSimuDraw(){
	// �t�B�[���h�̕`��
	for( int x=1; x<=6; ++x ){
	for( int y=1; y<=14; ++y ){
		const int SX = FIELD_LEFT+(x-1)*CELL_W;
		const int SY = FIELD_TOP +(14-y)*CELL_H;
		// �w�i�̕`��
		const int CELL_COLOR = (y==14)?0x111111:(y==13)?0x444444:0x888888;
		DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, CELL_COLOR, true );
		DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0x222222, false );
		// �����_�̕`��
		if( x==3 && y==12 )
			DrawLine( SX+4, SY+4, SX+CELL_W-4, SY+CELL_H-4, 0xff0000, 3 ),
			DrawLine( SX+4, SY+CELL_H-4, SX+CELL_W-4, SY+4, 0xff0000, 3 );
		// �t�B�[���h��̑I���ʒu��`��
		if( m_select_x==x && m_select_y==y ){
			DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0xffffff, false );
		}
		// �t�B�[���h�Ղ�̕`��
		Puyo::eKIND kind = m_field.get(x,y);
		if( kind!=Puyo::BRANK )
			drawPuyo( SX+CELL_W/2, SY+CELL_H/2, CELL_W-1, CELL_H-1, kind );
		// ���쒆�Ղ�̕`��
		if( m_chain_mode==NOT_CHAIN_MODE ){
			Puyo::Tumo tumo = m_next.get(m_turn);
			if( m_operate_x==x && m_operate_y==y )
				drawPuyo( SX+CELL_W/2, SY+CELL_H/2, CELL_W-1, CELL_H-1, tumo.first );
			if( m_operate_sub_x==x && m_operate_sub_y==y )
				drawPuyo( SX+CELL_W/2, SY+CELL_H/2, CELL_W-1, CELL_H-1, tumo.second );
		}
	}}
	// NEXT�G���A�̕`��
	for( int x=0; x<m_next_size; ++x ){
		Puyo::eKIND kind[2];
		Puyo::Tumo tumo = m_next.get(m_turn+x+1);
		kind[0] = tumo.second;
		kind[1] = tumo.first;
		for( int y=0; y<2; ++y ){
			const int SX = NEXT_LEFT+x*CELL_W;
			const int SY = NEXT_TOP+y*CELL_H;
			const int CELL_COLOR = 0x444444;
			DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, CELL_COLOR, true );
			DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0x222222, false );
			// �l�N�X�g��̑I���ʒu��`��
			if( m_select_next_x==x && m_select_next_y==y ){
				DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0xffffff, false );
			}
			// �Ղ�̕`��
			drawPuyo( SX+CELL_W/2, SY+CELL_H/2, CELL_W-1, CELL_H-1, kind[y] );
		}
	}
	// NEXT�\���̈�ύX�{�^���̕`��
	m_next_p.draw();
	m_next_m.draw();
	// �F�I���G���A�̕`��
	{
		eKIND kind[6] = { BRANK, RED, BLUE, YELLOW, GREEN, OJAMA };
		for( int i=0; i<6; ++i ){
			const int SX = SELECT_LEFT+i*CELL_W;
			const int SY = SELECT_TOP;
			// �w�i�̕`��
			DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0x444444, true );
			DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0x222222, false );
			// �Ղ�̕`��
			drawPuyo( SX+CELL_W/2, SY+CELL_H/2, CELL_W-1, CELL_H-1, kind[i] );
			// �F�I��̈��̑I���ʒu��`��
			if( m_select_color_pos==i ){
				DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0xffffff, false );
			}
			// �F�I�𒆂̈ʒu��`��
			if( m_select_kind==kind[i] ){
				DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0xff0000, false );
			}
		}
	}

	// ���̑����̕`��
	DrawFormatString( 280, 5, 0xffffff, "�萔:%d", m_turn );
	DrawFormatString( 280, 160, 0xffffff, "�A����:%d", m_chain_num );
	DrawFormatString( 280, 180, 0xffffff, "���_:%d", m_chain_score );
	DrawFormatString( 280, 200, 0xffffff, "���v���_:%d", m_score );

	// �T�|�[�g���̕`��
	DrawFormatString( 500, 130, 0xaaaaaa, "�A����:%d", m_support_chain_num );
	DrawFormatString( 500, 150, 0xaaaaaa, "���_:%d", m_support_score );
	for( int x=1; x<=6; ++x ){
	for( int y=1; y<=14; ++y ){
		const int CELL_W = 21;
		const int CELL_H = 21;
		const int SX = 500+(x-1)*CELL_W;
		const int SY = 180+(14-y)*CELL_H;
		// �w�i�̕`��
		const int CELL_COLOR = (y==14)?0x111111:(y==13)?0x444444:0x888888;
		DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, CELL_COLOR, true );
		DrawBox( SX, SY, SX+CELL_W, SY+CELL_H, 0x222222, false );
		// �����_�̕`��
		if( x==3 && y==12 )
			DrawLine( SX+4, SY+4, SX+CELL_W-4, SY+CELL_H-4, 0xff0000, 3 ),
			DrawLine( SX+4, SY+CELL_H-4, SX+CELL_W-4, SY+4, 0xff0000, 3 );
		// �t�B�[���h�Ղ�̕`��
		Puyo::eKIND kind = m_support_field.get(x,y);
		if( kind!=Puyo::BRANK )
			drawPuyo( SX+CELL_W/2, SY+CELL_H/2, CELL_W-1, CELL_H-1, kind );
	}}
	// AI�R���{�{�b�N�X�̕`��
	m_combo.draw();

	// �w���v�̕`��
	DrawFormatString( 540, 5, 0xffffff, "[F1]:Help" );
	if( help_flag )
		drawHelp();
}


/*----------------------------------------------------------------------*/
/*      �X�V�Ɏg���֐��Q                                                */
/*----------------------------------------------------------------------*/
static void check_DD(){
char buf[256];
	if(!GetDragFilePath(buf)){
		load( buf );
		DragFileInfoClear(); // ��ȏ�̃t�@�C���͓ǂ܂Ȃ�
	}
}
static void update_select_pos(){
	int xbuf,ybuf;
	if( GetMousePoint( &xbuf, &ybuf ) !=-1 ){
		// �t�B�[���h��
		int dx = xbuf - FIELD_LEFT;
		int dy = ybuf - FIELD_TOP;
		if( dx>=0 && dx<=6*CELL_W )
			 m_select_x = dx/CELL_W + 1;
		else m_select_x = 0;
		if( dy>=0 && dy<=14*CELL_H )
			 m_select_y = 13-dy/CELL_H +1 ;
		else m_select_y = 0;
		// �l�N�X�g��
		dx = xbuf - NEXT_LEFT;
		dy = ybuf - NEXT_TOP;
		if( dx>=0 && dx<=m_next_size*CELL_W )
			 m_select_next_x = dx/CELL_W;
		else m_select_next_x = -1;
		if( dy>=0 && dy<=CELL_H*m_next_size )
			 m_select_next_y = dy/CELL_H;
		else m_select_next_y = -1;
		// �F�I��̈��
		dx = xbuf - SELECT_LEFT;
		dy = ybuf - SELECT_TOP;
		if( dx>=0 && dx<=6*CELL_W && dy>=0 && dy<=CELL_H )
			 m_select_color_pos = dx/CELL_W;
		else m_select_color_pos = -1;
	}
}
static void select_click(){
	// �͈̓`�F�b�N
	// �t�B�[���h��
	if( m_select_x>=1 && m_select_x<=6 && m_select_y>=1 && m_select_y<=14 ){
		// ���쒆�Ղ���X�V����ꍇ
		if( m_select_x==m_operate_x && m_select_y==m_operate_y ){
			if( m_select_kind!=BRANK && m_select_kind!=OJAMA )
				m_next.set(m_turn,0,m_select_kind);
			update_support_info();
			return;
		}
		if( m_select_x==m_operate_sub_x && m_select_y==m_operate_sub_y ){
			if( m_select_kind!=BRANK && m_select_kind!=OJAMA )
				m_next.set(m_turn,1,m_select_kind);
			update_support_info();
			return;
		}
		// �t�B�[���h���X�V����ꍇ
		m_field.set( m_select_x, m_select_y, m_select_kind );
		m_history.field[m_turn] = m_field;
		update_support_info();
	}
	eKIND kind[6] = { BRANK, RED, BLUE, YELLOW, GREEN, OJAMA };
	// �l�N�X�g��
	if( m_select_next_x>=0 && m_select_next_x<m_next_size && m_select_next_y>=0 && m_select_next_y<=1 ){
		if( m_select_kind!=BRANK && m_select_kind!=OJAMA  ){
			m_next.set( m_turn+m_select_next_x+1, !m_select_next_y, m_select_kind );
		}
	}
	// �F�I��̈��
	if( m_select_color_pos>=0 && m_select_x<=5 ){
		m_select_kind = kind[m_select_color_pos];
	}
}
static void move_operate_right(){
	m_operate_x++;
	do{
		if( m_operate_x>6 || m_operate_sub_x>6 ) break;
		if( m_field.get(m_operate_x,m_operate_y)!=Puyo::BRANK ) break;
		if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ) break;
		// �����܂ŗ��ꂽ��ړ�����
		update_support_info();
		return;
	} while( 0 );
	m_operate_x--;
}
static void move_operate_left(){
	m_operate_x--;
	do{
		if( m_operate_x<1 || m_operate_sub_x<1 ) break;
		if( m_field.get(m_operate_x,m_operate_y)!=Puyo::BRANK ) break;
		if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ) break;
		// �����܂ŗ��ꂽ��ړ�����
		update_support_info();
		return;
	} while( 0 );
	m_operate_x++;
}
static void rotate_operate_left(){
	int dir_bak = m_operate_dir;
	m_operate_dir = (m_operate_dir+3)%4;
	do{
		if( m_operate_dir==0 )
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ) break;
		if( m_operate_dir==2 )
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ) break;
		if( m_operate_dir==1 )
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ){ // �E���������鎞
				if( m_field.get(m_operate_x-1,m_operate_y)==Puyo::BRANK ){
					m_operate_x--;
					update_support_info();
					return;
				}
				break;
			}
		if( m_operate_dir==3 ){
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ){ // �����������鎞
				if( m_field.get(m_operate_x+1,m_operate_y)==Puyo::BRANK ){
					m_operate_x++;
					update_support_info();
					return;
				}
				break;
			}
		}
		update_support_info();
		return;
	} while( 0 );
	m_operate_dir = dir_bak;
}
static void rotate_operate_right(){
	int dir_bak = m_operate_dir;
	m_operate_dir = (m_operate_dir+1)%4;
	do{
		if( m_operate_dir==0 )
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ) break;
		if( m_operate_dir==2 )
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ) break;
		if( m_operate_dir==1 )
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ){ // �E���������鎞
				if( m_field.get(m_operate_x-1,m_operate_y)==Puyo::BRANK ){
					m_operate_x--;
					update_support_info();
					return;
				}
				break;
			}
		if( m_operate_dir==3 ){
			if( m_field.get(m_operate_sub_x,m_operate_sub_y)!=Puyo::BRANK ){ // �����������鎞
				if( m_field.get(m_operate_x+1,m_operate_y)==Puyo::BRANK ){
					m_operate_x++;
					update_support_info();
					return;
				}
				break;
			}
		}
		update_support_info();
		return;
	} while( 0 );
	m_operate_dir = dir_bak;
}
static void go_next_turn(){
	m_chain_mode = NOT_CHAIN_MODE;
	// 14�i�ڂ̏���
	for( int x=1; x<=6; ++x ) m_field.set(x,14,BRANK);
	m_operate_x = 3; m_operate_y = 13; m_operate_dir = 0;
	// �����ɕۑ�
	m_history.pos[m_turn+1] = 3;
	m_history.dir[m_turn+1] = 0;
	m_history.field[m_turn+1] = m_field;
	m_history.score[m_turn+1] = m_score;
	m_history.size = m_turn+1;
	++m_turn;
	update_support_info();
}
static void set_operate_puyo(){
	m_field.set( m_operate_x,     m_operate_y,     m_next.get(m_turn,0) );
	m_field.set( m_operate_sub_x, m_operate_sub_y, m_next.get(m_turn,1) );
	fallPuyo( m_field );
	// �����ɕۑ� ����
	m_history.pos[m_turn] = m_operate_x;
	m_history.dir[m_turn] = m_operate_dir;
	// ���Ίm�F
	if( canFire(m_field) ){
		m_chain_num = 0;
		m_chain_score = 0;
		m_chain_mode = WAITING_FIRE;
		m_chain_flame_lim = 20;
	} else {
		go_next_turn();
	}
}
static void update_chain_info(){
	InfoStepFire info;
	stepFire( m_field, info );
	m_chain_num ++;

	const int A[] = {0,8,16,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512};
	const int B[] = {0,3,6,12,24};
	const int C[] = {0,2,3,4,5,6,7,10};

	int score = info.num*10; // ��{�_
	int bonas = 0;           // �{��
	bonas += A[m_chain_num-1];
	bonas += B[info.color_num-1];
	for( size_t i=0; i<info.connection.size(); ++i ){
		const int c = info.connection[i];
		bonas += (c>=11)?10:C[c-4];
	}
	if( bonas==0 ) bonas = 1;
	m_chain_score += score*bonas;
}
static void undo(){
	if( m_turn<=0 ) return;
	--m_turn;
	m_operate_x   = m_history.pos[m_turn];
	m_operate_dir = m_history.dir[m_turn];
	m_field = m_history.field[m_turn];
	m_score = m_history.score[m_turn];
	update_support_info();
}
static void redo(){
	if( m_history.size<=m_turn ) return;
	++m_turn;
	m_operate_x   = m_history.pos[m_turn];
	m_operate_dir = m_history.dir[m_turn];
	m_field       = m_history.field[m_turn];
	m_score       = m_history.score[m_turn];
	update_support_info();
}
static void save(){
	// replay�t�H���_��������΍��
	if( _mkdir("replay")!=-1 ) cout<<"replay�t�H���_���쐬���܂���"<<endl;
	// replay�t�H���_����dddd.rep�̋󂫂�T��
	char filename[256];
	for( int i=0; i<10000; ++i ){
		sprintf_s( filename, 256, "./replay/%04d.rep", i );
		if( PathFileExists(filename) ) continue;
		break;
	}
	// �t�@�C���֏�������
	ofstream ofs(filename, ios::binary);
	char n = m_turn;
	ofs.write( (char *)&n, sizeof(n) );
	char tmp[256];
	for( int i=0; i<128; ++i ) tmp[i*2] = m_next.get(i,0), tmp[i*2+1] = m_next.get(i,1);
	ofs.write( (char *)&tmp, sizeof(tmp) );
	for( int i=0; i<128; ++i ) tmp[i*2] = m_history.pos[i], tmp[i*2+1] = m_history.dir[i];
	ofs.write( (char *)&tmp, sizeof(tmp) );
	cout<<filename<<"\n���v���C��ۑ����܂���."<<endl;
}
static void load( char *path ){
	ifstream ifs( path, ios::binary );
	if( !ifs ){	printf( "�I�[�v���Ɏ��s:%s\n", path ); return; }

	SceneSimuInit();
	char hs;
	ifs.read( (char *)&hs, sizeof(char) );
	m_history.size = hs;
	char tmp[256];
	ifs.read( (char *)&tmp, sizeof(char)*256 );
	for( int i=0; i<128; ++i ) m_next.set(i,0,(eKIND)tmp[i*2]), m_next.set(i,1,(eKIND)tmp[i*2+1]);
	ifs.read( (char *)&tmp, sizeof(char)*256 );
	for( int i=0; i<128; ++i ) m_history.pos[i] = tmp[i*2], m_history.dir[i] = tmp[i*2+1];
	cout<<path<<"\n���v���C�����[�h���܂���."<<endl;
	// �\�ߖ������V�~�����[�g���ė���������Ă���
	for( int i=0; i<m_history.size; ++i ){
		Plan plan( m_history.pos[i], m_history.dir[i] );
		setTumo( m_field, m_next.get(i), plan );
		// ���Ώ���
		if( canFire(m_field) ){
			m_chain_score = 0; m_chain_num = 0;
			while( canFire(m_field) ){
				update_chain_info();
				fallPuyo(m_field);
			}
			m_score += m_chain_score;
		}
		m_history.score[i+1] = m_score;
		m_history.field[i+1] = m_field;
	}
	m_turn = m_history.size;
}
static void reset(){
	m_operate_x   = 3;
	m_operate_dir = 0;
	m_turn = 0;
	m_field.init();
	m_chain_num = 0;
	m_chain_score = 0;
	m_score = 0;
	m_history.field[0] = m_field;
	m_chain_mode = NOT_CHAIN_MODE;
}
static void ai_think_move(){
	const int num = m_combo.getSelectPos();
	Puyo::Next next;
	for( int i=0; i<128; ++i ) next.set(i,m_next.get(m_turn+i));
	printf( "> �v�Z�J�n : %s\n", getAiName(num) );
	DWORD st = GetTickCount();
	Plan result = executeAi( num, m_field, next );
	DWORD dt = GetTickCount() - st;
	printf("> �v�Z�I�� (%d ms) \n> Result:(%d,%d)\n", dt, result.pos, result.dir );
	if( result.pos>=1 && result.pos<=6 && result.dir>=0 && result.dir<=4 ){
		m_operate_x = result.pos;
		m_operate_dir = result.dir;
	}
	update_support_info();
}

History::History(){
	init();
}
void History::init(){
	memset( pos, 0, sizeof(pos) );
	memset( dir, 0, sizeof(pos) );
	memset( score, 0, sizeof(pos) );
	for( int i=0; i<HISTORY_MAX; ++i ){
		field[i].init();
	}
	size = 0;
}



void update_support_info(){
	InfoFire info;
	Field tmp = m_field;
	Plan plan( m_operate_x, m_operate_dir );
	setTumo( tmp, m_next.get(m_turn), plan );
	fireField( tmp, info );
	m_support_score = info.score;
	m_support_chain_num = info.chain_num;
	m_support_field = tmp;
}

extern void pm_test( Field field );
void debug(){

	pm_test( m_field );

#if 0
	Field field = m_field;
	int height[8];
	for( int i=1; i<=6; ++i ) height[i] = countHeight( field, i );

	// 1.���Γ_�T��
	// �Ƃ肠����1�u���Ă݂�
	for( int x=1; x<=6; ++x ){
	const int Y = height[x]+1;
	if( Y>12 ) continue;
	for( int i=1; i<=4; ++i ){
		field.set( x, Y, (eKIND)i );         // 1�u���Ă݂�
		if( countConnection(field,x,Y)>=4 ){ // 4�A���ȏサ���甭�΂���
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
			//fire_score = max( fire_score, score ); // �ߋ��ō��Ȃ烁��
		}
		field.set( x, height[x]+1, BRANK ); // �u�����Ղ�͏���
	}}
	cout<<"test"<<endl;
#endif
}