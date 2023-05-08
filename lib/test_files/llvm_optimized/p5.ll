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

2:                                                ; preds = %6, %1
  %3 = load i32, ptr %a, align 4
  %4 = load i32, ptr %i, align 4
  %5 = icmp slt i32 %3, %4
  br i1 %5, label %6, label %10

6:                                                ; preds = %2
  %7 = call i32 @Read()
  store i32 %7, ptr %a, align 4
  %8 = load i32, ptr %a, align 4
  %9 = add i32 10, %8
  store i32 %9, ptr %b, align 4
  br label %2

10:                                               ; preds = %2

11:                                               ; No predecessors!
  %12 = load i32, ptr %RETURN, align 4
  ret i32 %12
}
