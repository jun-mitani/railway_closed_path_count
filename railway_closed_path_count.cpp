#include "stdafx.h"

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <set>

#define CONST_R 0
#define CONST_S 1
#define CONST_L 2

#define RAIL_NUM 22 // �g�p���郌�[����

// �d�������O���������グ
#define UNIQUE_COUNT

using namespace std;

//**********************************************************************************


// �i���p�x���Ƃ̍��W�l����table�ix���W������, x���W��1/��2�W���Ay���W�������Ay���W��1/��2�W��, �p�x�C���f�b�N�X�j
// �p�x�C���f�b�N�X�� 3���̕�����0�Ƃ��āA45�x���݂Ŕ����v���
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
	int type; // R,S,L�̋��

	// ���[���[�_���W��(v[0]+v[1]/��2, v[2]+[v3]/��2)�ƕ\����, �i�s�p�x�� v[4] �ŕ\�킷
	int v[5];
	
	RailElement() {
		memset( &v[0] , 0x00 , sizeof(int)*5 );
	}
};

class Tree {
public:
	RailElement railArray[RAIL_NUM];
	int size; // ���݂̒���
	int typeCount[3]; // R��S��L�����ꂼ�ꂢ�����g���Ă��邩�̋L�^

	Tree() {
		size = 0;
		memset( &typeCount[0] , 0x00 , sizeof(int)*3 );
	}

	void append(int type) {
		size++;
		railArray[size-1].type = type;
		typeCount[type]++;

		// �I�_���W���X�V����
		if(size == 1) {
			for(int i = 0; i < 5; i++) {
				railArray[size-1].v[i] = railMap[type][0][i];
			}
		} else {
			for(int i = 0; i < 5; i++) {
				railArray[size-1].v[i] = railArray[size-2].v[i] + railMap[type][railArray[size-2].v[4]][i];
			}
		}

		// �I�[�p�x��8�̏�]
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

	// �}���̂��߂̔���i�������邱�Ƃ͕s�\�ȏ�Ԃł��邩�𔻒�j
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

		// ���̃A�C�f�A
		// L�̐���R�̐��̍���8�̔{���łȂ��Ƃ����Ȃ��̂ŁA
		// �c�肪8��菭�Ȃ� n �̂Ƃ��ɁA L�̐���R�̐��̍��� mod 8 �� 8-n �����傫���Ɩ���
		// ��F R��8 L��3 �̂Ƃ� R3���AL5�ɂ��Ȃ��Ƃ����Ȃ��̂ŁA�c�肪2���Ɩ���
		// R��L�̐��𖈉񐔂���킯�ɂ͂����Ȃ��̂ŁA�L�^���Ă����Ȃ��ƂȂ�Ȃ�
		// �����͋����K�v�ł��邱�Ƃ̍l��
		int remainRailNum = RAIL_NUM - size - typeCount[CONST_S]%2;
		if( remainRailNum < 8 &&
			abs(typeCount[CONST_R] - typeCount[CONST_L]) % 8 > remainRailNum &&
			8-(abs(typeCount[CONST_R] - typeCount[CONST_L]) % 8) > remainRailNum 			
			) {
			return true;
		}

		return false;
	}

