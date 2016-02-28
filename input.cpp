/************************************************************************/
/*                                                                      */
/*           �L�[���͊֘A                                               */
/*                                                                      */
/************************************************************************/
#include "DxLib.h"

// ���t���[�������������Ă��邩�̃J�E���^
static int key_count[256];
static int mouse_l_count;
static int mouse_r_count;

// input���X�V ���t���[���Ă�
int UpdateInputInfo(){
	// �L�[�{�[�h���X�V
    char key[256];
    GetHitKeyStateAll( key );
    for(int i=0;i<256;i++){
        if( key[i]==1 ) key_count[i]++;
        else            key_count[i] = 0;
    }
	// �}�E�X���X�V
	int mouse = GetMouseInput();
	if( mouse&MOUSE_INPUT_LEFT  ) mouse_l_count++;
	else                          mouse_l_count = 0;
	if( mouse&MOUSE_INPUT_RIGHT ) mouse_r_count++;
	else                          mouse_r_count = 0;

    return 0;
}

// handle�̉�����Ă����t���[������Ԃ� �L�[�{�[�h�p
int GetStateKey(unsigned char handle){
        return key_count[handle];
}
// handle�̉�����Ă����t���[������Ԃ��@�}�E�X�p
int GetStateMouse(unsigned char handle){
	if( handle==MOUSE_INPUT_LEFT  ) return mouse_l_count;
	if( handle==MOUSE_INPUT_RIGHT ) return mouse_r_count;
	return 0;
}


// �����L�[��������Ă�����true
bool CheckStateKeyAll(){
	for( int i=0; i<256; ++i ){
		if( GetStateKey(i) ) return true;
	}
	return false;

}