
=== GENERATED CODE (ASSEMBLY-LIKE) ===
.section .text
.global main

  mov %42, %global_var
  mov %3.14159, %pi
add:
  mov %a, %t0
  mov %b, %t1
  mov %t0, %rax
  add %t1, %rax
  mov %rax, %result
  mov %result, %t2
  mov %t2, %rax
  ret
count_up:
  mov %0, %i
L1:
  mov %i, %t4
  mov %max, %t5
  cmp %t5, %t4
  setl %al
  movzx %al, %t3
  cmp $0, %t3
  je L1
  mov %i, %t6
  mov %1, %t7
  mov %t6, %rax
  add %t7, %rax
  mov %rax, %i
  jmp L1
L1:
factorial:
  mov %1, %result
  mov %result, %t8
  mov %t8, %rax
  ret
abs_value:
  mov %x, %t10
  mov %0, %t11
  cmp %t11, %t10
  setl %al
  movzx %al, %t9
  cmp $0, %t9
  je L3
  mov %t12, %rax
  ret
  jmp L3
L3:
  mov %x, %t13
  mov %t13, %rax
  ret
L3:
main:
  mov %10, %x
  mov %20, %y
  mov %5.5, %value
  mov %"Hello World", %name
  mov %sum, %t15
  mov %25, %t16
  cmp %t16, %t15
  setg %al
  movzx %al, %t14
  cmp $0, %t14
  je L5
  mov %sum, %t17
  mov %5, %t18
  mov %t17, %rax
  sub %t18, %rax
  mov %rax, %sum
L5:
  mov %0, %t19
  mov %t19, %rax
  ret

.end
