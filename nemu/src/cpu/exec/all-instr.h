#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);

//control.c
make_EHelper(call);
make_EHelper(call_rm);
make_EHelper(jmp);
make_EHelper(jmp_rm);

//data-mov.c
make_EHelper(push);

//arith.c
make_EHelper(sub);
