
=== GENERATED CODE (ASSEMBLY-LIKE) ===
.section .text
.global main

main:
  mov %10, %x
  mov %3.14, %y
  mov %"Hello", %name
  mov %x, %t1
  mov %5, %t2
  cmp %t2, %t1
  setg %al
  movzx %al, %t0
  cmp $0, %t0
  je L1
  mov %y, %t3
  mov %2.5, %t4
  mov %t3, %rax
  add %t4, %rax
  mov %rax, %y
  mov %x, %t5
  mov %t5, %rax
  ret
L1:
  mov %0, %t6
  mov %t6, %rax
  ret

.end
