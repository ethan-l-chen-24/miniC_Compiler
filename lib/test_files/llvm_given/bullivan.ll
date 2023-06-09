; ModuleID = './tests/local_optimization_tests/test4.c'
source_filename = "./tests/local_optimization_tests/test4.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store i32 10, i32* %1, align 4
  store i32 20, i32* %2, align 4
  %7 = mul nsw i32 57, 310          ; this should be taken care of by constant folding
  store i32 %7, i32* %6, align 4    ; this should be taken care of by constant folding
  %8 = sub nsw i32 25, %7           ; this should be taken care of by constant folding
  %9 = load i32, i32* %6, align 4
  store i32 %8, i32* %6, align 4    ; this should be taken care of by constant folding
  %10 = load i32, i32* %6, align 4  ; this shouldn't be substituted in for %9 due to the store in the middle
  %11 = load i32, i32* %6, align 4  ; common subexpr elimination
  %12 = add nsw i32 %11, 200
  store i32 20, i32* %6, align 4
  %13 = add nsw i32 %11, 200        ; should be replaced by %12 due to common subexpr elimination
  store i32 %13, i32* %5, align 4
  %14 = sub nsw i32 25, %7          ; this should be taken care of by constant folding and dead code elimination
  %15 = load i32, i32* %4, align 4
  ret i32 %13
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="non-leaf" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+outline-atomics,+v8a" }

!llvm.module.flags = !{!0, !1, !2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"branch-target-enforcement", i32 0}
!2 = !{i32 1, !"sign-return-address", i32 0}
!3 = !{i32 1, !"sign-return-address-all", i32 0}
!4 = !{i32 1, !"sign-return-address-with-bkey", i32 0}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 1}
!8 = !{i32 7, !"frame-pointer", i32 1}
!9 = !{!"Ubuntu clang version 14.0.0-1ubuntu1"}