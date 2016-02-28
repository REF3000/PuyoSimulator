/************************************************************************/
/*                                                                      */
/*      DxLibでコンボボックス                                           */
/*                                                                      */
/*  REF 2015/11/21                                                      */
/*                                                                      */
/************************************************************************/
#include <vector>
#include <string>

class ComboBoxDx{
	static const int LINE_H = 20;
	static const int LINE_W = 180;
	int LEFT, TOP; // 左上座標
	int OPEN_TOP;  // 展開部の左上座標
	std::vector<std::string> element_names;
	int size;
	int select_pos;
	bool mouse_over_flag;   // 表示部にマウスオーバーしているか
	bool drop_down_flag;    // ドロップダウンしているか
	int mouse_over_num;     // 展開部のどこにマウスオーバーしているか していない:-1

	void click();

public :
	ComboBoxDx( int left=0, int top=0 ); // 配置する左上座標が引数
	void setElements( std::vector<std::string> elements ); // vector<string>で要素を渡す
	void update();          // 毎フレーム呼ぶ
	void draw();            // 描画したいタイミングで呼ぶ
	int  getSelectPos();    // 選択されているidを返す
};