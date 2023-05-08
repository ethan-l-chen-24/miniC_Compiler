target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  %2 = load i32, ptr %i, align 4
  store i32 %2, ptr %RETURN, align 4
  %3 = load i32, ptr %RETURN, align 4
  ret i32 %3
}
