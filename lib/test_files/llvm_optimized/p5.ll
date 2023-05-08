target triple = "x86_64-pc-linux-gnu"

declare void @Print(i32)

declare i32 @Read()

define i32 @func(i32 %0) {
  br label %2

2:                                                ; preds = %3, %1
  br i1 <badref>, label %3, label %5

3:                                                ; preds = %2
  %4 = call i32 @Read()
  br label %2

5:                                                ; preds = %2

6:                                                ; No predecessors!
  ret i32 <badref>
}
