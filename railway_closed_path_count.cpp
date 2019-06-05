#include "stdafx.h"

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <set>

#define CONST_R 0
#define CONST_S 1
#define CONST_L 2

#define RAIL_NUM 22 // 使用するレール数

// 重複を除外した数え上げ
#define UNIQUE_COUNT

using namespace std;

//**********************************************************************************


// 進入角度ごとの座標値増分table（x座標整数部, x座標の1/√2係数、y座標整数部、y座標の1/√2係数, 角度インデックス）
// 角度インデックスは 3時の方向を0として、45度刻みで反時計回り
int railMap[3][8][5] = {
// R
{
  { 0, 1,-1, 1,-1}, // 0 
  { 0, 1, 1,-1,-1}, // 45
  { 1,-1, 0, 1,-1}, // 90
  {-1, 1, 0, 1,-1}, //135
  { 0,-1, 1,-1,-1}, //180 
  { 0,-1,-1, 1,-1}, //225
  {-1, 1, 0,-1,-1}, //270
  { 1,-1, 0,-1,-1}, //315
},
//S
{
  { 1, 0, 0, 0, 0}, // 0 
  { 0, 1, 0, 1, 0}, // 45
  { 0, 0, 1, 0, 0}, // 90
  { 0,-1, 0, 1, 0}, //135
  {-1, 0, 0, 0, 0}, //180 
  { 0,-1, 0,-1, 0}, //225
  { 0, 0,-1, 0, 0}, //270
  { 0, 1, 0,-1, 0}, //315
},
// L
{
  { 0, 1, 1,-1,1}, // 0 
  { 1,-1, 0, 1,1}, // 45
  {-1, 1, 0, 1,1}, // 90
  { 0,-1, 1,-1,1}, //135
  { 0,-1,-1, 1,1}, //180 
  {-1, 1, 0,-1,1}, //225
  { 1,-1, 0,-1,1}, //270
  { 0, 1,-1, 1,1}, //315
}
};

class RailElement {
public:
	int type; // R,S,Lの区別

	// レール端点座標を(v[0]+v[1]/√2, v[2]+[v3]/√2)と表して, 進行角度を v[4] で表わす
	int v[5];
	
	RailElement() {
		memset( &v[0] , 0x00 , sizeof(int)*5 );
	}
};

class Tree {
public:
	RailElement railArray[RAIL_NUM];
	int size; // 現在の長さ
	int typeCount[3]; // RとSとLをそれぞれいくつずつ使っているかの記録

	Tree() {
		size = 0;
		memset( &typeCount[0] , 0x00 , sizeof(int)*3 );
	}

	void append(int type) {
		size++;
		railArray[size-1].type = type;
		typeCount[type]++;

		// 終点座標を更新する
		if(size == 1) {
			for(int i = 0; i < 5; i++) {
				railArray[size-1].v[i] = railMap[type][0][i];
			}
		} else {
			for(int i = 0; i < 5; i++) {
				railArray[size-1].v[i] = railArray[size-2].v[i] + railMap[type][railArray[size-2].v[4]][i];
			}
		}

		// 終端角度は8の剰余
		railArray[size-1].v[4] = (railArray[size-1].v[4] + 8) % 8;
	}
	
	void deleteLast() {
		typeCount[railArray[size-1].type]--;
		size--;
	}

	void debugOut() {
		for(int i = 0; i < size; i++) {
			switch(railArray[i].type) {
			case CONST_R: printf("R"); break;
			case CONST_S: printf("S"); break;
			case CONST_L: printf("L"); break;
			}
		}
		printf("\n");
	}

	bool isClosed() {
		return (railArray[size-1].v[0] == 0 && 
			railArray[size-1].v[1] == 0 &&
			railArray[size-1].v[2] == 0 &&
			railArray[size-1].v[3] == 0 &&
			railArray[size-1].v[4] == 0);
	}

	// 枝刈のための判定（もう閉じることは不可能な状態であるかを判定）
	bool impossibleToReturn() {

/*		return 
		abs(endCoordinate.v[0]) + 
		abs(endCoordinate.v[1]) +
		abs(endCoordinate.v[2]) +
		abs(endCoordinate.v[3]) > (RAIL_NUM - size)*3;
	*/	

		if( (railArray[size-1].v[0] + railArray[size-1].v[1] *0.707) * 
			(railArray[size-1].v[0] + railArray[size-1].v[1] * 0.707) +
			(railArray[size-1].v[2] + railArray[size-1].v[3] * 0.707) *
			(railArray[size-1].v[2] + railArray[size-1].v[3] * 0.707) >
			(RAIL_NUM - size) * (RAIL_NUM - size)){
				return true;
		}

		// 他のアイデア
		// Lの数とRの数の差は8の倍数でないといけないので、
		// 残りが8より少ない n のときに、 Lの数とRの数の差の mod 8 が 8-n よりも大きいと無理
		// 例： Rが8 Lが3 のとき R3か、L5にしないといけないので、残りが2だと無理
		// RとLの数を毎回数えるわけにはいかないので、記録しておかないとならない
		// 直線は偶数必要であることの考慮
		int remainRailNum = RAIL_NUM - size - typeCount[CONST_S]%2;
		if( remainRailNum < 8 &&
			abs(typeCount[CONST_R] - typeCount[CONST_L]) % 8 > remainRailNum &&
			8-(abs(typeCount[CONST_R] - typeCount[CONST_L]) % 8) > remainRailNum 			
			) {
			return true;
		}

		return false;
	}

