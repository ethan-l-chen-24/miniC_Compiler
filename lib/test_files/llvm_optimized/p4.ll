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
  %5 = load i32, ptr %a, align 4
  %6 = load i32, ptr %i, align 4
  %7 = icmp slt i32 %5, %6
  br i1 %7, label %8, label %11

8:                                                ; preds = %4
  %9 = load i32, ptr %b, align 4
  %10 = add i32 10, %9
  store i32 %10, ptr %a, align 4
  call void @Print(i32 %9)
  br label %4

11:                                               ; preds = %4
}
