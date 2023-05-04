; ModuleID = 'test4.c'
source_filename = "test4.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %6 = load i32, ptr %2, align 4
  %7 = add nsw i32 %6, 10
  store i32 %7, ptr %3, align 4
  %8 = load i32, ptr %2, align 4
  %9 = add nsw i32 %8, 20
  store i32 %9, ptr %4, align 4
  %10 = load i32, ptr %3, align 4
  %11 = icmp slt i32 %10, 10
  br i1 %11, label %12, label %24

12:                                               ; preds = %1
  %13 = load i32, ptr %4, align 4
  %14 = load i32, ptr %4, align 4
  %15 = mul nsw i32 %13, %14
  store i32 %15, ptr %4, align 4
  %16 = load i32, ptr %4, align 4
  %17 = load i32, ptr %3, align 4
  %18 = icmp sgt i32 %16, %17
  br i1 %18, label %19, label %23

19:                                               ; preds = %12
  %20 = load i32, ptr %4, align 4
  %21 = load i32, ptr %3, align 4
  %22 = mul nsw i32 %20, %21
  store i32 %22, ptr %3, align 4
  br label %23

23:                                               ; preds = %19, %12
  br label %28

24:                                               ; preds = %1
  %25 = load i32, ptr %3, align 4
  %26 = load i32, ptr %4, align 4
  %27 = mul nsw i32 %25, %26
  store i32 %27, ptr %5, align 4
  br label %28

28:                                               ; preds = %24, %23
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.7"}
