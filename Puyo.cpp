#include "Puyo.h"
#include <cassert>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace Puyo;

Field::Field(){
	init();
}

void Field::init(){
	for( int i=0; i<8; ++i )
	for( int j=0; j<16; ++j )
		field[i][j] = Puyo::BRANK;
	for( int x=0; x<8; ++x ) // 床
		field[x][0] = Puyo::WALL;
	for( int y=0; y<16; ++y ) // 壁（左右）
		field[0][y] = Puyo::WALL,
		field[7][y] = Puyo::WALL;
}

eKIND Field::get( int x, int y ) const {
	assert( x>=0 && x<=7 && y>=0 && y<=15 );
	return field[x][y];
}

void Field::set( int x, int y, eKIND kind ){
	assert( x>=1 && x<=6 && y>=1 && y<=14 );
	assert( kind>=0 && kind<TAIL );
	field[x][y] = kind;
}

//-----------------------------------------
Next::Next(){
	init();
}
void Next::init(){
	static std::random_device rnd;
	// テーブルにぷよをセット
	for( int i=0; i<256; ++i )
		table[i] = (eKIND)(i%4+1);
	// シャッフルする（初手4色はやり直し）
	do{
		//shuffle( table, table+256, rnd ); 使えない悲しみ
		for( int i=255; i>0; --i ) swap( table[i], table[rnd()%(i+1)] );
		unsigned char flag=0;
		for( int i=0; i<4; ++i ) flag|=(1<<(table[i]-1));
		if( flag!=0x000f ) break;
	}while(1);
}

Tumo Next::get( int turn ){
	const int n = turn%128;
	return make_pair( table[n*2], table[n*2+1] );
}
eKIND Next::get( int turn, int axis_sub ){
	const int n = turn%128;
	return table[n*2+axis_sub];
}
void Next::set( int turn, int axis_sub, eKIND kind ){
	assert( turn>=0 && turn<=127 );
	assert( axis_sub==0 || axis_sub==1 );
	table[(turn%128)*2+axis_sub] = kind;
}
void Next::set( int turn, Tumo tumo ){
	assert( turn>=0 && turn<=127 );
	table[(turn%128)*2]   = tumo.first;
	table[(turn%128)*2+1] = tumo.second;
}









