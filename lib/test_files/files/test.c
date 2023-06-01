extern void print(int);
extern int read();

int func(int n) {
  int i;

  print(n * n);

  while (1 < 2) {
    int val;
    val = read();

    if (val == 0) {
      int t;
      int g;
      int x;
      print(-1);
      t = read();
      g = read();
      x = t + g;
      if (x < n) {
        print(1);
      }
      if (x < n) {
        print(11);
      }
      if (x > n) {
        print(-11);
      }
    }
    else if (val == 1) {
      int t;
      print(-2);
      t = read();
      if (t >= n) {
        print(1);
      } else {
        print(0);
      }
    }
    else if (0 <= val) {
      int t;
      print(-3);
      t = read();
      i = 0;
      while (i > t) {
        print(i);
        i = i - 1;
      }
    }
    else {
      print(100000001);
      i = 0;
      while (i < 3) {
        print(i);
        i = 1 + i;
      }

      return -n;
    }
  }

  return 0;
}