/************************************************************************/
/*                                                                      */
/*      DxLib�ŃR���{�{�b�N�X                                           */
/*                                                                      */
/*  REF 2015/11/21                                                      */
/*                                                                      */
/************************************************************************/
#include <vector>
#include <string>

class ComboBoxDx{
	static const int LINE_H = 20;
	static const int LINE_W = 180;
	int LEFT, TOP; // ������W
	int OPEN_TOP;  // �W�J���̍�����W
	std::vector<std::string> element_names;
	int size;
	int select_pos;
	bool mouse_over_flag;   // �\�����Ƀ}�E�X�I�[�o�[���Ă��邩
	bool drop_down_flag;    // �h���b�v�_�E�����Ă��邩
	int mouse_over_num;     // �W�J���̂ǂ��Ƀ}�E�X�I�[�o�[���Ă��邩 ���Ă��Ȃ�:-1

	void click();

public :
	ComboBoxDx( int left=0, int top=0 ); // �z�u���鍶����W������
	void setElements( std::vector<std::string> elements ); // vector<string>�ŗv�f��n��
	void update();          // ���t���[���Ă�
	void draw();            // �`�悵�����^�C�~���O�ŌĂ�
	int  getSelectPos();    // �I������Ă���id��Ԃ�
};