source_filename = "../lib/test_files/files/p4.c"
target triple = "x86_64-pc-linux-gnu"

define i32 @func(i32 %0) {
  %RETURN = alloca i32, align 4
  %i = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 %0, ptr %i, align 4
  store i32 3, ptr %a, align 4
  store i32 4, ptr %b, align 4
  br i1 true, label %2, label %3

2:                                                ; preds = %1
  store i32 4, ptr %RETURN, align 4
  br label %4

3:                                                ; preds = %1
  store i32 3, ptr %RETURN, align 4
  br label %4

4:                                                ; preds = %3, %2
  %5 = load i32, ptr %RETURN, align 4
  ret i32 %5
}
