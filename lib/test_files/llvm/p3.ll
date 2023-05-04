target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  br label %4

2:                                                ; No predecessors!
  %3 = load i32, ptr %RETURN, align 4
  ret i32 %3

4:                                                ; preds = %8, %1
  %5 = load i32, ptr %b, align 4
  %6 = load i32, ptr %i, align 4
  %7 = icmp slt i32 %5, %6
  br i1 %7, label %8, label %14

8:                                                ; preds = %4
  %9 = load i32, ptr %b, align 4
  %10 = add i32 10, %9
  store i32 %10, ptr %a, align 4
  %11 = load i32, ptr %b, align 4
  %12 = load i32, ptr %i, align 4
  %13 = mul i32 %11, %12
  store i32 %13, ptr %b, align 4
  br label %4

14:                                               ; preds = %4
  br label %15

15:                                               ; preds = %19, %14
  %16 = load i32, ptr %b, align 4
  %17 = load i32, ptr %i, align 4
  %18 = icmp slt i32 %16, %17
  br i1 %18, label %19, label %22

19:                                               ; preds = %15
  %20 = load i32, ptr %b, align 4
  %21 = mul i32 %20, 10
  store i32 %21, ptr %b, align 4
  br label %15

22:                                               ; preds = %15
}
