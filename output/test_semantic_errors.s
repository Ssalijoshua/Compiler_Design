
=== GENERATED CODE (ASSEMBLY-LIKE) ===
.section .text
.global main

main:
  mov %10, %x
  mov %x, %t0
  mov %5, %t1
  mov %t0, %rax
  add %t1, %rax
  mov %rax, %y
  mov %20, %x
  mov %0, %t2
  mov %t2, %rax
  ret

.end
