extern void print(int);
extern int read();

int fun(int i){
	int a;
	int b;
	a = 2;
	b = 0;
	
	while(a < 10) {
		a = a + 3;
		b = b + 1;
	}

	return a + b;
}
