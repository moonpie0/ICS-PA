#include "common.h"
#include "syscall.h"
#include "arch.h"

int sys_none() {
  return 1;
}

void sys_exit(int a){
  _halt(a);
}

int sys_write(int fd, void *buf, size_t len) {
	if(fd == 1 || fd == 2){
		char c;
Log("buffer:%s", (char*)buf);
		for(int i = 0; i < len; i++) {
			memcpy(&c ,buf + i, 1);
			_putc(c);
		}
		return len;
	}
   else{
     panic("Unhandled fd=%d in sys_write()",fd);
   }
	return -1;			
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);

  Log("ARG1= %d", a[0]);
Log("ARG2= %d", a[1]);
Log("ARG3= %d", a[2]);
Log("ARG4= %d", a[3]);

  switch (a[0]) {
    case SYS_none: 
      SYSCALL_ARG1(r) = sys_none();
      break;
    case SYS_exit: 
Log("sys_exit");
      sys_exit(a[1]);
      break;
     case SYS_write:
	Log("sys_write");
      SYSCALL_ARG1(r) = sys_write(a[1], (void*)a[2], a[3]);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
