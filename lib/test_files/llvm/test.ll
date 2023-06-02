source_filename = "../lib/test_files/files/test.c"
target triple = "x86_64-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %n = alloca i32, align 4
  %i = alloca i32, align 4
  %val = alloca i32, align 4
  %t = alloca i32, align 4
  %g = alloca i32, align 4
  %x = alloca i32, align 4
  store i32 %0, ptr %n, align 4
  %2 = load i32, ptr %n, align 4
  %3 = load i32, ptr %n, align 4
  %4 = mul i32 %2, %3
  call void @print(i32 %4)
  br label %5

5:                                                ; preds = %23, %1
  br i1 true, label %6, label %10

6:                                                ; preds = %5
  %7 = call i32 @read()
  store i32 %7, ptr %val, align 4
  %8 = load i32, ptr %val, align 4
  %9 = icmp eq i32 %8, 0
  br i1 %9, label %11, label %20

10:                                               ; preds = %5
  store i32 0, ptr %RETURN, align 4
  br label %70

11:                                               ; preds = %6
  call void @print(i32 -1)
  %12 = call i32 @read()
  store i32 %12, ptr %t, align 4
  %13 = call i32 @read()
  store i32 %13, ptr %g, align 4
  %14 = load i32, ptr %t, align 4
  %15 = load i32, ptr %g, align 4
  %16 = add i32 %14, %15
  store i32 %16, ptr %x, align 4
  %17 = load i32, ptr %x, align 4
  %18 = load i32, ptr %n, align 4
  %19 = icmp slt i32 %17, %18
  br i1 %19, label %24, label %25

20:                                               ; preds = %6
  %21 = load i32, ptr %val, align 4
  %22 = icmp eq i32 %21, 1
  br i1 %22, label %36, label %41

23:                                               ; preds = %44, %35
  br label %5

24:                                               ; preds = %11
  call void @print(i32 1)
  br label %25

25:                                               ; preds = %24, %11
  %26 = load i32, ptr %x, align 4
  %27 = load i32, ptr %n, align 4
  %28 = icmp slt i32 %26, %27
  br i1 %28, label %29, label %30

29:                                               ; preds = %25
  call void @print(i32 11)
  br label %30

30:                                               ; preds = %29, %25
  %31 = load i32, ptr %x, align 4
  %32 = load i32, ptr %n, align 4
  %33 = icmp sgt i32 %31, %32
  br i1 %33, label %34, label %35

34:                                               ; preds = %30
  call void @print(i32 -11)
  br label %35

35:                                               ; preds = %34, %30
  br label %23

36:                                               ; preds = %20
  call void @print(i32 -2)
  %37 = call i32 @read()
  store i32 %37, ptr %t, align 4
  %38 = load i32, ptr %t, align 4
  %39 = load i32, ptr %n, align 4
  %40 = icmp sge i32 %38, %39
  br i1 %40, label %45, label %46

41:                                               ; preds = %20
  %42 = load i32, ptr %val, align 4
  %43 = icmp sle i32 0, %42
  br i1 %43, label %48, label %50

44:                                               ; preds = %59, %47
  br label %23

45:                                               ; preds = %36
  call void @print(i32 1)
  br label %47

46:                                               ; preds = %36
  call void @print(i32 0)
  br label %47

47:                                               ; preds = %46, %45
  br label %44

48:                                               ; preds = %41
  call void @print(i32 -3)
  %49 = call i32 @read()
  store i32 %49, ptr %t, align 4
  store i32 0, ptr %i, align 4
  br label %51

50:                                               ; preds = %41
  call void @print(i32 100000001)
  store i32 0, ptr %i, align 4
  br label %60

51:                                               ; preds = %55, %48
  %52 = load i32, ptr %i, align 4
  %53 = load i32, ptr %t, align 4
  %54 = icmp sgt i32 %52, %53
  br i1 %54, label %55, label %59

55:                                               ; preds = %51
  %56 = load i32, ptr %i, align 4
  call void @print(i32 %56)
  %57 = load i32, ptr %i, align 4
  %58 = sub i32 %57, 1
  store i32 %58, ptr %i, align 4
  br label %51

59:                                               ; preds = %51
  br label %44

60:                                               ; preds = %63, %50
  %61 = load i32, ptr %i, align 4
  %62 = icmp slt i32 %61, 3
  br i1 %62, label %63, label %67

63:                                               ; preds = %60
  %64 = load i32, ptr %i, align 4
  call void @print(i32 %64)
  %65 = load i32, ptr %i, align 4
  %66 = add i32 1, %65
  store i32 %66, ptr %i, align 4
  br label %60

67:                                               ; preds = %60
  %68 = load i32, ptr %n, align 4
  %69 = mul i32 %68, -1
  store i32 %69, ptr %RETURN, align 4
  br label %70

70:                                               ; preds = %10, %67
  %71 = load i32, ptr %RETURN, align 4
  ret i32 %71
}
