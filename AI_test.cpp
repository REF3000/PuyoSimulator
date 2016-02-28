#include "PuyoAI.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace Puyo;

struct AI_RESULT{
	int pos;
	int dir;
	AI_RESULT( int p=0, int d=0 ) :pos(p), dir(d) {}
};


class PuyoAITest{
public:

	PuyoAITest();

	AI_RESULT getResult( int field[13][6], int next[][2], int next_num );

	void Debug( int field[13][6] );

private:

};



// 定石データベース
vector<string> DB;


// フィールド操作関連 -------------------------------------------------------
typedef char FIELD[15][8];

int get_height( FIELD field, int x ){
	for( int y=13; y>=1; --y )
		if( field[y][x]!=0 ) return y;
	return 0;
}
void print( FIELD field ){
	static int num=0;
	cout<<"print : "<<++num<<endl;
#if 1
	for( int i=0; i<13; ++i ){
		for( int j=0; j<6; ++j ){
			cout << (int)field[13-i][j+1];
		}
		cout<<endl;
	}
#endif
#if 0
	string str = "http://www.inosendo.com/puyo/rensim/??";
	for( int i=0; i<13; ++i ){
		for( int j=0; j<6; ++j ){
			char c='0';
			if( field[13-i][j+1]==1 ) c='4';
			if( field[13-i][j+1]==2 ) c='5';
			if( field[13-i][j+1]==3 ) c='6';
			if( field[13-i][j+1]==4 ) c='7';
			str += c;
		}
	}
	cout<<str<<endl;
#endif
}
int do_count_connection( FIELD field, int x, int y, int color, int flag[15][8] ){
	if( flag[y][x] ) return 0;
	if( field[y][x] != color ) return 0;
	if( y>=13 ) return 0; // 幽霊はカウントしない
	flag[y][x] = 1;
	return do_count_connection( field, x, y+1, color, flag ) +
		   do_count_connection( field, x+1, y, color, flag ) +
		   do_count_connection( field, x, y-1, color, flag ) +
		   do_count_connection( field, x-1, y, color, flag ) + 1 ;
}
int count_connection( FIELD field, int x, int y ){
	if( field[y][x]<1 || 4<field[y][x] ) return 0;
	int flag[15][8]={0};
	return do_count_connection( field, x, y, field[y][x], flag );
}
int do_del_connection( FIELD field, int x, int y, int color ){
	if( field[y][x]!=color ){
		if( field[y][x]==5 ) field[y][x] = 0; // OJAMA
		return 0;
	}
	field[y][x] = 0;
	return do_del_connection( field, x, y+1, color ) +
		   do_del_connection( field, x+1, y, color ) +
		   do_del_connection( field, x, y-1, color ) +
		   do_del_connection( field, x-1, y, color ) + 1 ;
}
int del_connection( FIELD field, int x, int y ){
	return do_del_connection( field, x, y, field[y][x] );
}
void fall( FIELD field ){
	for( int x=1; x<=6; ++x ){
		int h=0;
		for( int y=1; y<=13; ++y ){
			if( field[y][x] ) swap( field[y][x], field[++h][x] );
		}
	}
}
int fire_field( FIELD field ){
	const int A[] = {0,8,16,32,64,96,128,160,192,224,256,288,320,352,384,416,448,480,512};
	const int B[] = {0,3,6,12,24};
	const int C[] = {0,2,3,4,5,6,7,10};

	int score=0;

	for(int cnt=0; cnt<20; cnt++ ){
		// cnt連鎖目の処理
		int num = 0;        // 消えたぷよの数
		int bonas = 0;      // 
		int color[5] = {0}; // 消えた色数

		int flag[15][8] = {0};
		for( int y=1; y<=13; ++y ){
			for( int x=1; x<=6; ++x ){
				if( field[y][x]<1 || 4<field[y][x] ) continue;
				int n = do_count_connection( field, x, y, field[y][x], flag );
				if( n>=4 ){
					color[field[y][x]] = 1;
					del_connection( field, x, y );
					num   += n;
					bonas += C[ n>10 ? 7 : n-4 ];
				}
			}
		}
		if( num==0 ) break;

		int color_num = 0;
		for( int i=0; i<5; ++i ) if( color[i] ) color_num++;
		bonas += A[cnt];
		bonas += B[color_num-1];
		if( bonas==0 ) bonas = 1;
		score += num*10*bonas;

		fall( field );
	}

	return score;
}


// --------------------------------------------------------------------------

