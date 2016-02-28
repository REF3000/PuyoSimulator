#pragma once
#include <utility>
#include <random>
#include <vector>
namespace Puyo{

enum eKIND{
	BRANK,
	RED, BLUE, YELLOW, GREEN, PURPLE,
	OJAMA,
	WALL,

	TAIL
};

typedef std::pair<eKIND,eKIND> Tumo;

//-------------------------------
class Field{
	eKIND field[8][16]; // (1,1)Ç™ç∂â∫ äOë§Ç…î‘ï∫Ç‡Ç¢ÇÈÇÊ
public :
	Field();
	void init();
	eKIND get( int x, int y ) const;
	void set( int x, int y, eKIND kind );
};


//--------------------------------
class Next{
	eKIND table[256];
public :
	Next();
	void init();
	Tumo  get( int turn );
	eKIND get( int turn, int axis_sub );
	void  set( int turn, int axis_sub, eKIND kind );
	void  set( int turn, Tumo tumo );
};



};