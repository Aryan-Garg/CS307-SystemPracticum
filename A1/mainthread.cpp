#include <bits/stdc++.h>
using namespace std;
#define MAX 1005
int step_i=0;
int** mat1;
int** mat2;
int** res;
int global_n;
void* multi(void* arg)
{

    int i = step_i++; //i denotes row number of resultant matC

    for (int j = 0; j < global_n; j++){
      for (int k = 0; k < global_n; k++){
        res[i][j] += (mat1[i][k] * mat2[k][j]);
      }
    }
 
}
int main(int argc, char** argv){
	int N = atoi(argv[argc-1]);
	double total_time = 0.0;
	for(int n=1; n<=N; n++){
		global_n=n;
		step_i=0;
		mat1 = new int*[n];
		mat2 = new int*[n];
		res = new int*[n];
		for(int i=0; i<n; i++){
			mat1[i]=new int[n];
			mat2[i]=new int[n];
			res[i]=new int[n];
		}
		for(int i=0; i<n; i++){
			for(int j=0; j<n; j++){
				mat1[i][j]=1;
				mat2[i][j]=1;
				res[i][j]=0;
			}
		}
		pthread_t threads[n];
		clock_t t;
    	t = clock();
		for (int i = 0; i<n; i++) {
			int *p;
        	pthread_create(&threads[i], NULL, multi, (void*)p);
    	}
    	for (int i = 0; i <n; i++){
        	pthread_join(threads[i], NULL); 
    	}
//    	for(int i=0; i<n; i++){
//    		for(int j=0; j<n; j++){
//    			cout<<res[i][j]<<" ";
//			}
//			cout<<endl;
//		}
    	t = clock() - t;
    	double time_taken = ((double)t)/CLOCKS_PER_SEC;
    	cout<<"TIME TAKEN: "<<time_taken<<"\n";
		total_time+=time_taken;
	}
	cout<<"Total time: "<<total_time<<endl;
	cout<<"Average time: "<<total_time/(float)N<<endl;
	return 0;
}
