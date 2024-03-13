#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

void print_wp(void);
WP* new_wp();
void free_wp(WP* wp);

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
    /* extract the first argument */
  char *arg = strtok(NULL, " ");

  if(arg!=NULL)
    cpu_exec(atoi(arg));
  else  
    cpu_exec(1);
  return 0;
}

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_p(char *args);

static int cmd_w(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  {"si", "Execute the N instructions step by step", cmd_si},
  {"info","Print the statement of programs, 'r'is register and 'w'is watchpoint",cmd_info},
  {"x", "scan the memory, regard the result of expr as address and print", cmd_x},
  {"p", "Print the result of the expression",cmd_p},
  {"w","Set watchpoint at expr's value",cmd_w},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}


static int cmd_info(char *args)
{
  char *arg =strtok(NULL, "");
  if(arg==NULL)
  {
    printf("Please input SUBCMD 'r' or 'w'\n");
    return 0;
  }

  if(strcmp(arg,"r")==0)
    for(int i=0;i<8;i++)
      printf("%s:0x%x\n", reg_name(i,4),reg_l(i));
  else if(strcmp(arg,"w")==0)
    print_wp();
  else
    printf("No such SUBCMD! Please input 'r' or 'w'\n");
  
  return 0;
}

static int cmd_x(char *args){
  char *arg1 = strtok(NULL, " ");
  if(arg1==NULL){
    printf("Please input subcmp N\n");
    return 0;
  }
  int i_arg1 = atoi(arg1);
  char *arg2 = strtok(NULL, " ");

  if(arg2==NULL){
    printf("Please input subcmp expr\n");
    return 0;
  }
  //uint32_t addr_begin = strtoul(arg2,NULL,16);
  uint32_t addr_begin ;
  sscanf(arg2, "0x%x", &addr_begin);
  printf("the result of expr is:%x\n",addr_begin);
  for(int i=0;i<i_arg1;i++){
    printf("0x%x ", vaddr_read(addr_begin,1));
    addr_begin+=1;
  }
  printf("\n");
  return 0;
}

static int cmd_p(char *args){
  char *arg =strtok(NULL, "");
  if(arg==NULL)
  {
    printf("Please input expression\n");
    return 0;
  }

  bool success=true;
  uint32_t result=expr(arg, &success);
  if(!success)
    printf("Your expression is erroneous!\n");
  else
    printf("the result is: %d\n",result);
  return 0;
}

static int cmd_w(char *args){
  char *arg =strtok(NULL, "");
  if(arg==NULL)
  {
    printf("Please input expression\n");
    return 0;
  }
  bool success=false;
  uint32_t result=expr(arg, &success);
  if(!success)
  {
    printf("Your expression is erroneous!\n");
    return 0;
  }

  WP *wp=new_wp();
  wp->value=result;
  int i;
  for(i=0;arg[i];i++)
    wp->expr[i]=arg[i];
  wp->expr[i]='\0';
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
