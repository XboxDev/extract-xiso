static char rcsid[] = "$Id$";

/*
  $Log$
  Revision 1.1.1.1  2004/04/30 22:17:55  brian
  initial import

 * Revision 5.0  1995/12/10  10:34:21  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *
 * Revision 4.2  1995/09/09  09:51:49  orel
 *  Change type String to FtpString for clean conflicting with
 *  X11's type string which is pointer to character only.
 *  Thanks for MIT for this good name of type.
 *
 * Revision 4.2  1995/09/09  09:51:49  orel
 *  Change type String to FtpString for clean conflicting with
 *  X11's type string which is pointer to character only.
 *  Thanks for MIT for this good name of type.
 *
 * Revision 4.1  1995/08/19  18:42:25  orel
 * *** empty log message ***
 *
 * Revision 4.1  1995/08/19  18:42:25  orel
 * *** empty log message ***
 *
 * Revision 4.0  1995/07/11  14:51:08  orel
 * Libftp version 4.0
 *
 * Revision 4.0  1995/07/11  14:51:08  orel
 * Libftp version 4.0
 *
 * Revision 3.1  1995/06/21  09:28:34  orel
 * Porting to AIX and some modification..........
 *
 * Revision 3.1  1995/06/21  09:28:34  orel
 * Porting to AIX and some modification..........
 *
 * Revision 3.0  1995/03/20  05:26:29  orel
 * *** empty log message ***
 *
 * Revision 3.0  1995/03/20  05:26:29  orel
 * *** empty log message ***
 *
 * Revision 1.3  1995/03/20  05:17:54  orel
 * *** empty log message ***
 *
 * Revision 1.2  1995/03/20  05:17:31  orel
 * *** empty log message ***
 *
 * Revision 1.2  1995/03/20  05:17:31  orel
 * *** empty log message ***
 *
 * Revision 1.1  1995/03/16  09:38:04  orel
 * Initial revision
 *
*/

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