// urlをフィールド文字列化
string url_to_string( string str ){
	string buf;
	if( str.size()<39 ) return buf;
	if( str[0]!='h' ) return buf;
	str = str.substr(38);

	char tmp[7]="000000";
	size_t p;
	reverse( str.begin(), str.end() );
	for( p=0; p<str.size(); ++p ){
		char c;
		switch( str[p] ){
		case '0': c='0'; break;
		case '4': c='1'; break;
		case '5': c='2'; break;
		case '6': c='3'; break;
		case '7': c='4'; break;
		default:
			c='0';
		}
		tmp[5-p%6] = c;
		if( (p%6)==5 ){
			buf += tmp;
			memset( tmp,'0',sizeof(tmp) ); tmp[6]='\0';
		}
	}
	if( p%6!=0 )buf += tmp;
	cout<<buf<<endl;

	return buf;
}


PuyoAITest::PuyoAITest(){
	// DB.datを読み込む
	// 読み込んだデータは(1,1),(2,1)...の順で文字列化される
	// 最初に発見したぷよを1とする
	return;
	ifstream ifs;
	ifs.open("DB.dat");
	if( !ifs ) cout<<"DB.datのオープンに失敗"<<endl;
	DB.clear();
	string line;
	while( getline( ifs, line ) ){
		//cout<<line<<endl;
		string buf = url_to_string( line );
		if( buf.size() == 0 ) continue;
		DB.push_back( buf );
	}
}












struct MEMO{
	int x, color;
} search_memo[4]; // どこに何を置いたかメモる
#if 0
int search( int level, FIELD &field ){
	if( level==3 ) return -1;

	for( int x=0; x<6; ++x ){
	for( int c=1; c<=4; ++c ){
	if( field.setPuyo( x, c )+1 ){
		search_memo[ level ].x = x;
		search_memo[ level ].color = c;
		if( field.getChain( x )>=4 ){
			FIELD bak( field );
			//cout<< "Fire!" << endl;
			for( int i=0; i<=level; ++i ){
				;//printf( "[%d] x:%d, color:%d\n", i, search_memo[i].x, search_memo[i].color );
			}
			//bak.fire();
			//bak.print();
		} else {
			search( level+1, field );
		}
		field.delPuyo( x );
	}}}

	return 0;
}
#endif


int compare_books( FIELD field ){

	int max_hit_num = 0;
	int find_num = 0;
	
	// フィールドをstring化する
	string str;
	int max_h=0;
	for( int i=1; i<=6; ++i ) 
		max_h = max( max_h, get_height( field, i ) );
	for( int y=1; y<=max_h; ++y ){
		char buf[] = "000000";
		for( int x=1; x<=6; ++x ){
			buf[x-1] = '0'+field[y][x];
		}
		str+=buf;
	}
	// DBとの比較
	for( size_t i=0; i<DB.size(); ++i ){
		const string dbs = DB[i];
		int color[5] = {0}; //ex:S=1の場所がD=2ならcolor[1] = 2;
		int color_f[5] = {0};
		int hit_num = 0;
		int LIM = min( dbs.size(), str.size() );
		int j;
		for( j=0; j<LIM; ++j ){
			const int S = str[j]-'0';
			const int D = dbs[j]-'0';
			if( D>0 && S>0 ){
				if( color[S]==0 ){ // 新しい色を発見
					if( color_f[D] ) break;
					else color[S] = D, color_f[D]=1;
				}
				if( color[S]!=D ) break;
				hit_num++;
			}
		}
		if( j==LIM ){
			max_hit_num = max( max_hit_num, hit_num );
			find_num ++ ;
		}
	}

	return max_hit_num*10 + find_num;

}

struct ANALYZE{
	int score;
	int pos;
	int dir;
	int dis; // 発火までの遠さ
	ANALYZE( int s, int p, int d, int di=0 ) :score(s), pos(p), dir(d), dis(di) {}
	bool operator< (const ANALYZE &obj) const{
		return score<obj.score;
	}
};
vector<ANALYZE> anal; // *


// フィールドについて解析
int analyze_field( FIELD field ){

	// まずは発火のチェック
	int fire_score = fire_field( field );

	// 窒息
	if( field[12][3] ) return -1;
	
	int value;
	//int book_score = 0; // 定石点
	int shape_score; // フィールド形点


	// 定石形との比較
	//compare_books( field );
	
	// フィールド形の解析
	int height[6];
	int max_height = 0;
	int min_height = 14;
	for( int i=0; i<6; ++i )
		height[i] = get_height( field, i+1 );
	for( int i=0; i<6; ++i ){
		max_height = max( max_height, height[i] ); 
		min_height = min( min_height, height[i] );
	}
	shape_score = 10 - (max_height-min_height);

	// 発火可能性の解析
	//do_analyze_field( field, 0 );
	int max_score = 0;
	for( int x=1; x<=6; ++x ){
		const int y = height[x-1]+1;
		for( int c=1; c<=4; ++c ){
			FIELD tmp;
			memcpy( tmp, field, sizeof(tmp) );
			tmp[y][x] = c;
			if( count_connection( tmp, x, y )>=4 ){
				int s = fire_field( tmp );
				max_score = max( s, max_score );
			}
			tmp[y][x] = 0;
		}
	}
	//!
	// フィールドが狭かったら理想発火の評価を下げる
	int space = 72+6;
	for( int i=0; i<6; ++i ) space -= height[i];
	if( space<20 ) max_score /= 2;



	//value = book_score*100 + shape_score;
	value = max(max_score,fire_score) + shape_score;

#if 1
	//! 準窒息
	for( int i=2; i<=5; ++i ){
		if( field[12][i] ) return -1;
	}
	//! 準窒息
	for( int i=1; i<=6; ++i ){
		if( field[13][i] ) return -1;
	}
	if( field[11][2] ) return -1;
	if( field[11][3] ) return -1;
	if( field[11][4] ) return -1;
	if( field[11][5] ) return -1;
#endif

	return value;
}



