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
  %4 = load i32, ptr %b, align 4
  %5 = add i32 %3, %4
  store i32 %5, ptr %c, align 4
  %6 = load i32, ptr %b, align 4
  %7 = load i32, ptr %c, align 4
  %8 = add i32 %6, %7
  store i32 %8, ptr %d, align 4
  %9 = load i32, ptr %d, align 4
  %10 = load i32, ptr %a, align 4
  %11 = add i32 %9, %10
  store i32 %11, ptr %e, align 4
  %12 = load i32, ptr %e, align 4
  %13 = load i32, ptr %c, align 4
  %14 = add i32 %12, %13
  store i32 %14, ptr %b, align 4
  %15 = load i32, ptr %a, align 4
  %16 = load i32, ptr %d, align 4
  %17 = add i32 %15, %16
  store i32 %17, ptr %c, align 4
  %18 = load i32, ptr %i, align 4
  %19 = load i32, ptr %b, align 4
  %20 = add i32 %18, %19
  store i32 %20, ptr %a, align 4
  %21 = load i32, ptr %d, align 4
  %22 = load i32, ptr %c, align 4
  %23 = add i32 %21, %22
  store i32 %23, ptr %e, align 4
  %24 = load i32, ptr %e, align 4
  store i32 %24, ptr %RETURN, align 4
  %25 = load i32, ptr %RETURN, align 4
  ret i32 %25
}
