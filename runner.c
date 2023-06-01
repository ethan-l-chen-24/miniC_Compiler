#include<stdio.h>

extern int fun(int);

void print(int n){
	printf("%d\n", n);
}

int read(){
	int n;
	scanf("%d",&n);
	return(n); 
}

int main(){
    int a = fun(20);
    printf("%d\n", a);
}

