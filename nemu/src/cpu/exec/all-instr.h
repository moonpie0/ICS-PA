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
make_EHelper(ret);
make_EHelper(jcc);

//data-mov.c
make_EHelper(push);
make_EHelper(pop);
make_EHelper(mov);
make_EHelper(leave);
make_EHelper(lea);



//arith.c
make_EHelper(sub);
make_EHelper(add);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(cmp);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(neg);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);

//logic.c
make_EHelper(xor);
make_EHelper(or);
make_EHelper(and);
make_EHelper(test);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(sar);
make_EHelper(rol);
make_EHelper(not);
make_EHelper(setcc);

//system.c
make_EHelper(lidt);

