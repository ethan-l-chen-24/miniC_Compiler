target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  br label %3

2:                                                ; No predecessors!
  ret ptr %RETURN

3:                                                ; preds = %16, %1
  %4 = load i32, ptr %a, align 4
  %5 = load i32, ptr %i, align 4
  %6 = icmp slt i32 %4, %5
  br i1 %6, label %7, label %8

7:                                                ; preds = %3
  br label %9

8:                                                ; preds = %3

9:                                                ; preds = %13, %7
  %10 = load i32, ptr %b, align 4
  %11 = load i32, ptr %i, align 4
  %12 = icmp slt i32 %10, %11
  br i1 %12, label %13, label %16

13:                                               ; preds = %9
  %14 = load i32, ptr %b, align 4
  %15 = add i32 %14, 20
  store i32 %15, ptr %b, align 4
  br label %9

16:                                               ; preds = %9
  %17 = load i32, ptr %b, align 4
  %18 = add i32 10, %17
  store i32 %18, ptr %a, align 4
  br label %3
}
