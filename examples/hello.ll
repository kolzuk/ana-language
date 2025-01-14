; ModuleID = 'hello'
source_filename = "hello"
target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-w64-windows-gnu"

define i64 @_t3Gcd3GCD(i64 %a, i64 %b) {
entry:
  %0 = icmp eq i64 %b, 0
  br i1 %0, label %if.body, label %while.cond

if.body:                                          ; preds = %entry
  ret i64 %a

while.cond:                                       ; preds = %while.body, %entry
  %1 = phi i64 [ %2, %while.body ], [ %a, %entry ]
  %2 = phi i64 [ %4, %while.body ], [ %b, %entry ]
  %3 = icmp ne i64 %2, 0
  br i1 %3, label %while.body, label %after.while

while.body:                                       ; preds = %while.cond
  %4 = srem i64 %1, %2
  br label %while.cond

after.while:                                      ; preds = %while.cond
  ret i64 %1
}

@.str = private constant [13 x i8] c"Hello, world!"

declare i32 @puts(i8*)

define i32 @main() {
entry:
  %0 = call i32 @puts(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @.str, i32 0, i32 0))
  ret i32 0
}
