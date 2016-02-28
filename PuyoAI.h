#include "Puyo.h"

// 設置方法
struct Plan{
	int pos, dir;
	Plan( int pos=-1, int dir=-1 ) : pos(pos), dir(dir){}
};

struct InfoStepFire{
	int num;                // 消えた数
	int color_num;          // 消えた色数
	std::vector<int> connection; // 連結数
};
struct InfoFire{
	int score;              // 合計得点
	int chain_num;          // 連鎖数
	std::vector<InfoStepFire> step_infos;
};

// いろいろ便利関数

// Planを列挙するとき使う
Plan getPlan( int pattern_id );
// フィールドのx列の高さを返す
int  countHeight( const Puyo::Field &field, int x );
// フィールドにPlanでツモを置く
int  setTumo( Puyo::Field &field, const Puyo::Tumo &tumo, const Plan &plan );
// フィールドをプリントする
void printField( const Puyo::Field &field );
// Fieldが発火可能か
bool canFire( const Puyo::Field &field );
// 発火を1ステップ進める
void stepFire( Puyo::Field &field, InfoStepFire &info );
// 発火して全ステップ進める
void fireField( Puyo::Field &field, InfoFire &info );
// 空中のぷよを設置する
void fallPuyo( Puyo::Field &field );
// 接続されているぷよを消して接続数を返す（隣接したおじゃまも消す）
int deleteConnection( Puyo::Field &field, int x, int y );
// 接続数を数える
int countConnection( const Puyo::Field &field, int x, int y );
