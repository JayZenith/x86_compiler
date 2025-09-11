; ModuleID = 'toy_module'
source_filename = "toy_module"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 2, ptr %x, align 4
  ret i32 0
}
