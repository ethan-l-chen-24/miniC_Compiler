target triple = "x86_64-pc-linux-gnu"

define i32 @test() {
  %m = alloca i32, align 4
  %n = alloca i32, align 4
  %o = alloca i32, align 4
  store i32 10, ptr %m, align 4
  %1 = add i32 10, 12
  store i32 %1, ptr %n, align 4
}
