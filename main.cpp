/************************************************************************/
/*                                                                      */
/*      とこぷよシミュレータ ver1.0                                     */
/*                                                                      */
/*  REF 2015/11/28                                                      */
/*                                                                      */
/************************************************************************/
#define GLOBAL_MAIN
#include "DxLib.h"
#include "input.h"
#include "Fps.h"
#include "SceneSimu.h"
#include <iostream>
using namespace std;
/*----------------------------------------------------------------------*/
/*      ループで必ず行う処理                                            */
/*----------------------------------------------------------------------*/
int ProcessLoop(){
    if(ProcessMessage()!=0)return -1;//プロセス処理がエラーなら-1を返す
    if(ClearDrawScreen()!=0)return -1;//画面クリア処理がエラーなら-1を返す
    UpdateInputInfo();//現在のinput情報の更新を行う
    return 0;
}
/*----------------------------------------------------------------------*/
/*      Fps調整含むScreenFlip                                           */
/*----------------------------------------------------------------------*/
int ScreenFlip_2(){
	static Fps fps;

	fps.Update();
	DrawFormatString(0, 0, GetColor(255,255,255), "%.1f", fps.GetFps() );
	ScreenFlip();
	fps.Wait();
    return 0;
}

/*----------------------------------------------------------------------*/
/*      main                                                            */
/*----------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow){
	//SetDoubleStartValidFlag( TRUE ) ; // 多重起動を許可
	SetAlwaysRunFlag( TRUE ) ; // 非アクティブでも処理をする
	ChangeWindowMode(TRUE); //ウィンドウモード
	SetMainWindowText( "とこぷよAI Simulator" ) ;
	//デバッグ用にコンソールを呼び出す
	FILE *stream;
	{
		// TODO:解決策模索 多分AllocConsole()の前にcoutとか出力しちゃうとバグる
        AllocConsole();
        freopen_s( &stream, "CONOUT$", "w", stdout);
        //freopen("debug.txt", "w", stdout); 
        //freopen("CONIN$", "r", stdin);
	}
    if(DxLib_Init() == -1 || SetDrawScreen( DX_SCREEN_BACK )!=0) return -1;//初期化と裏画面化
	SetDragFileValidFlag(TRUE); // D&Dを許可
	
	SceneSimuInit();
	while( ProcessLoop()==0 ){ //メインループ
		SceneSimuUpdate();
		SceneSimuDraw();
		ScreenFlip_2(); //裏画面を反映して待機
	}

	fclose( stream );
	FreeConsole();
    DxLib_End();
    return 0;
}

