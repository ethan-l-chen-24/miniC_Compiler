; ModuleID = '../lib/test_files/llvm_given/vasanta.ll'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @func(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  store i32 %0, ptr %3, align 4
  store i32 10, ptr %6, align 4
  %8 = load i32, ptr %3, align 4
  %9 = add nsw i32 %8, 10
  store i32 %9, ptr %4, align 4
  %10 = load i32, ptr %6, align 4
  %11 = add nsw i32 %8, %10
  store i32 %11, ptr %5, align 4
  store i32 20, ptr %7, align 4
  store i32 10, ptr %4, align 4
  br label %12

12:                                               ; preds = %15, %1
  %13 = load i32, ptr %3, align 4
  %14 = icmp slt i32 %13, 100
  br i1 %14, label %15, label %22

15:                                               ; preds = %12
  %16 = load i32, ptr %3, align 4
  %17 = add nsw i32 %16, 5
  store i32 %17, ptr %3, align 4
  %18 = load i32, ptr %6, align 4
  %19 = add nsw i32 %18, 10
  store i32 %19, ptr %6, align 4
  %20 = load i32, ptr %4, align 4
  %21 = add nsw i32 %20, 10
  store i32 %21, ptr %7, align 4
  br label %12, !llvm.loop !6

22:                                               ; preds = %12
  %23 = load i32, ptr %7, align 4
  %24 = icmp sgt i32 %23, 100
  br i1 %24, label %25, label %27

25:                                               ; preds = %22
  %26 = load i32, ptr %6, align 4
  store i32 %26, ptr %2, align 4
  br label %28

27:                                               ; preds = %22
  store i32 100, ptr %2, align 4
  br label %28

28:                                               ; preds = %27, %25
  %29 = load i32, ptr %2, align 4
  ret i32 %29
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
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
