
=== GENERATED CODE (ASSEMBLY-LIKE) ===
.section .text
.global main

main:
  mov %10, %x
  mov %x, %t1
  mov %5, %t2
  cmp %t2, %t1
  setg %al
  movzx %al, %t0
  cmp $0, %t0
  je L1
  mov %20, %x
L1:

.end
