target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  br label %2

2:                                                ; preds = %17, %1
  %3 = load i32, ptr %a, align 4
  %4 = load i32, ptr %i, align 4
  %5 = icmp slt i32 %3, %4
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  br label %10

7:                                                ; preds = %2

8:                                                ; No predecessors!
  %9 = load i32, ptr %RETURN, align 4
  ret i32 %9

10:                                               ; preds = %14, %6
  %11 = load i32, ptr %b, align 4
  %12 = load i32, ptr %i, align 4
  %13 = icmp slt i32 %11, %12
  br i1 %13, label %14, label %17

14:                                               ; preds = %10
  %15 = load i32, ptr %b, align 4
  %16 = add i32 %15, 20
  store i32 %16, ptr %b, align 4
  br label %10

17:                                               ; preds = %10
  %18 = load i32, ptr %b, align 4
  %19 = add i32 10, %18
  store i32 %19, ptr %a, align 4
  br label %2
}
