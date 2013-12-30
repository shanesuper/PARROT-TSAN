#include "stdio.h"
#include "stdlib.h"
#include "pthread.h"
#define n 10000000
pthread_t th[4];
double a[4];
void* run_0(void* arg){
	int i;
	for(i=1;i<n;++i)
	a[0]++;
	return NULL;
}
void* run_1(void* arg){
	int i;
	for(i=1;i<n;++i)
	a[1]++;
	return NULL;
}
void* run_2(void* arg){
	int i;
	for(i=1;i<n;++i)
	a[2]++;
	return NULL;
}
void* run_3(void* arg){
	int i;
	for(i=1;i<n;++i)
	a[3]++;
	return NULL;
}
int main(){

	pthread_create(&th[0],NULL,run_0,NULL);
	pthread_create(&th[1],NULL,run_1,NULL);
	pthread_create(&th[2],NULL,run_2,NULL);
	pthread_create(&th[3],NULL,run_3,NULL);
	int i;
	for(i=1;i<n;++i){
	a[0]++;
	a[1]++;
	a[2]++;
	a[3]++;
	}
	pthread_join(th[0],NULL);
	pthread_join(th[1],NULL);
	pthread_join(th[2],NULL);
	pthread_join(th[3],NULL);
	return 0;
}
