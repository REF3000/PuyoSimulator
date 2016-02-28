#include <DxLib.h>
#include "ComboBoxDx.h"
using namespace std;

ComboBoxDx::ComboBoxDx( int left, int top ){
	LEFT = left; TOP = top;
	OPEN_TOP = top + LINE_H + 1;
	select_pos = 0;
	drop_down_flag = 0;
	mouse_over_num = 0;
}
void ComboBoxDx::setElements( vector<string> elements ){
	element_names = elements;
	size = elements.size();
}

void ComboBoxDx::update(){
	int mx,my;
	if( GetMousePoint( &mx, &my ) ==-1 ) return;

	// �\�������}�E�X�I�[�o�[���Ă��邩
	if( mx>LEFT && mx<(LEFT+LINE_W) && my>TOP && my<TOP+LINE_H )
		mouse_over_flag = true;
	else
		mouse_over_flag = false;

	// �W�J�����}�E�X�I�[�o�[���Ă��邩
	mouse_over_num = -1;
	if( mx>=LEFT && mx<=(LEFT+LINE_W) ){
		for( int i=0; i<size; ++i ){
			if( my>=OPEN_TOP+LINE_H*i && my<=OPEN_TOP+LINE_H*(i+1) ){
				mouse_over_num = i; break;
			}
		}
	}

	// �}�E�X���������ꂽ�u�Ԃ�click()���Ă�
	static bool click_flag = false;
	if( ( GetMouseInput() & MOUSE_INPUT_LEFT ) ){
		if( click_flag == false ) click();
		click_flag = true;
	} else {
		click_flag = false;
	}
}

void ComboBoxDx::click(){
	// �\����
	if( mouse_over_flag ){
		drop_down_flag = !drop_down_flag;
		return;
	}
	// �h���b�v�_�E����
	if( drop_down_flag && mouse_over_num!=-1 ){
		select_pos = mouse_over_num;
	}
	// �O
	drop_down_flag = false;
}

void ComboBoxDx::draw(){
	// �\����
	{
		// �\��
		{
			const int X = LEFT;
			const int Y = TOP;
			DrawBox( X,   Y,   X+LINE_W, Y+LINE_H, 0x444444, true );
			if( !drop_down_flag && mouse_over_flag )
				DrawBox( X+1, Y+1, X+LINE_W, Y+LINE_H, 0xcccccc, true ); // �}�E�X�I�[�o�[��
			else if( drop_down_flag )
				DrawBox( X+1, Y+1, X+LINE_W, Y+LINE_H, 0x888888, true ); // �W�J��
			else
				DrawBox( X+1, Y+1, X+LINE_W, Y+LINE_H, 0xaaaaaa, true );
			DrawString( X+5, Y+2, element_names[select_pos].c_str(), 0x0 ) ;
		}
		// ��
		{
			const int RIGHT = LEFT+LINE_W;
			const int X = RIGHT - 18;
			const int Y = TOP;
			DrawBox( X, Y, RIGHT,   Y+LINE_H,   0x444444, true );
			DrawBox( X, Y, RIGHT-1, Y+LINE_H-1, 0x888888, true );
			DrawTriangle(  X+3,Y+6,  X+15,Y+6,  X+9,Y+LINE_H-7  , 0x0, true );
		}
	}
	// �W�J��
	if( drop_down_flag ){
		for( int i=0; i<size; ++i ){
			const int X = LEFT;
			const int Y = OPEN_TOP + LINE_H*i;
			if( mouse_over_num==i )
				DrawBox( X, Y, X+LINE_W, Y+LINE_H, 0xaaaaaa, true ); // �}�E�X�I�[�o�[��
			else
				DrawBox( X, Y, X+LINE_W, Y+LINE_H, 0x888888, true );
			DrawString( X+5, Y+2, element_names[i].c_str(), 0x0 ) ;
		}
	}
}

int ComboBoxDx::getSelectPos(){
	return select_pos;
}