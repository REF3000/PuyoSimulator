/************************************************************************/
/*                                                                      */
/*      とこぷよAI                                                      */
/*      パターンマッチのテスト                                    */
/*                                                                      */
/************************************************************************/
#include "PuyoAI.h"
#include <iostream>
//using namespace Puyo;
using namespace std;

//--------------------------------------------------
typedef std::pair<int,int> Tumo;

struct Action{
	int id;
	int pos;
	int dir;
	Action( int id=-1, int pos=0, int dir=0 ){
		this->id = id;
		this->pos = pos;
		this->dir = dir;
	}
};

struct Field{
	static const int H = 13;
	static const int W = 6;
	int field[W][H];
	Field(){
		init();
	}
	void init(){
		for( int i=0; i<W; ++i )
			for( int j=0; j<H; ++j )
				field[i][j]=0;
	}
	int get( int x, int y ){
		if( x>=1 && x<=6 && y==14 ) return 0;
		if( x<1 || x>6 || y<1 || y>13 ) return -1;
		return field[x-1][y-1];
	}
	int set( int x, int y, int puyo ){
		if( x<1 || x>6 || y<1 || y>13 ) return -1;
		field[x-1][y-1] = puyo;
		return 0;
	}
	int set( int x, int puyo ){
		for( int y=1; y<=13; ++y ){
			if( get(x,y)==0 ){
				set(x,y,puyo);
				return y;
			}
		}
		return -1;
	}
	int set( Action act, Tumo tumo ){
		if( act.id==0 ) return 0;
		int fx = act.pos;
		int sx = act.pos+(act.dir==1?1:(act.dir==3?-1:0));
		int y;
		if( act.dir==2 ){
			    set( sx, tumo.second );
			y = set( fx, tumo.first );
		} else {
			y = set( fx, tumo.first );
			    set( sx, tumo.second );
		}
		return 13-y;
	}
private:
	int doCountConnection( int x, int y, int puyo, int flag[W][H] );
	int doDeleteConnection( int x, int y, int puyo );
public:
	int countConnection( int x, int y ){
		const int puyo = get(x,y);
		if( !(1<=puyo && puyo<=4) ) return 0;
		int flag[W][H] = {0};
		return doCountConnection( x, y, puyo, flag );
	}
	int deleteConnection( int x, int y ){
		const int puyo = get(x,y);
		if( !(1<=puyo && puyo<=4) ) return 0;
		return doDeleteConnection( x, y, puyo );
	}
	int fall(){
		int fallen_flag = 0;
		for( int x=1; x<=6; ++x ){
			std::vector<int> tmp;
			for( int y=1; y<=13; ++y ) if(get(x,y)) tmp.push_back(get(x,y));
			for( int y=1; y<=13; ++y ) set(x,y,0), fallen_flag=1;
			for( size_t i=0; i<tmp.size(); ++i) set( x,i+1,tmp[i] );
		}
		return fallen_flag;
	}
};

struct Next{
	static const int SIZE = 128;
	Tumo next[SIZE];
	Next(){
		std::random_device rnd;
		for( int i=0; i<SIZE; ++i ){
			std::pair<int,int> tumo;
			tumo.first  = ((size_t)rnd())%4 + 1;
			tumo.second = ((size_t)rnd())%4 + 1;
			next[i] = tumo;
		}
	}
	Next( char array_byte[] ){
		for( int i=0; i<SIZE; ++i ){
			std::pair<int,int> tumo;
			tumo.first  = array_byte[i*2];
			tumo.second = array_byte[i*2+1];
			next[i] = tumo;
		}
	}
	Tumo get( int turn ){
		return next[turn%SIZE];
	}
};


#include <fstream>
#include <string>
#include <iostream>
#include <map>
using namespace std;
#define ACT(pos,dir) (Action(1,(pos),(dir)))

//--------------------------------------------------
bool isSyote( Field &field );
Action think_syote( Field &field, vector<Tumo> &next );

Action think( Field field, vector<::Tumo> next ){

	if( isSyote(field) ) return think_syote( field, next );
	


	return ACT(1,0);
}

bool isSyote( Field &field ){
	for( int y=1; y<=12; ++y )
		for( int x=1; x<=6; ++x )
			if( field.get(x,y)!=0 ) return false;
	return true;
}

Action think_syote( Field &field, vector<Tumo> &next ){
	Tumo t1 = next[0];
	Tumo t2 = next[1];
	// 同色か？
	if( t1.first == t1.second ){
		if( t2.first == t2.second ) return ACT(2,1); // AABB
		if( t1.first!=t2.first && t1.first!=t2.second ) return ACT(1,0); // AABC 
		return ACT(1,1); // AAAB
	}
	// 次が同色か？
	if( t2.first == t2.second ){
		if( t1.first  == t2.first ) return ACT(3,0); // ABAA
		if( t1.second == t2.first ) return ACT(3,2); // BAAA
		return ACT(2,2); // BCAA
	}
	// 以下異色
	if( t1.first==t2.first  && t1.second==t2.second ||
		t1.first==t2.second && t1.second==t1.first ) return ACT(2,0); // ABAB
	if( t1.first == t2.first || t1.first == t2.second ) return ACT(1,1); // ABAC
	return ACT(2,3); // ABCD
}

struct PatternFile{
	map<int,int> color_map;
	char raw[13][6];
};
typedef PatternFile PTF;

int loadPatternFile( PTF &ptf, const char *path ){
	ifstream ifs( path );
	if( !ifs ) return -1;
	string buf;
	map<char,int> table;
	table['0'] = 0;
	int cnt=0;
	int line=0;
	for( ; ifs>>buf; ++line ){
		for( size_t i=0; i<buf.size(); ++i ){
			char c = buf[i];
			ptf.raw[line][i] = c;
			if( c=='0' ) continue;
			if( table.find(c)!=table.end() ) continue;
			table[c] = ++cnt;
		}
	}
	for( int i=0; i<13; ++i ){
		for( int j=0; j<6; ++j ){
			ptf.raw[i][j] = table[ptf.raw[i][j]];
		}
	}
	
	return 0;
}

void makeAllPattern( vector<Field> &fields, PTF &ptf ){
	


}

//---------------------------------------------

Plan AI_pattern_match( Puyo::Field field, Puyo::Next next ){
	vector<Tumo> ne;
	ne.push_back( next.get(0) );
	ne.push_back( next.get(1) );
	ne.push_back( next.get(2) );
	Field fd;
	for( int x=1; x<=6; ++x )
		for( int y=1; y<=13; ++y )
			fd.set(x,y, field.get(x,y) );
	Action act = think( fd, ne );
	return Plan( act.pos, act.dir );
}

//-------------

void pm_test( Puyo::Field field, Puyo::Next next ){

	//printField( field );

	PTF ptf;
	loadPatternFile( ptf, "./data/gtr.txt" );

	vector<Field> patterns;
	makeAllPattern( patterns, ptf );



	for( int i=0; i<13; ++i ){
		for( int j=0; j<6; ++j ){
			cout<<(int)ptf.raw[i][j];
		}
		cout<<endl;
	}

}
