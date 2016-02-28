#include "Puyo.h"

// �ݒu���@
struct Plan{
	int pos, dir;
	Plan( int pos=-1, int dir=-1 ) : pos(pos), dir(dir){}
};

struct InfoStepFire{
	int num;                // ��������
	int color_num;          // �������F��
	std::vector<int> connection; // �A����
};
struct InfoFire{
	int score;              // ���v���_
	int chain_num;          // �A����
	std::vector<InfoStepFire> step_infos;
};

// ���낢��֗��֐�

// Plan��񋓂���Ƃ��g��
Plan getPlan( int pattern_id );
// �t�B�[���h��x��̍�����Ԃ�
int  countHeight( const Puyo::Field &field, int x );
// �t�B�[���h��Plan�Ńc����u��
int  setTumo( Puyo::Field &field, const Puyo::Tumo &tumo, const Plan &plan );
// �t�B�[���h���v�����g����
void printField( const Puyo::Field &field );
// Field�����Ή\��
bool canFire( const Puyo::Field &field );
// ���΂�1�X�e�b�v�i�߂�
void stepFire( Puyo::Field &field, InfoStepFire &info );
// ���΂��đS�X�e�b�v�i�߂�
void fireField( Puyo::Field &field, InfoFire &info );
// �󒆂̂Ղ��ݒu����
void fallPuyo( Puyo::Field &field );
// �ڑ�����Ă���Ղ�������Đڑ�����Ԃ��i�אڂ���������܂������j
int deleteConnection( Puyo::Field &field, int x, int y );
// �ڑ����𐔂���
int countConnection( const Puyo::Field &field, int x, int y );
