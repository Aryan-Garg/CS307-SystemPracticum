#include <bits/stdc++.h>
#include <mutex>
using namespace std;
#define MAX 1005
int step_i=0;
int** mat1;
int** mat2;
int** res_seq;
int** res_thread;
int global_n;
mutex foo, bar;
void* multi(void* arg)
{
	std::lock(foo,bar);
    int i = step_i++;
    //cout<<"I : "<<i<<endl;
    for (int j = 0; j < global_n; j++){
      for (int k = 0; k < global_n; k++){
        res_thread[i][j] += (mat1[i][k] * mat2[k][j]);
      }
    }
    foo.unlock();
    bar.unlock();
}
void multiply()
{
    int i, j, k;
    for (i = 0; i < global_n; i++) {
        for (j = 0; j < global_n; j++) {
            res_seq[i][j] = 0;
            for (k = 0; k < global_n; k++)
                res_seq[i][j] += mat1[i][k] * mat2[k][j];
        }
    }
}
int main(int argc, char** argv){
	int N = atoi(argv[argc-1]);
	double total_time_seq = 0.0, total_time_thread = 0.0;
	for(int n=1; n<=N; n++){
		global_n=n;
		step_i=0;
		mat1 = new int*[n];
		mat2 = new int*[n];
		res_thread = new int*[n];
		res_seq = new int*[n];
		for(int i=0; i<n; i++){
			mat1[i]=new int[n];
			mat2[i]=new int[n];
			res_thread[i]=new int[n];
			res_seq[i]=new int[n];
		}
		for(int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				mat1[i][j]=rand()%51; //Random values in range [0,50]
				mat2[i][j]=rand()%51;
				res_thread[i][j]=0;
				res_seq[i][j]=0;
			}
		}
		clock_t t;
		//Calculating sequential time
		t=clock();
		multiply();
		t = clock() - t;
		double time_taken_seq = ((double)t)/CLOCKS_PER_SEC;
		total_time_seq += time_taken_seq;
		
		pthread_t threads[n];
		//Calculating threaded time
    	t = clock();

		for (int i = 0; i<n; i++) {
			int *p;
        	pthread_create(&threads[i], NULL, multi, (void*)&p);
    	}
    	
    	for (int i = 0; i <n; i++){
        	pthread_join(threads[i], NULL); 
    	}

    	t = clock() - t;
    	double time_taken_thread = ((double)t)/CLOCKS_PER_SEC;
		total_time_thread+=time_taken_thread;
		
		int flag=0;
		for(int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				if(res_seq[i][j]!=res_thread[i][j]){
					flag=1;
					break;
				}
			}
			if(flag){
				break;
			}
		}
		if(flag){
			cout<<"Does not matach"<<endl;
//			for(int i=0; i<n; i++){
//				for(int j=0; j<n; j++){
//					cout<<res_seq[i][j]<<" ";
//				}
//				cout<<endl;
//			}
//			for(int i=0; i<n; i++){
//				for(int j=0; j<n; j++){
//					cout<<res_thread[i][j]<<" ";
//				}
//				cout<<endl;
//			}
		}
		else{
			cout<<"Matched"<<endl;
		}
		for(int i=0; i<n; i++){
			delete [] mat1[i];
			delete [] mat2[i];
			delete [] res_seq[i];
			delete [] res_thread[i];
		}
		delete [] mat1;
		delete [] mat2;
		delete [] res_seq;
		delete [] res_thread;
	}
	cout<<"total time seq: "<<total_time_seq<<endl;
	cout<<"total time thread: "<<total_time_thread<<endl;
	return 0;
}
