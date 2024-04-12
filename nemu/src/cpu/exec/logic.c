#include "cpu/exec.h"

make_EHelper(test) {
  // 逻辑与运算，将目标操作数和源操作数进行按位与运算
  rtl_and(&t2, &id_dest->val, &id_src->val);
  // 更新ZF、SF标志位
  rtl_update_ZFSF(&t2, id_dest->width);
  // 清空CF和OF标志位
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(test);
}

make_EHelper(and) {
  // 逻辑与运算，将目标操作数和源操作数进行按位与运算
  rtl_and(&t2, &id_dest->val, &id_src->val);
  // 将结果写回目标操作数
  operand_write(id_dest, &t2);
  // 更新ZF、SF标志位
  rtl_update_ZFSF(&t2, id_dest->width);
  // 清空CF和OF标志位
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(and);
}

make_EHelper(xor) {
  // 异或运算，将目标操作数和源操作数进行按位异或运算
  rtl_xor(&t2, &id_dest->val, &id_src->val);
  // 将结果写回目标操作数
  operand_write(id_dest, &t2);
  // 更新ZF、SF标志位
  rtl_update_ZFSF(&t2, id_dest->width);
  // 清空CF和OF标志位
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(xor);
}

make_EHelper(or) {
  // 逻辑或运算，将目标操作数和源操作数进行按位或运算
  rtl_or(&t2, &id_dest->val, &id_src->val);
  // 将结果写回目标操作数
  operand_write(id_dest, &t2);
  // 更新ZF、SF标志位
  rtl_update_ZFSF(&t2, id_dest->width);
  // 清空CF和OF标志位
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  print_asm_template2(or);
}

make_EHelper(sar) {
  // 算术右移，将目标操作数进行算术右移
  // 在NEMU中不需要更新CF和OF标志位
  rtl_sext(&t2, &id_dest->val, id_dest->width);
  rtl_sar(&t2, &t2, &id_src->val);
  operand_write(id_dest, &t2);
  // 更新ZF、SF标志位
  rtl_update_ZFSF(&t2, id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  // 逻辑左移，将目标操作数进行逻辑左移
  // 在NEMU中不需要更新CF和OF标志位
  rtl_shl(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  // 更新ZF、SF标志位
  rtl_update_ZFSF(&t2, id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  // 逻辑右移，将目标操作数进行逻辑右移
  // 在NEMU中不需要更新CF和OF标志位
  rtl_shr(&t2, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t2);
  // 更新ZF、SF标志位
  rtl_update_ZFSF(&t2, id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  // 根据条件码设置目标操作数的值
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);
  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  // 按位取反，将目标操作数进行按位取反
  rtl_not(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(not);
}

make_EHelper(rol) {
  // 循环左移，将目标操作数进行循环左移
  rtl_shri(&t2, &id_dest->val, id_dest->width * 8 - id_src->val);
  rtl_shl(&t3, &id_dest->val, &id_src->val);
  rtl_or(&t1, &t2, &t3);
  operand_write(id_dest, &t1);
  print_asm_template2(rol);
}

