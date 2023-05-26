#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <arm_neon.h>
#define MAXSIZE 2000
#include <omp.h>
#define NUM_THREADS 4
using namespace std;

class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};
bool operator<(const index& s1, const index& s2)
{
	return s1.len < s2.len; //从小到大排序
}
index s;
index temp;
void sorted(int* list, vector<index>& idx, int num) {
	for (int i = 0; i < num - 1; i++) {
		for (int j = 0; j < num - i - 1; j++) {
			if (idx[list[j]].len > idx[list[j + 1]].len) {
				int tmp = list[j];
				list[j] = list[j + 1];
				list[j + 1] = tmp;
			}
		}
	}
}
index* s0=new index[NUM_THREADS];

vector<index> idx;
vector<unsigned int> S;
vector<unsigned int>& ADP(int* list, vector<index>& idx, int num)
{
	bool found = true;
	vector<index> idx_;
	int i = 0, t = 0, j = 0, k = 0,m=0;
 bool parallel = true;
#pragma omp parallel if(parallel),num_threads(NUM_THREADS),private(i,j,t,k,m)
#pragma omp single
	for ( k = 0; k < num; k++)
	{
		idx_.push_back(idx[list[k]]);
	}
	sort(idx_.begin(), idx_.end());
#pragma omp for
	for (j = 0; j < idx_[0].len; j++)
	{
		unsigned int e = idx_[0].arr[j];
		for (i = 1; i < num; i++) {
			found = false;
			uint32_t length = ceil(idx[list[i]].len / 4) * 4;
			for ( m = idx[list[i]].len ; m < length; m++)
				idx[list[i]].arr[m] = 0;
			for (int t = 0; t < length; t+=4 ) {//��i���б��ĸ���ֵ��s�б��еĵڡ�j]��ֵ�Ƚ�
				found = true;
				unsigned int res[4] = { 0 };
				uint32x4_t temp0 = vmovq_n_u32(e);
				uint32x4_t temp1 = vld1q_u32( &idx[list[i]].arr[t]);
				uint32x4_t result = vceqq_u32(temp0, temp1);
				vst1q_u32(res, result);
				if (res[0]==1||res[1]==1||res[2]==1||res[3]==1) {
					found = false;
					break;
				}
			}
		}
		if (found == true) {
			S.push_back(e);
		}
	}
	return S;
}
int main() {
	//��ȡ�������ļ�
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}

	for (int i = 0; i < 2000; i++) {
		index temp;
		file.read((char*)&temp.len, sizeof(temp.len));
		for (int j = 0; j < (temp.len); j++)
		{
			unsigned int binary_temp;
			file.read((char*)&binary_temp, sizeof(binary_temp));
			temp.arr.push_back(binary_temp);
		}
		idx.push_back(temp);
	}
	file.close();
	/*---------------------------------------------------------------------------------------------------------*/
	file.open("ExpQuery", ios::in);
	int query[1000][5] = { 0 };
	string line;
	int count = 0;
	while (getline(file, line)) {
		stringstream ss; //������
		ss << line; //�����д�ֵ
		int a = 0;
		while (!ss.eof()) {
			int temp;
			ss >> temp;
			query[count][a] = temp;
			a++;
		}
		count++;
	}
	file.close();
	/*-----------------------------------------------------------------------------------------------------------*/


	struct timespec sts, ets;
	timespec_get(&sts, TIME_UTC);
 for(int i=0;i<5;i++){
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];//例子中的数
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - 按表求交-------- -
		ADP(list, idx, num);
		delete list;
	}
	timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec - sts.tv_sec;
	long dnsec = ets.tv_nsec - sts.tv_nsec;
  if(dnsec<0){
   dsec--;
   dnsec+=1000000000ll;
  }
	printf(" % lld. % 09llds\n", dsec, dnsec);
	return 0;
}

