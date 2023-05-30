source_filename = "../lib/test_files/files/test.c"
target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %e = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  %2 = load i32, ptr %i, align 4
  store i32 %2, ptr %a, align 4
  store i32 4, ptr %b, align 4
  %3 = load i32, ptr %a, align 4
  %4 = add i32 %3, 4
  store i32 %4, ptr %c, align 4
  %5 = load i32, ptr %c, align 4
  %6 = add i32 4, %5
  store i32 %6, ptr %d, align 4
  %7 = load i32, ptr %d, align 4
  %8 = add i32 %7, %3
  store i32 %8, ptr %e, align 4
  %9 = load i32, ptr %e, align 4
  %10 = add i32 %9, %5
  store i32 %10, ptr %b, align 4
  %11 = add i32 %3, %7
  store i32 %11, ptr %c, align 4
  %12 = load i32, ptr %b, align 4
  %13 = add i32 %2, %12
  store i32 %13, ptr %a, align 4
  %14 = load i32, ptr %c, align 4
  %15 = add i32 %7, %14
  store i32 %15, ptr %e, align 4
  %16 = load i32, ptr %e, align 4
  store i32 %16, ptr %RETURN, align 4
  %17 = load i32, ptr %RETURN, align 4
  ret i32 %17
}
