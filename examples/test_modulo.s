
=== GENERATED CODE (ASSEMBLY-LIKE) ===
.section .text
.global main

test_mod:
  mov %10, %a
  mov %3, %b
  mov %a, %t0
  mov %b, %t1
  mov %t0, %rax
  cqo
  idiv %t1
  mov %rdx, %result
  mov %result, %t2
  mov %t2, %rax
  ret
main:
  mov %t3, %rax
  ret

.end
