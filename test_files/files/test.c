extern void print(int);
extern int read();

int func(int i){
	int a;
	int b;

    if(a < i) {
        b = a;
    } else if(a > 3) {
        b = 3;
    } else {
        print(a);
    }
	
	while (a < i){
		int a;
		a = read();
		b = 10 + a;
	}
}
