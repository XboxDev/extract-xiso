#ifndef __LIST_H__
#define __LIST_H__
typedef struct _list
{
  char item[1024];
  struct _list *next;
} LIST;


int list_init(LIST**);
int list_add(LIST**,char*);
int list_remove(LIST**,char*);
int list_count(LIST**);
#endif


