#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */


WP* new_wp(){
  if(free_==NULL){
    printf("no free watch point!");
    assert(free_!=NULL);
  }

  WP * wp = free_;
  free_ = free_ -> next;
  wp -> next = head;
  head = wp;

  //wp->value=1;
  return wp;
}

WP *search_wp(int NO){
  WP *cur = head;
  while(cur)
  {
    if(cur->NO == NO)
      return cur;
    cur = cur -> next;
  }
  return NULL;
}

void free_wp(WP *wp){
  WP *cur = head;
  WP *pre = NULL;
  while(cur)
  {
    if(cur == wp)
    {
      if(pre)
        pre -> next = cur -> next;
      else
        head = cur -> next;
      cur -> next = free_;
      free_ = cur;
      break;
    }
    pre = cur;
    cur = cur -> next;
  }
}


void print_wp()
{
  WP *cur=head;
  if(cur==NULL)
  {
    printf("No watchpoints!\n");
    return;
  }
  while(cur)
  {
    printf("No:%d, Expr:%s, Val:%d\n",cur->NO,cur->expr,cur->value);
    cur=cur->next;
  }
}

bool check_watchpoints_value( ){
  WP *cur=head;
  bool flag=false,success=true;
  while(cur)
  {
    uint32_t result=expr(cur->expr,&success);
    if(!success)
    {
      printf("Calculate expression fail/n");
      continue;
    }
    if(result != cur->value)
    {
      flag=true;
      printf("No:%d, Expr:%s, Val:%d, New Val:%d\n", cur->NO, cur->expr, cur->value, result);
      cur->value=result;
    }
    cur=cur->next;
  }
  return flag;
}

