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

3:                                                ; preds = %7, %1
  %4 = load i32, ptr %b, align 4
  %5 = load i32, ptr %i, align 4
  %6 = icmp slt i32 %4, %5
  br i1 %6, label %7, label %13

7:                                                ; preds = %3
  %8 = load i32, ptr %b, align 4
  %9 = add i32 10, %8
  store i32 %9, ptr %a, align 4
  %10 = load i32, ptr %b, align 4
  %11 = load i32, ptr %i, align 4
  %12 = mul i32 %10, %11
  store i32 %12, ptr %b, align 4
  br label %3

13:                                               ; preds = %3
  br label %14

14:                                               ; preds = %18, %13
  %15 = load i32, ptr %b, align 4
  %16 = load i32, ptr %i, align 4
  %17 = icmp slt i32 %15, %16
  br i1 %17, label %18, label %21

18:                                               ; preds = %14
  %19 = load i32, ptr %b, align 4
  %20 = mul i32 %19, 10
  store i32 %20, ptr %b, align 4
  br label %14

21:                                               ; preds = %14
}
