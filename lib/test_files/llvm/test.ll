target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  %2 = load i32, ptr %i, align 4
  %3 = icmp sgt i32 %2, 3
  br i1 %3, label %4, label %7

4:                                                ; preds = %1
  %5 = load i32, ptr %i, align 4
  %6 = icmp sgt i32 %5, 5
  br i1 %6, label %8, label %10

7:                                                ; preds = %10, %1
  ret i32 0

8:                                                ; preds = %4
  %9 = load i32, ptr %i, align 4
  call void @Print(i32 %9)
  br label %10

10:                                               ; preds = %8, %4
  br label %7
}
