target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  %2 = load i32, ptr %a, align 4
  %3 = load i32, ptr %i, align 4
  %4 = icmp slt i32 %2, %3
  br i1 %4, label %6, label %7

5:                                                ; No predecessors!
  ret ptr %RETURN

6:                                                ; preds = %1
  br label %12

7:                                                ; preds = %1
  %8 = load i32, ptr %b, align 4
  %9 = load i32, ptr %i, align 4
  %10 = icmp slt i32 %8, %9
  br i1 %10, label %22, label %24

11:                                               ; preds = %26, %19

12:                                               ; preds = %16, %6
  %13 = load i32, ptr %b, align 4
  %14 = load i32, ptr %i, align 4
  %15 = icmp slt i32 %13, %14
  br i1 %15, label %16, label %19

16:                                               ; preds = %12
  %17 = load i32, ptr %b, align 4
  %18 = add i32 %17, 20
  store i32 %18, ptr %b, align 4
  br label %12

19:                                               ; preds = %12
  %20 = load i32, ptr %b, align 4
  %21 = add i32 10, %20
  store i32 %21, ptr %a, align 4
  br label %11

22:                                               ; preds = %7
  %23 = load i32, ptr %a, align 4
  store i32 %23, ptr %b, align 4
  br label %26

24:                                               ; preds = %7
  %25 = load i32, ptr %b, align 4
  store i32 %25, ptr %a, align 4
  br label %26

26:                                               ; preds = %24, %22
  br label %11
}
