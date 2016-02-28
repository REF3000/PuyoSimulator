/************************************************************************/
/*                                                                      */
/*      DxLibでボタン                                                   */
/*                                                                      */
/*  REF 2015/11/25                                                      */
/*                                                                      */
/************************************************************************/
#include <vector>
#include <string>

class ButtonDx{
	static const int H = 15;
	static const int W = 15;
	int LEFT, TOP; // 左上座標
	bool mouse_over_flag;   // 表示部にマウスオーバーしているか
	bool press_down_flag;   // 押されているか
	bool press_down_first_flag; // 押され始めの1回だけtrue

	int id;        // +ボタンか-ボタンかを表す 良くない実装

	void press();

public :
	ButtonDx( int left=0, int top=0 , int id=0 ); // 配置する左上座標が引数
	void update();      // 毎フレーム呼ぶ
	void draw();        // 描画したいタイミングで呼ぶ
	int  getState();    // 押され始めの1フレームだけtrue
};