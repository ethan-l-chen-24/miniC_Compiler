source_filename = "../lib/test_files/files/p1.c"
target triple = "x86_64-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @fun(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  store i32 2, ptr %a, align 4
  store i32 3, ptr %b, align 4
  br label %2

2:                                                ; preds = %5, %1
  %3 = load i32, ptr %a, align 4
  %4 = icmp slt i32 %3, 10
  br i1 %4, label %5, label %8

5:                                                ; preds = %2
  %6 = load i32, ptr %a, align 4
  %7 = add i32 %6, 3
  store i32 %7, ptr %a, align 4
  br label %2

8:                                                ; preds = %2
  %9 = load i32, ptr %a, align 4
  store i32 %9, ptr %RETURN, align 4
  %10 = load i32, ptr %RETURN, align 4
  ret i32 %10
}
