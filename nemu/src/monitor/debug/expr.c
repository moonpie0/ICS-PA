#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, 

  /* TODO: Add more token types */
  TK_NEQ, 
  TK_OR, TK_AND, 
  TK_LAND, TK_LOR, TK_NOT,
  DEC, HEX,
  REG,
  TK_POINT,


};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},          // minus
  {"\\*", '*'},          // multi/getval
  {"\\/", '/'},          // div
  {"==", TK_EQ},         // equal
  {"!=", TK_NEQ},       // not equal
  {"\\|", TK_OR},        // calc-or
  {"&", TK_AND},         // calc-and
  {"!", TK_NOT},          //log-not
  {"\\|\\|", TK_LOR},      // log-or
  {"&&", TK_LAND},         // log-and

  {"\\$[eE][0-9a-zA-Z]{2}", REG}, // registers
  {"0[xX][a-fA-F0-9]+", HEX}, // hex
  {"[0-9]|([1-9][0-9]*)", DEC}, // decimal


  {"\\(", '('},          // l-paren
  {"\\)", ')'},          // r-paten
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

enum{
  OP_VAL,
  OP_PARN,
  OP_UADD, //+1 -1
  OP_DEFER,
  OP_MUL,
  OP_ADD, // 1+1
  OP_NOT,
  OP_EQ,
  OP_AND,
  OP_OR,
  OP_LAND,
  OP_LOR,

};

typedef struct token {
  int type;
  char str[32];
  int preference; 
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          //default: TODO();
          case TK_NOTYPE: break;
          case '+':
          case '-':
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_ADD;
            ++nr_token;
            break;
          }
          case '*':
          case '/':
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_MUL;
            ++nr_token;
            break;
          }
          case '(':
          case ')':
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_PARN;
            ++nr_token;
            break;
          }
          case TK_EQ:
          case TK_NEQ:
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_EQ;
            ++nr_token;
            break;
          }
          case TK_AND:
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_AND;
            ++nr_token;
            break;
          }
          case TK_OR:
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_OR;
            ++nr_token;
            break;
          }
          case TK_LAND:
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_LAND;
            ++nr_token;
            break;
          }
          case TK_LOR:
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_LOR;
            ++nr_token;
            break;
          }
          case TK_NOT:
          {
            tokens[nr_token].type=rules[i].token_type;
            tokens[nr_token].preference=OP_NOT;
            ++nr_token;
            break;
          }
          case DEC:
          case HEX:{
            for(int i=0;i<substr_len;i++){
              tokens[nr_token].str[i] = substr_start[i];
            }
            tokens[nr_token].str[substr_len] = '\0';
            tokens[nr_token].preference=OP_VAL;
          }
  
          default: {
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
          }
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  for(int i=0;i<nr_token;i++)
  {
    if(tokens[i].type=='*'&&((tokens[i].preference>1)||i==0)){
      tokens[i].type = TK_POINT;
    }
  }

  return true;
}

bool check_parentheses(int l, int r,bool *success){
  int match = 0;
  //bool flag=true;
  for(int i=l;i<=r;i++){
    if(tokens[i].type=='('){
      match+=1;
    }
    else if(tokens[i].type==')'){
      match-=1;
    }
    if(match<0){
      //flag = false;
      *success=false;
      return false;
    }
  }

  if(match==0)
  {
    *success=true;
    if(tokens[l].type=='('&&tokens[r].type==')')
      return true;
  }
  return false;
}

static int find_dominant_operator(int l, int r , bool *success)
{
    int max_level=-1, index=-1;
    int match_paren=0;
    for(int i=l;i<=r;i++)
    {
      if(tokens[i].type=='(')
      {
        match_paren++;
        continue;
      }
      else if(tokens[i].type==')')
      {
        match_paren--;
        continue;
      }

      if(match_paren!=0)
        continue;
      int cur_level=tokens[i].preference;
      if(cur_level<2)  //not operator
        continue;

      if(cur_level>=max_level)
      {
        max_level=cur_level;
        index=i;
      }

    }
    return index;
}


static int32_t eval(int l, int r, bool *success)
{
  if(l>r){
    //*success=false;
    return 0;
  }
  if(l==r)
  {
    *success=true;
    switch(tokens[l].type)
    {
      case DEC:
      {
        int dec = atoi(tokens[l].str);
        return dec;
      }
      case HEX:
      {
        uint32_t hex;
        sscanf(tokens[l].str, "0x%x", &hex);
        return hex;
      }
      case REG:
      {
        if(strcmp(&tokens[l].str[1],"eip")==0) 
          return cpu.eip;
        for(int i=0;i<8;i++){
          if(strcmp(&tokens[l].str[1],reg_name(i,4))==0) 
            return reg_l(i);
        }
        printf("register's name has error\n");
      }
      default:
      {
        *success=false;
        return 0;
      }
    }
  }
  else if(check_parentheses(l,r,success))
    return eval(l+1,r-1,success);
  else
  {
    int op_index=find_dominant_operator(l,r,success);
    int32_t val2=0;
    val2=eval(op_index+1,r,success);
    printf("val2=%d\n",val2);
    if(tokens[op_index].type==TK_NOT)
    {
      if(*success)
        return !val2;
      else 
        return 0;
    }
    else if(tokens[op_index].type==TK_POINT)
      return vaddr_read(val2,4);
    
    int32_t val1=0;
    val1=eval(l,op_index-1,success);
    printf("val1=%d\n",val1);
    if(!*success)
      return 0;

    switch(tokens[op_index].type)
    {
      case '+':
        return val1+val2;
      case '-':
      return val1-val2;
    case '*':
      return val1*val2;
    case '/':
    {
      if(val2==0)
      {
        printf("0 cannot be divided\n");
        *success = false;
        return 0;
      }
      else
        return val1/val2;
    }
    case TK_EQ:
      return val1==val2;
    case TK_NEQ:
      return val1!=val2;
    case TK_LAND:
      return val1&&val2;
    case TK_LOR:
      return val1||val2;
    case TK_AND:
      return val1&val2;
    case TK_OR:
      return val1|val2;
    default:
    {
      *success=false;
      printf("Fail!\n");
      return 0;
    }
  }
  }

}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  int32_t result=eval(0,nr_token-1,success);

  return result;
}

