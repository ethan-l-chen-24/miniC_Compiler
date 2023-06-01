source_filename = "../lib/test_files/files/test.c"
target triple = "x86_64-pc-linux-gnu"

declare void @print(i32)

declare i32 @read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  store i32 10, ptr %a, align 4
  store i32 10, ptr %b, align 4
  %2 = load i32, ptr %a, align 4
  call void @print(i32 %2)
  %3 = load i32, ptr %b, align 4
  store i32 %3, ptr %RETURN, align 4
  %4 = load i32, ptr %RETURN, align 4
  ret i32 %4
}