	// 枝刈に使用 重複するものが存在するか途中段階で判断する
	bool notUnique() {
		// Lが過半数（厳密には、総数-直線数の過半）を占めたら、必ずRにひっくり返されるので、Rが先頭となる文字列になって、
		// これは総当たりで必ず出てくるものだから、重複するから対象外にできる
		// 重複後のカウントには有効。重複無視には適用できない
		if(typeCount[CONST_L] > (RAIL_NUM - typeCount[CONST_S]) / 2) {
			return true;
		}

		// 先頭から順番に見て行って、後で必ずローテーションさせられるものは除外
		// RLRR ← 必ず RRが先頭に来る 仮にLがたくさんでひっくり返ったとしても LRLL になって、ローテーションさせられる
		// RRLRRR ← これもそう。
		// ようするに、最初に連続するRの個数よりも長くRが連続するものが現れたときが対象となる ← 後ろから見ていく

		// 最低でも4つ以上必要
		if(size < 4) return false;

		// 最低でも後ろ2つはR
		if(railArray[size-1].type != CONST_R) return false;
		if(railArray[size-2].type != CONST_R) return false;

		// 最初にRがいくつ続くか
		int firstRnum = 0;
		for(int i = 0; i < size; i++) {
			if(railArray[i].type == CONST_R) {
				firstRnum++;
			} else {
				break;
			}
		}

		if(firstRnum == size) return false; // 全部Rだったら

		// 後ろからRを探す
		int tailRnum = 0;
		for(int i = size-1; i >= 0; i--) {
			if(railArray[i].type == CONST_R) {
				tailRnum++;
				if(tailRnum > firstRnum) return true;
			} else {
				break;
			}
		}

		return false;
	}

	// original の方が target より小さければ true を返す
	bool smallerThan(const int *original, const int *target) {
		for(int i = 0; i < size; i++) {
			if(original[i] < target[i]) {
				return true;
			}
			if(original[i] > target[i]) {
				return false;
			}
		}

		// 同じことはありえないけど
		return false;
	}

	// out の配列をローテーションさせて最小になるものを minArr に格納する
	// 速いと思ったけどそうでもなかった
/*	void getMinArrByRotation(int *minArr, const int *out) {
//		for(int i = 0; i < size; i++) {
//			minArr[i] = out[i];
//		}
		
//		memcpy(minArr, out, sizeof(int) * RAIL_NUM); 

		int minPibot = 0;
		for(int pibot = 1; pibot < size; pibot++) {
			bool bSmaller = false;
			for(int i = 0; i < size; i++) {
				int index = (i + pibot ) % size;
				int minIndex = (i + minPibot) % size; 
				if(out[minIndex] < out[index]) {
					break;
				}
				if(out[minIndex] > out[index]) {
					minPibot = pibot;
		//			bSmaller = true;
					break;
				}
			}
		}

	//	if(minPibot != 0) {
				for(int i = 0; i < size; i++) {
					int index = (i + minPibot ) % size;
					minArr[i] = out[index];
				}
	//	}

	}
	*/
	void getMinArrByRotation(int *minArr, const int *out) {

		// ローテーションしなくて済むように2倍のサイズで
		int buff[RAIL_NUM*2];
		memcpy(buff, out, sizeof(int) * RAIL_NUM); 
		memcpy(&buff[RAIL_NUM], out, sizeof(int) * RAIL_NUM); 

		int minPibot = 0;
		for(int pibot = 1; pibot < size; pibot++) {
			for(int i = 0; i < size; i++) {
				if(buff[i + minPibot] < buff[i + pibot]) {
					break;
				}
				if(buff[i + minPibot] > buff[i + pibot]) {
					minPibot = pibot;
					break;
				}
			}
		}

		for(int i = 0; i < size; i++) {
			minArr[i] = buff[i + minPibot];
		}
	}

	void reverseLR(int *buff) {
		for(int i = 0; i < size; i++) {
			if(buff[i] == CONST_R) {
				buff[i] = CONST_L;
			} else if(buff[i] == CONST_L) {
				buff[i] = CONST_R;
			}
		}
	}

	// 正規化処理

	// 形が一緒で、回転方向が逆のものを同一視する
    	// RとLの数を比較して、Lが多ければ、RとLを反転する ← LとRが同じ数の時はどうするか ← サイクルさせて小さくなる方を採用
	    // サイクルさせて、最も小さい値になるようにする
    	// R < S < L

