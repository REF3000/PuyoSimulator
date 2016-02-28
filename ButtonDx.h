/************************************************************************/
/*                                                                      */
/*      DxLib�Ń{�^��                                                   */
/*                                                                      */
/*  REF 2015/11/25                                                      */
/*                                                                      */
/************************************************************************/
#include <vector>
#include <string>

class ButtonDx{
	static const int H = 15;
	static const int W = 15;
	int LEFT, TOP; // ������W
	bool mouse_over_flag;   // �\�����Ƀ}�E�X�I�[�o�[���Ă��邩
	bool press_down_flag;   // ������Ă��邩
	bool press_down_first_flag; // ������n�߂�1�񂾂�true

	int id;        // +�{�^����-�{�^������\�� �ǂ��Ȃ�����

	void press();

public :
	ButtonDx( int left=0, int top=0 , int id=0 ); // �z�u���鍶����W������
	void update();      // ���t���[���Ă�
	void draw();        // �`�悵�����^�C�~���O�ŌĂ�
	int  getState();    // ������n�߂�1�t���[������true
};