target triple = "x86_64-pc-linux-gnu"

define i32 @test() {
  %m = alloca i32, align 4
  store i32 27, ptr %m, align 4
  %1 = load i32, ptr %m, align 4
  ret i32 %1
}