	// 鏡像を除外する
	//  並び順を反転させてから上記と同じ処理。
	// 鏡像にする前と、比較して小さい方を採用する

	void rotationNormalize(int *buff) {
		int num_r = 0;
		int num_l = 0;

		// RとLの数をカウント
		for(int i = 0; i < size; i++) {
			if(railArray[i].type == CONST_R) {
				num_r++;
			} else if(railArray[i].type == CONST_L) {
				num_l++;
			}
		}

		// L の方が多ければ L と R の反転
		if(num_l > num_r) reverseLR(buff);

		// サイクルさせて最も値が小さくなるようにする
		int minArr[RAIL_NUM];
		getMinArrByRotation(minArr, buff);

		// L と R が同じ数の場合は、試しに反転させて、より小さくなるか確認
		if(num_l != num_r) {
			memcpy(buff, minArr, sizeof(int) * RAIL_NUM); 
		} else {
			int minArr2[RAIL_NUM];

			reverseLR(buff);
			getMinArrByRotation(minArr2, buff);

			// 小さいものを採用
			if(smallerThan(minArr, minArr2)) {
				memcpy(buff, minArr, sizeof(int) * RAIL_NUM); 
			} else {
				memcpy(buff, minArr2, sizeof(int) * RAIL_NUM); 
			}
		}
	}

	void getNormalizedArr(int *out) {

		int buff[RAIL_NUM];
		for(int i = 0; i < size; i++) {
			buff[i] = railArray[i].type;
		}

		rotationNormalize(buff);


		int buff2[RAIL_NUM];
		// 並び順を逆にした場合のものについても同様に処理する
		for(int i = 0; i < size; i++) {
			buff2[i] = railArray[size-1-i].type;
		}

		rotationNormalize(buff2);

		// 正順と逆順で小さいものを採用
		if(smallerThan(buff, buff2)) {
			memcpy(out, buff, sizeof(int) * RAIL_NUM); 
		} else {
			memcpy(out, buff2, sizeof(int) * RAIL_NUM); 
		}
	}
};


Tree *tree;
long count_all;
long count_r;
long count_s;
long check_count;

char railLetter[] = {'R', 'S', 'L'};

class Answer {
public:
	char str[RAIL_NUM+1];

	Answer() {
		str[RAIL_NUM] = 0;
	}

	bool operator<(const Answer &right) const {return strcmp(str, right.str) < 0;}
};

// set を使って解を格納
std::set<Answer> answer;

void addAnswer() {
	int arr[RAIL_NUM];
	tree->getNormalizedArr(arr);

	Answer ans;
	for(int i = 0; i < tree->size; i++) {
		ans.str[i] = railLetter[arr[i]];
	}
	answer.insert(ans);
}

void depthFirstSearch(int type) {
	tree->append(type);

	// この先どうしても戻ってこれないなら終了
	if(tree->size > RAIL_NUM/2 && tree->impossibleToReturn()) {
		return;
	}

#ifdef UNIQUE_COUNT
	if(tree->notUnique()) {
		return;
	}
#endif

	if(tree->size == RAIL_NUM) {
		check_count++;
		if(tree->isClosed()) {
			count_all++;
			//printf("%d:", count_all);
			//	tree->debugOut();

#ifdef UNIQUE_COUNT
			addAnswer();
#endif
		}
		return;
	}
	
	depthFirstSearch(CONST_R);
	tree->deleteLast();
	
	depthFirstSearch(CONST_S);
	tree->deleteLast();
	
	depthFirstSearch(CONST_L);
	tree->deleteLast();
}

void startSearch() {
	check_count = 0;
	count_all = 0;
	tree = new Tree();

	depthFirstSearch(CONST_R);
	tree->deleteLast();

	count_r = count_all;

#ifndef UNIQUE_COUNT
	// 重複除外の場合、直線で始まるものは必ずRで始まるものに含まれるので除外
	depthFirstSearch(CONST_S);
	tree->deleteLast();
	count_s = count_all - count_r;
#endif

	// L で開始するものは、Rで始まる逆向きの同形が必ず存在するので、調べなくてよい
    //	depthFirstSearch(CONST_L);
    //	tree->deleteLast();
}


int _tmain(int argc, _TCHAR* argv[]) {
	long start = time(NULL);
	printf("start %d\n", RAIL_NUM);
	startSearch();

	// 出力
//	 for(auto itr = answer.begin(); itr != answer.end(); ++itr) {
//		 printf("%s \n", (*itr).str);
 //   }
	printf("calc time = %ld \n", time(NULL) - start);

#ifndef UNIQUE_COUNT
	printf("total = %ld \n", count_r + count_s + count_r);
#endif
	 printf("unique answer=%ld\n", answer.size());
	 printf("check_count = %ld\n", check_count);

	 return 0;
}

