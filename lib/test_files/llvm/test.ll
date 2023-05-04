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
  %6 = load i32, ptr %a, align 4
  %7 = load i32, ptr %i, align 4
  %8 = add i32 %6, %7
  store i32 %8, ptr %b, align 4
  %9 = load i32, ptr %a, align 4
  %10 = load i32, ptr %b, align 4
  %11 = add i32 %9, %10
  store i32 %11, ptr %i, align 4
  %12 = load i32, ptr %a, align 4
  %13 = add i32 %12, 3
  store i32 %13, ptr %b, align 4
  store i32 8, ptr %b, align 4
  %14 = load i32, ptr %i, align 4
  %15 = load i32, ptr %a, align 4
  %16 = icmp slt i32 %14, %15
  br i1 %16, label %19, label %21

17:                                               ; preds = %40, %38
  %18 = load i32, ptr %RETURN, align 4
  ret i32 %18

19:                                               ; preds = %1
  %20 = load i32, ptr %a, align 4
  store i32 %20, ptr %b, align 4
  br label %24

21:                                               ; preds = %1
  %22 = load i32, ptr %a, align 4
  %23 = icmp sgt i32 %22, 3
  br i1 %23, label %25, label %26

24:                                               ; preds = %28, %19
  br label %29

25:                                               ; preds = %21
  store i32 3, ptr %b, align 4
  br label %28

26:                                               ; preds = %21
  %27 = load i32, ptr %a, align 4
  call void @Print(i32 %27)
  br label %28

28:                                               ; preds = %26, %25
  br label %24

29:                                               ; preds = %38, %24
  %30 = load i32, ptr %a, align 4
  %31 = load i32, ptr %i, align 4
  %32 = icmp slt i32 %30, %31
  br i1 %32, label %33, label %37

33:                                               ; preds = %29
  %34 = call i32 @Read()
  store i32 %34, ptr %a, align 4
  %35 = load i32, ptr %a, align 4
  %36 = sub i32 10, %35
  store i32 %36, ptr %b, align 4
  br label %38

37:                                               ; preds = %29
  br label %40

38:                                               ; preds = %33
  %39 = load i32, ptr %b, align 4
  store i32 %39, ptr %RETURN, align 4
  br label %17
  br label %29

40:                                               ; preds = %37
  %41 = call i32 @Read()
  store i32 %41, ptr %RETURN, align 4
  br label %17
}
