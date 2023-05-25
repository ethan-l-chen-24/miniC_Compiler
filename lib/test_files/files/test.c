extern void print(int);
extern int read();

int func(int i){
	
	int a;
	int b;
	int c;
	int d;
	int e;

	a = i;
	b = 4;
	c = a + b;
	d = b + c;
	e = d + a;
	b = e + c;
	c = a + d;
	a = i + b;
	e = d + c;

	return e;
}