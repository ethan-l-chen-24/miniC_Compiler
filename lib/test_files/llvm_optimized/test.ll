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
  %3 = add i32 4, %2
  store i32 %3, ptr %a, align 4
  %4 = load i32, ptr %a, align 4
  %5 = add i32 %4, 3
  store i32 %5, ptr %b, align 4
  %6 = add i32 %4, %2
  store i32 %6, ptr %b, align 4
  %7 = load i32, ptr %b, align 4
  %8 = add i32 %4, %7
  store i32 %8, ptr %i, align 4
  %9 = add i32 %4, 3
  store i32 %9, ptr %b, align 4
  store i32 8, ptr %b, align 4
  %10 = load i32, ptr %i, align 4
  %11 = icmp slt i32 %10, %4
  br i1 %11, label %14, label %16

12:                                               ; preds = %35, %33
  %13 = load i32, ptr %RETURN, align 4
  ret i32 %13

14:                                               ; preds = %1
  %15 = load i32, ptr %a, align 4
  store i32 %15, ptr %b, align 4
  br label %19

16:                                               ; preds = %1
  %17 = load i32, ptr %a, align 4
  %18 = icmp sgt i32 %17, 3
  br i1 %18, label %20, label %21

19:                                               ; preds = %23, %14
  br label %24

20:                                               ; preds = %16
  store i32 3, ptr %b, align 4
  br label %23

21:                                               ; preds = %16
  %22 = load i32, ptr %a, align 4
  call void @Print(i32 %22)
  br label %23

23:                                               ; preds = %21, %20
  br label %19

24:                                               ; preds = %19
  %25 = load i32, ptr %a, align 4
  %26 = load i32, ptr %i, align 4
  %27 = icmp slt i32 %25, %26
  br i1 %27, label %28, label %32

28:                                               ; preds = %24
  %29 = call i32 @Read()
  store i32 %29, ptr %a, align 4
  %30 = load i32, ptr %a, align 4
  %31 = sub i32 10, %30
  store i32 %31, ptr %b, align 4
  br label %33

32:                                               ; preds = %24
  br label %35

33:                                               ; preds = %28
  %34 = load i32, ptr %b, align 4
  store i32 %34, ptr %RETURN, align 4
  br label %12

35:                                               ; preds = %32
  %36 = call i32 @Read()
  store i32 %36, ptr %RETURN, align 4
  br label %12
}
