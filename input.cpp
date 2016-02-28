/************************************************************************/
/*                                                                      */
/*           キー入力関連                                               */
/*                                                                      */
/************************************************************************/
#include "DxLib.h"

// 何フレーム押し続けられているかのカウンタ
static int key_count[256];
static int mouse_l_count;
static int mouse_r_count;

// input情報更新 毎フレーム呼ぶ
int UpdateInputInfo(){
	// キーボード情報更新
    char key[256];
    GetHitKeyStateAll( key );
    for(int i=0;i<256;i++){
        if( key[i]==1 ) key_count[i]++;
        else            key_count[i] = 0;
    }
	// マウス情報更新
	int mouse = GetMouseInput();
	if( mouse&MOUSE_INPUT_LEFT  ) mouse_l_count++;
	else                          mouse_l_count = 0;
	if( mouse&MOUSE_INPUT_RIGHT ) mouse_r_count++;
	else                          mouse_r_count = 0;

    return 0;
}

// handleの押されていたフレーム数を返す キーボード用
int GetStateKey(unsigned char handle){
        return key_count[handle];
}
// handleの押されていたフレーム数を返す　マウス用
int GetStateMouse(unsigned char handle){
	if( handle==MOUSE_INPUT_LEFT  ) return mouse_l_count;
	if( handle==MOUSE_INPUT_RIGHT ) return mouse_r_count;
	return 0;
}


// 何かキーが押されていたらtrue
bool CheckStateKeyAll(){
	for( int i=0; i<256; ++i ){
		if( GetStateKey(i) ) return true;
	}
	return false;

}