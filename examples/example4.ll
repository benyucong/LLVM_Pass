; ModuleID = 'example4.c'
source_filename = "example4.c"
target datalayout = "e-m:e-i8:8:32-i16:16:32-i64:64-i128:128-n32:64-S128"
target triple = "aarch64-unknown-linux-gnu"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %z = alloca i32, align 4
  %N = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %retval, align 4
  store i32 0, i32* %z, align 4
  store i32 0, i32* %i, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %0 = load i32, i32* %i, align 4
  %1 = load i32, i32* %N, align 4
  %cmp = icmp slt i32 %0, %1
  br i1 %cmp, label %while.body, label %while.end

while.body:                                       ; preds = %while.cond
  %2 = load i32, i32* %x, align 4
  %3 = load i32, i32* %y, align 4
  %mul = mul nsw i32 2, %3
  %mul1 = mul nsw i32 %mul, 3
  %4 = load i32, i32* %z, align 4
  %mul2 = mul nsw i32 %mul1, %4
  %add = add nsw i32 %2, %mul2
  %rem = srem i32 %add, 3
  %sub = sub nsw i32 0, %rem
  store i32 %sub, i32* %x, align 4
  %5 = load i32, i32* %x, align 4
  %mul3 = mul nsw i32 3, %5
  %6 = load i32, i32* %y, align 4
  %mul4 = mul nsw i32 2, %6
  %add5 = add nsw i32 %mul3, %mul4
  %7 = load i32, i32* %z, align 4
  %add6 = add nsw i32 %add5, %7
  %rem7 = srem i32 %add6, 11
  store i32 %rem7, i32* %y, align 4
  %8 = load i32, i32* %z, align 4
  %inc = add nsw i32 %8, 1
  store i32 %inc, i32* %z, align 4
  br label %while.cond, !llvm.loop !10

while.end:                                        ; preds = %while.cond
  %9 = load i32, i32* %retval, align 4
  ret i32 %9
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
!10 = distinct !{!10, !11}
!11 = !{!"llvm.loop.mustprogress"}
