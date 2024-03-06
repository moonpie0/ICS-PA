#include "nemu.h"

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
  TK_VAL,


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

  {"[0-9]|([1-9][0-9]*)", DEC}, // decimal
  {"0[xX][a-fA-F0-9]+", HEX}, // hex
  {"\\$[eE][0-9a-zA-Z]{2}", REG}, // registers

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
    if(tokens[i].type=='*'&&((tokens[i].preference==OP_VAL)||i==0)){
      tokens[i].type = TK_VAL;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
