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
#define NUM_THREADS 3
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

vector<unsigned int>& ADP2(int* list, vector<index>& idx, int num)
{
	bool found = true;
	vector<index> idx_;
	vector<unsigned int> ans;
	bool parallel=true;
	for (int k = 0; k < num; k++)
	{
		idx_.push_back(idx[list[k]]);
	}
	sort(idx_.begin(), idx_.end());
	index s = idx[list[0]];
	#pragma omp parallel num_threads(NUM_THREADS)\
     shared(s,idx_,num,list,ans)
    #pragma omp for simd schedule(guided)
    for (int t_id = 0; t_id < NUM_THREADS; t_id++) {

        int start = (s.arr.size() / NUM_THREADS) * t_id;
        int _end;
        if (t_id == NUM_THREADS - 1) {
            _end = s.arr.size();
        }
        else {
            _end = start + (s.arr.size()) / NUM_THREADS;
        }
        s0[t_id].arr.assign(s.arr.begin() + start, s.arr.begin() + _end);
        s0[t_id].len=s0[t_id].arr.size();
        for(int i=0;i<s0[t_id].len;i++){
            //遍历每个list，判断s0[t_id].arr[i]是否出现在其他数组
            bool found=false;
        for(int j=1;j<num;j++){
            //从第1个数组开始遍历，如果第一个中就没有，则退出循环
            for(int k=0;k<idx_[j].len;k++){
                if(s0[t_id].arr[i]==idx_[j].arr[k]){
                    found=true;
                    break;
                }
            }
            if(found==false){
                break;
            }
        }
        if(found==true){
            ans.push_back(s0[t_id].arr[i]);
        }
        }

	}
	#pragma omp for
	for (int t_id = 0; t_id < NUM_THREADS; t_id++) {
		S.insert(S.end(), s0[t_id].arr.begin(), s0[t_id].arr.end());
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
 for(int i=0;i<20;i++){
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
		ADP2(list, idx, num);
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

