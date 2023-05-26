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

class _index {
public:
	int len = 0;
	vector<unsigned int> arr;
};

_index s;
_index temp;
void sorted(int* list, vector<_index>& idx, int num) {
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
_index* s0=new _index[THREADS_NUM];
_index& SVS_omp_sections(int* list, vector<_index>& idx, int num) {//20249
	s = idx[list[0]];
	int id;
    int sum;
    #pragma omp parallel num_threads(4)
    {
        #pragma omp sections nowait
        {
            #pragma omp section
            {
            int id=1;
            for(int k=0;k<s.arr.size();k++){
                    bool isExit=false;
                #pragma omp simd
                for(int j=0;j<idx[list[id]].arr.size();j++){
                    if(s.arr[k]==idx[list[id]].arr[j]){
                        isExit=true;
                        //break;
                    }
                }
                if(isExit==false){
                    //#pragma omp critical
                    #pragma omp atomic write
                    s.arr[k]=-1;
                }
            }
            

             }

        #pragma omp section
        {
            int id=2;
            for(int k=0;k<s.arr.size();k++){
                    bool isExit=false;
                #pragma omp simd
                for(int j=0;j<idx[list[id]].arr.size();j++){
                    if(s.arr[k]==idx[list[id]].arr[j]){
                        isExit=true;
                        //break;
                    }
                }
                if(isExit==false){
                    //#pragma omp critical
                    #pragma omp atomic write
                    s.arr[k]=-1;
                }
            }
            

        }

        #pragma omp section
        {
            if(num>=4){
            int id=3;
            for(int k=0;k<s.arr.size();k++){
                    bool isExit=false;
                #pragma omp simd
                for(int j=0;j<idx[list[id]].arr.size();j++){
                    if(s.arr[k]==idx[list[id]].arr[j]){
                        isExit=true;
                       // break;
                    }
                }
                if(isExit==false){
                    //#pragma omp critical
                   #pragma omp atomic write
                    s.arr[k]=-1;
                }
            }
          }

        }

        #pragma omp section
        {
            if(num>=5){
            int id=4;
            for(int k=0;k<s.arr.size();k++){
                    bool isExit=false;
                #pragma omp simd
                for(int j=0;j<idx[list[id]].arr.size();j++){
                    if(s.arr[k]==idx[list[id]].arr[j]){
                        isExit=true;
                        //break;
                    }
                }
                if(isExit==false){
                    //pragma omp critical
                    #pragma omp atomic write
                    s.arr[k]=-1;
                }
            }
          }
          
        }
      }

    }
	vector<unsigned int>::iterator newEnd(remove(s.arr.begin(), s.arr.end(), -1));
	    s.arr.erase(newEnd);
	return s;
}
vector<_index> idx;
int main() {
	//读取二进制文件
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "No file";
		return 0;

	}

	for (int i = 0; i < 2000; i++) {
		_index temp;
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
	for (int i = 0; i < 5; i++) {
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
		SVS_omp_sections(list, idx, num);
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
