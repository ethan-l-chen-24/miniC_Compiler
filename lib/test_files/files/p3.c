extern void print(int);
extern int read();

int func(int i){
	int a;
	int b;
	a = 1;
	b = 1;
	
	while (b < i){
		b = b * 2;
		print(b);
	}

	return b;
}
