extern void print(int);
extern int read();

int func(int i){
	int a;
	int b;
	b = 2;
	
	while (b < i){
		a = read();
		b = b * a;
		print(b);
	}

	return b;
}
