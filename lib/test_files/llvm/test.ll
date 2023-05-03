target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  %2 = load i32, ptr %i, align 4
  %3 = load i32, ptr %a, align 4
  %4 = icmp slt i32 %2, %3
  br i1 %4, label %6, label %8

5:                                                ; preds = %27, %25
  ret ptr %RETURN

6:                                                ; preds = %1
  %7 = load i32, ptr %a, align 4
  store i32 %7, ptr %b, align 4
  br label %11

8:                                                ; preds = %1
  %9 = load i32, ptr %a, align 4
  %10 = icmp sgt i32 %9, 3
  br i1 %10, label %12, label %13

11:                                               ; preds = %15, %6
  br label %16

12:                                               ; preds = %8
  store i32 3, ptr %b, align 4
  br label %15

13:                                               ; preds = %8
  %14 = load i32, ptr %a, align 4
  call void @Print(i32 %14)
  br label %15

15:                                               ; preds = %13, %12
  br label %11

16:                                               ; preds = %25, %11
  %17 = load i32, ptr %a, align 4
  %18 = load i32, ptr %i, align 4
  %19 = icmp slt i32 %17, %18
  br i1 %19, label %20, label %24

20:                                               ; preds = %16
  %21 = call i32 @Read()
  store i32 %21, ptr %a, align 4
  %22 = load i32, ptr %a, align 4
  %23 = sub i32 10, %22
  store i32 %23, ptr %b, align 4
  br label %25

24:                                               ; preds = %16
  br label %27

25:                                               ; preds = %20
  %26 = load i32, ptr %b, align 4
  store i32 %26, ptr %RETURN, align 4
  br label %5
  br label %16

27:                                               ; preds = %24
  %28 = call i32 @Read()
  store i32 %28, ptr %RETURN, align 4
  br label %5
}
