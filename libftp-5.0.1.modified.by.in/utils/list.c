#include "list.h"
#include "uftp.h"
#include <stdio.h>


list_init(LIST **list)
{
  if (*list==NULL) return;
  
  if ((*list)->next!=NULL)
    list_init(&((*list)->next));

  free(*list);
  *list=NULL;
}

list_add(LIST **list,char *item)
{
  register LIST *t;

  for (t=(*list);;t=t->next)
    {
      if (t==NULL)
	{
	  t=(LIST *) malloc(sizeof(LIST));
	  if (t==NULL) return -1;
	  bzero(t,sizeof *t);
	  break;
	}
      
      if (!strcmp(t->item,item)) return 0; /* ALready in list */

      if (t->next==NULL)
	{
	  t = t->next = (LIST *) malloc(sizeof(LIST));
	  if (t==NULL) return -1;
	  bzero(t,sizeof *t);
	  break;
	}
    }

  strcpy(t->item,item);
  t->next=NULL;
  if ((*list)==NULL) (*list)=t;
  
  return 1;
}

list_count(LIST **list)
{
  register int count=0;
  register LIST *t;
  
  for(t=(*list);t!=NULL;t=t->next)
    count++;

  return count;
}

list_remove(LIST **list, char *item)
{
    register LIST *t,*prev;

    for(t=(*list);t!=NULL;prev=t,t=t->next)
      {
	if (!strcmp(t->item,item))
	  {
	    if ((*list)=t)
	      {
		*list=t->next;
		free(t);
		return;
	      }

	    prev -> next = t -> next;

	    free(t);
	    return;
	  }
      }
    
}



list_sort(LIST **list)
{
  LIST *l,*ll;
  FtpString tmp;

  for (l=(*list);l!=NULL && (l->next)!=NULL ;l=l->next)
    for (ll=l;ll!=NULL;ll=ll->next)
      if (strcmp(l->item,ll->item)>0)
	{
	  strcpy(tmp,l->item);
	  strcpy(l->item,ll->item);
	  strcpy(ll->item,tmp);
	}
}
