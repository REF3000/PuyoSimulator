#include <DxLib.h>
#include "ButtonDx.h"
using namespace std;

ButtonDx::ButtonDx( int left, int top, int id ){
	LEFT = left; TOP = top;
	press_down_flag = false;
	mouse_over_flag = false;
	press_down_first_flag = false;
	this->id = id;
}

void ButtonDx::update(){
	int mx,my;
	if( GetMousePoint( &mx, &my ) ==-1 ) return;

	// 表示部をマウスオーバーしているか
	if( mx>LEFT && mx<(LEFT+W) && my>TOP && my<TOP+H )
		mouse_over_flag = true;
	else
		mouse_over_flag = false;

	// マウス左が押されている間press()を呼ぶ
	press_down_first_flag = false;
	if( mouse_over_flag && ( GetMouseInput() & MOUSE_INPUT_LEFT ) ){
		press();
	} else {
		press_down_flag = false;
	}
}

void ButtonDx::press(){
	if( !press_down_flag ) // 初回
		press_down_first_flag = true;
	press_down_flag = true;

}

void ButtonDx::draw(){
	const int X = LEFT;
	const int Y = TOP;

	if( press_down_flag ){
		// 押されている
		DrawBox( X, Y, X+W, Y+H, 0x222222, true );
	} else {
		// 押されていない
		if( mouse_over_flag ){
			// マウス乗ってる
			DrawBox( X, Y, X+W, Y+H, 0x888888, true );
		} else {
			// マウス乗ってない
			DrawBox( X, Y, X+W, Y+H, 0x666666, true );
		}
	}

	// idごとの描画
	int mX = ((X+W)+X)/2;
	int mY = ((Y+H)+Y)/2;
	if( id==1 ){        // -
		DrawBox( mX-5, mY-1, mX+5, mY+1, 0xaa0000, true );
	} else if( id==2 ){ // +
		DrawBox( mX-5, mY-1, mX+5, mY+1, 0xaa0000, true );
		DrawBox( mX-1, mY-5, mX+1, mY+5, 0xaa0000, true );
	}
}

int ButtonDx::getState(){
	return press_down_first_flag;
}