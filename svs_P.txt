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
#define THREADS_NUM 4
using namespace std;

class index {
public:
	int len = 0;
	vector<unsigned int> arr;
};

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
index* s0=new index[THREADS_NUM];
index& SVS_omp_baseonP(int* list, vector<index>& idx, int num) {
	s = idx[list[0]];
	int id;
    int sum;
	#pragma omp parallel num_threads(THREADS_NUM)\
     shared(s,idx,num,list),private(id)
	{
      id=omp_get_thread_num();
      int start=s.arr.size()/THREADS_NUM*id;
      int _end;
      if (id == THREADS_NUM-1) {
		    _end = s.arr.size();
	    }
	    else {
		    _end = start + (s.arr.size()) / THREADS_NUM;
	    }
	    s0[id].arr.assign(s.arr.begin() + start, s.arr.begin() + _end);
      for(int i=1;i<num;i++){

	    for(int k=0;k<s0[id].arr.size();k++){
            bool isexit=false;
            #pragma omp simd
            for (int jj = 0; jj < idx[list[i]].arr.size(); jj++) {
			if (s0[id].arr[k] == idx[list[i]].arr[jj]) {
				isexit = true;
				//break;
			}
		}
		if (isexit == false) {
			s0[id].arr[k]=-1;
		}
	    }
	    vector<unsigned int>::iterator newEnd(remove(s0[id].arr.begin(), s0[id].arr.end(), -1));
	    s0[id].arr.erase(newEnd);
	}
	}
	for(int t_id=0;t_id<THREADS_NUM;t_id++){
        s.arr.insert(s.arr.end(),s0[t_id].arr.begin(),s0[t_id].arr.end());
	}
	return s;
}
vector<index> idx;
int main() {
	//读取二进制文件
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
		stringstream ss; //输入流
		ss << line; //向流中传值
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
		sorted(list, idx, num);
		SVS_omp_baseonP(list, idx, num);
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

