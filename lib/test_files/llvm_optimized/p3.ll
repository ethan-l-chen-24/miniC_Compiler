source_filename = "../lib/test_files/files/p3.c"
target triple = "x86_64-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  store i32 2, ptr %b, align 4
  br label %2

2:                                                ; preds = %6, %1
  %3 = load i32, ptr %b, align 4
  %4 = load i32, ptr %i, align 4
  %5 = icmp slt i32 %3, %4
  br i1 %5, label %6, label %12

6:                                                ; preds = %2
  %7 = call i32 @read()
  store i32 %7, ptr %a, align 4
  %8 = load i32, ptr %b, align 4
  %9 = load i32, ptr %a, align 4
  %10 = mul i32 %8, %9
  store i32 %10, ptr %b, align 4
  %11 = load i32, ptr %b, align 4
  call void @print(i32 %11)
  br label %2

12:                                               ; preds = %2
  %13 = load i32, ptr %b, align 4
  store i32 %13, ptr %RETURN, align 4
  %14 = load i32, ptr %RETURN, align 4
  ret i32 %14
}
