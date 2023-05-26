#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <time.h>
#include <sstream>
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
	int i = 0, t = 0, j = 0, k = 0;
#pragma omp parallel if(parallel),num_threads(NUM_THREADS),private(i,j,t,k);
#pragma omp single
	for ( k = 0; k < num; k++)
	{
		idx_.push_back(idx[list[k]]);
	}
	sort(idx_.begin(), idx_.end());
#pragma omp for
	for (t = 0; t < idx_[0].len; t++)
	{
		unsigned int e = idx_[0].arr[t];
		for (i = 1; i < num; i++) {
			found = false;
			for (j = 0; j < idx_[i].len; j++) {
				if (idx_[i].arr[j] > e)
					break;
				if (idx_[i].arr[j] == e) {
					found = true;
					break;
				}
			}
			if (found == false) {
				break;
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


	//struct timespec sts, ets;
	//timespec_get(&sts, TIME_UTC);
	clock_t start,_end;
	start=clock();
	for (int i = 0; i < 5; i++) {
		int num = 0;
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];//�����е���
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		//------ - ������-------- -
		ADP(list, idx, num);
		delete list;
	}
	_end=clock();
	cout<<(_end-start)/5;
	/*timespec_get(&ets, TIME_UTC);
	time_t dsec = ets.tv_sec - sts.tv_nsec;
	long dnsec = ets.tv_nsec - sts.tv_nsec;
	printf(" % llu. % 09llus\n", dsec, dnsec);*/
	return 0;
}

