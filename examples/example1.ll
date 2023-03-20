; ModuleID = 'example1.c'
source_filename = "example1.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %5, align 4
  %6 = load i32, i32* %2, align 4
  %7 = icmp slt i32 %6, -10
  br i1 %7, label %8, label %9

8:                                                ; preds = %0
  store i32 2, i32* %3, align 4
  br label %10

9:                                                ; preds = %0
  store i32 -4, i32* %3, align 4
  br label %10

10:                                               ; preds = %9, %8
  %11 = load i32, i32* %3, align 4
  %12 = add nsw i32 %11, 10
  store i32 %12, i32* %3, align 4
  %13 = load i32, i32* %4, align 4
  %14 = icmp ne i32 %13, 10
  br i1 %14, label %15, label %16

15:                                               ; preds = %10
  store i32 50, i32* %5, align 4
  br label %16

16:                                               ; preds = %15, %10
  %17 = load i32, i32* %1, align 4
  ret i32 %17
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
