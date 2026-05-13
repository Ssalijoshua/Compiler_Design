
=== GENERATED CODE (ASSEMBLY-LIKE) ===
.section .text
.global main

main:
  mov %10, %x
  mov %20, %y
  mov %x, %t0
  mov %y, %t1
  mov %t0, %rax
  imul %t1, %rax
  mov %rax, %z

.end
