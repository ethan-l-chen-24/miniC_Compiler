extern void print(int);
extern int read();

int func(int i){
	int a;
	int b;
    a = 4 + i;
    b = a + 3;
    b = a + i;
    i = a + b;
    b = a + 3;
    b = 3 + 5;

    if(i < a) {
        b = a;
    } else if(a > 3) {
        b = 3;
    } else {
        print(a);
    }
	
	while (a < i){
		int a;
		a = read();
		b = 10 - a;
        return b;
	}

    return read();
}