int PATTERN[22][4] = { // x1,y1,x2,y2 // 1が軸 2が属 // yは高さとの増分
	{ 1, 1, 1, 2 }, // dir0
	{ 2, 1, 2, 2 },
	{ 3, 1, 3, 2 },
	{ 4, 1, 4, 2 },
	{ 5, 1, 5, 2 },
	{ 6, 1, 6, 2 },
	{ 1, 1, 2, 1 }, // dir1
	{ 2, 1, 3, 1 },
	{ 3, 1, 4, 1 },
	{ 4, 1, 5, 1 },
	{ 5, 1, 6, 1 },
	{ 1, 2, 1, 1 }, // dir2
	{ 2, 2, 2, 1 },
	{ 3, 2, 3, 1 },
	{ 4, 2, 4, 1 },
	{ 5, 2, 5, 1 },
	{ 6, 2, 6, 1 },
	{ 2, 1, 1, 1 }, // dir3
	{ 3, 1, 2, 1 },
	{ 4, 1, 3, 1 },
	{ 5, 1, 4, 1 },
	{ 6, 1, 5, 1 },
};
#define p_dir(x) ( (x<6)?0:(x<11)?1:(x<17)?2:3 )
// 解析:返すのは評価が最大の点数
int history_p[256];
int history_d[256];
int solve( FIELD field, int next[][2], int level, int next_num ){

	const int N1 = next[level][0];
	const int N2 = next[level][1];

	// 落下パターンは22通りある（同色なら11）
	const int LOOP_NUM = (N1==N2) ? 11 : 22;
	for( int i=0; i<LOOP_NUM; ++i ){
		const int POS_X1 = PATTERN[i][0];
		const int POS_Y1 = get_height( field, POS_X1 ) + PATTERN[i][1];
		const int POS_X2 = PATTERN[i][2];
		const int POS_Y2 = get_height( field, POS_X2 ) + PATTERN[i][3];
		if( POS_Y1>13 || POS_Y2>13 ) continue;

		FIELD tmp;
		memcpy( tmp, field, sizeof(FIELD) );

		tmp[POS_Y1][POS_X1] = N1;
		tmp[POS_Y2][POS_X2] = N2;
		history_p[level] = POS_X1;
		history_d[level] = p_dir(i);

		int value = analyze_field( tmp );
		if( value == -1 ) continue;
		anal.push_back( ANALYZE( value, history_p[0], history_d[0] ));
		if( level<next_num ){
			solve( tmp, next, level+1, next_num );
		}
	}

	return 0;
}




AI_RESULT PuyoAITest::getResult( int field[13][6], int next[][2], int next_num ){

	AI_RESULT result;

	FIELD tmp;
	memset( tmp, 7, sizeof(tmp) );
	for( int i=1; i<=13; ++i ){
	for( int j=1; j<=6;  ++j ){
		tmp[i][j] = field[i-1][j-1];
	}}
	print( tmp );
	for( int i=0; i<next_num+1; ++i )
		printf("[%d,%d]\n",next[i][0],next[i][1]);
	anal.clear();
	solve( tmp, next, 0, next_num );
	sort( anal.begin(), anal.end() );
	for( size_t i=anal.size()-5; i<anal.size(); ++i ) cout<<anal[i].score<<","<<anal[i].pos<<","<<anal[i].dir<<endl; //!
	result.dir = anal.back().dir;
	result.pos = anal.back().pos;
	

	return result;

}


Plan AI_test( Field field, Next next ){
	static PuyoAITest obj;

	int fd[13][6];
	int nt[3][2];

	for( int x=1; x<=6; ++x ){
		for( int y=1; y<=13; ++y ){
			fd[y-1][x-1] = field.get(x,y);
		}
	}
	for( int i=0; i<3; ++i ){
		nt[i][0] = next.get(i,0);
		nt[i][1] = next.get(i,1);
	}
	AI_RESULT result = obj.getResult( fd, nt, 2 );

	return Plan( result.pos, result.dir );
}