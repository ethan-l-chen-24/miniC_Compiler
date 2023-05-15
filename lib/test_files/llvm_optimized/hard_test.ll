target triple = "x86_64-pc-linux-gnu"

define i32 @test() {
  %m = alloca i32, align 4
  %n = alloca i32, align 4
  store i32 10, ptr %m, align 4
  store i32 8, ptr %n, align 4
  store i32 18, ptr %m, align 4
  store i32 20, ptr %n, align 4
  br label %1

1:                                                ; preds = %0
  store i32 20, ptr %n, align 4
  ret i32 20
}
