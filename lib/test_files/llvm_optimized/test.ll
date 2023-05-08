target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  br label %2

2:                                                ; preds = %1
  %3 = load i32, ptr %i, align 4
  store i32 %3, ptr %RETURN, align 4
  br label %4

4:                                                ; preds = %2
  %5 = load i32, ptr %RETURN, align 4
  ret i32 %5
}
