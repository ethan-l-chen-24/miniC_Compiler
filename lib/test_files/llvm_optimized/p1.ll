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
  br i1 %4, label %5, label %6

5:                                                ; preds = %1
  br label %13

6:                                                ; preds = %1
  %7 = load i32, ptr %b, align 4
  %8 = load i32, ptr %i, align 4
  %9 = icmp slt i32 %7, %8
  br i1 %9, label %23, label %25

10:                                               ; preds = %27, %20

11:                                               ; No predecessors!
  %12 = load i32, ptr %RETURN, align 4
  ret i32 %12

13:                                               ; preds = %17, %5
  %14 = load i32, ptr %b, align 4
  %15 = load i32, ptr %i, align 4
  %16 = icmp slt i32 %14, %15
  br i1 %16, label %17, label %20

17:                                               ; preds = %13
  %18 = load i32, ptr %b, align 4
  %19 = add i32 %18, 20
  store i32 %19, ptr %b, align 4
  br label %13

20:                                               ; preds = %13
  %21 = load i32, ptr %b, align 4
  %22 = add i32 10, %21
  store i32 %22, ptr %a, align 4
  br label %10

23:                                               ; preds = %6
  %24 = load i32, ptr %a, align 4
  store i32 %24, ptr %b, align 4
  br label %27

25:                                               ; preds = %6
  %26 = load i32, ptr %b, align 4
  store i32 %26, ptr %a, align 4
  br label %27

27:                                               ; preds = %25, %23
  br label %10
}
