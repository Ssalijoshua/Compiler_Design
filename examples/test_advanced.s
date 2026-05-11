
=== GENERATED CODE (ASSEMBLY-LIKE) ===
.section .text
.global main

test_operators:
  mov %10, %a
  mov %3, %b
  mov %a, %t0
  mov %b, %t1
  mov %t0, %rax
  add %t1, %rax
  mov %rax, %add_result
  mov %a, %t2
  mov %b, %t3
  mov %t2, %rax
  sub %t3, %rax
  mov %rax, %sub_result
  mov %a, %t4
  mov %b, %t5
  mov %t4, %rax
  imul %t5, %rax
  mov %rax, %mul_result
  mov %a, %t6
  mov %b, %t7
  mov %t6, %rax
  cqo
  idiv %t7
  mov %rax, %div_result
  mov %a, %t8
  mov %b, %t9
  mov %t8, %rax
  cqo
  idiv %t9
  mov %rdx, %mod_result
  mov %1, %bool1
  mov %0, %bool2
  mov %bool1, %t10
  mov %bool2, %t11
  mov %bool1, %t12
  mov %bool2, %t13
  mov %a, %t14
  mov %b, %t15
  cmp %t15, %t14
  sete %al
  movzx %al, %eq
  mov %a, %t16
  mov %b, %t17
  cmp %t17, %t16
  setne %al
  movzx %al, %ne
  mov %a, %t18
  mov %b, %t19
  cmp %t19, %t18
  setl %al
  movzx %al, %lt
  mov %a, %t20
  mov %b, %t21
  cmp %t21, %t20
  setg %al
  movzx %al, %gt
  mov %a, %t22
  mov %b, %t23
  cmp %t23, %t22
  setle %al
  movzx %al, %le
  mov %a, %t24
  mov %b, %t25
  cmp %t25, %t24
  setge %al
  movzx %al, %ge
  mov %0, %t26
  mov %t26, %rax
  ret
test_expressions:
  mov %2, %t27
  mov %3, %t29
  mov %4, %t30
  mov %t29, %rax
  imul %t30, %rax
  mov %rax, %t28
  mov %t27, %rax
  add %t28, %rax
  mov %rax, %x
  mov %2, %t33
  mov %3, %t34
  mov %t33, %rax
  add %t34, %rax
  mov %rax, %t31
  mov %4, %t32
  mov %t31, %rax
  imul %t32, %rax
  mov %rax, %y
  mov %10, %t37
  mov %5, %t38
  mov %t37, %rax
  sub %t38, %rax
  mov %rax, %t35
  mov %2, %t36
  mov %t35, %rax
  sub %t36, %rax
  mov %rax, %z
  mov %100, %t41
  mov %2, %t42
  mov %t41, %rax
  cqo
  idiv %t42
  mov %rax, %t39
  mov %5, %t40
  mov %t39, %rax
  cqo
  idiv %t40
  mov %rax, %w
  mov %1, %t47
  mov %2, %t48
  mov %t47, %rax
  add %t48, %rax
  mov %rax, %t45
  mov %3, %t49
  mov %4, %t50
  mov %t49, %rax
  add %t50, %rax
  mov %rax, %t46
  mov %t45, %rax
  imul %t46, %rax
  mov %rax, %t43
  mov %5, %t51
  mov %6, %t52
  mov %t51, %rax
  imul %t52, %rax
  mov %rax, %t44
  mov %t43, %rax
  sub %t44, %rax
  mov %rax, %nested
  mov %nested, %t53
  mov %t53, %rax
  ret
test_arrays:
  mov %1, %t54
  mov %10, %t55
  mov %3.14, %t56
  mov %"array test", %str
  ret
nested_loops:
  ret
nested_conditions:
  mov %5, %x
  mov %10, %y
  mov %x, %t58
  mov %0, %t59
  cmp %t59, %t58
  setg %al
  movzx %al, %t57
  cmp $0, %t57
  je L1
  mov %y, %t61
  mov %0, %t62
  cmp %t62, %t61
  setg %al
  movzx %al, %t60
  cmp $0, %t60
  je L3
  mov %x, %t64
  mov %5, %t65
  cmp %t65, %t64
  sete %al
  movzx %al, %t63
  cmp $0, %t63
  je L5
L5:
L5:
L5:
  ret
main:
  mov %0, %t66
  mov %t66, %rax
  ret

.end