	// �}���Ɏg�p �d��������̂����݂��邩�r���i�K�Ŕ��f����
	bool notUnique() {
		// L���ߔ����i�����ɂ́A����-�������̉ߔ��j���߂���A�K��R�ɂЂ�����Ԃ����̂ŁAR���擪�ƂȂ镶����ɂȂ��āA
		// ����͑�������ŕK���o�Ă�����̂�����A�d�����邩��ΏۊO�ɂł���
		// �d����̃J�E���g�ɂ͗L���B�d�������ɂ͓K�p�ł��Ȃ�
		if(typeCount[CONST_L] > (RAIL_NUM - typeCount[CONST_S]) / 2) {
			return true;
		}

		// �擪���珇�ԂɌ��čs���āA��ŕK�����[�e�[�V��������������̂͏��O
		// RLRR �� �K�� RR���擪�ɗ��� ����L����������łЂ�����Ԃ����Ƃ��Ă� LRLL �ɂȂ��āA���[�e�[�V������������
		// RRLRRR �� ����������B
		// �悤����ɁA�ŏ��ɘA������R�̌���������R���A��������̂����ꂽ�Ƃ����ΏۂƂȂ� �� ��납�猩�Ă���

		// �Œ�ł�4�ȏ�K�v
		if(size < 4) return false;

		// �Œ�ł����2��R
		if(railArray[size-1].type != CONST_R) return false;
		if(railArray[size-2].type != CONST_R) return false;

		// �ŏ���R������������
		int firstRnum = 0;
		for(int i = 0; i < size; i++) {
			if(railArray[i].type == CONST_R) {
				firstRnum++;
			} else {
				break;
			}
		}

		if(firstRnum == size) return false; // �S��R��������

		// ��납��R��T��
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

	// original �̕��� target ��菬������� true ��Ԃ�
	bool smallerThan(const int *original, const int *target) {
		for(int i = 0; i < size; i++) {
			if(original[i] < target[i]) {
				return true;
			}
			if(original[i] > target[i]) {
				return false;
			}
		}

		// �������Ƃ͂��肦�Ȃ�����
		return false;
	}

	// out �̔z������[�e�[�V���������čŏ��ɂȂ���̂� minArr �Ɋi�[����
	// �����Ǝv�������ǂ����ł��Ȃ�����
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

		// ���[�e�[�V�������Ȃ��čςނ悤��2�{�̃T�C�Y��
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

	// ���K������

	// �`���ꏏ�ŁA��]�������t�̂��̂𓯈ꎋ����
    	// R��L�̐����r���āAL��������΁AR��L�𔽓]���� �� L��R���������̎��͂ǂ����邩 �� �T�C�N�������ď������Ȃ�����̗p
	    // �T�C�N�������āA�ł��������l�ɂȂ�悤�ɂ���
    	// R < S < L

	// ���������O����
	//  ���я��𔽓]�����Ă����L�Ɠ��������B
	// �����ɂ���O�ƁA��r���ď����������̗p����

	void rotationNormalize(int *buff) {
		int num_r = 0;
		int num_l = 0;

		// R��L�̐����J�E���g
		for(int i = 0; i < size; i++) {
			if(railArray[i].type == CONST_R) {
				num_r++;
			} else if(railArray[i].type == CONST_L) {
				num_l++;
			}
		}

		// L �̕���������� L �� R �̔��]
		if(num_l > num_r) reverseLR(buff);

		// �T�C�N�������čł��l���������Ȃ�悤�ɂ���
		int minArr[RAIL_NUM];
		getMinArrByRotation(minArr, buff);

		// L �� R ���������̏ꍇ�́A�����ɔ��]�����āA��菬�����Ȃ邩�m�F
		if(num_l != num_r) {
			memcpy(buff, minArr, sizeof(int) * RAIL_NUM); 
		} else {
			int minArr2[RAIL_NUM];

			reverseLR(buff);
			getMinArrByRotation(minArr2, buff);

			// ���������̂��̗p
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
		// ���я����t�ɂ����ꍇ�̂��̂ɂ��Ă����l�ɏ�������
		for(int i = 0; i < size; i++) {
			buff2[i] = railArray[size-1-i].type;
		}

		rotationNormalize(buff2);

		// �����Ƌt���ŏ��������̂��̗p
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

// set ���g���ĉ����i�[
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

	// ���̐�ǂ����Ă��߂��Ă���Ȃ��Ȃ�I��
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
	// �d�����O�̏ꍇ�A�����Ŏn�܂���͕̂K��R�Ŏn�܂���̂Ɋ܂܂��̂ŏ��O
	depthFirstSearch(CONST_S);
	tree->deleteLast();
	count_s = count_all - count_r;
#endif

	// L �ŊJ�n������̂́AR�Ŏn�܂�t�����̓��`���K�����݂���̂ŁA���ׂȂ��Ă悢
    //	depthFirstSearch(CONST_L);
    //	tree->deleteLast();
}


int _tmain(int argc, _TCHAR* argv[]) {
	long start = time(NULL);
	printf("start %d\n", RAIL_NUM);
	startSearch();

	// �o��
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

