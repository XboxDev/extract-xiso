#include <uftp.h>

TOKEN *root = NULL;
     

TOKEN *get_pointer(TOKEN*,char*);


TOKEN *new_token()
{
  TOKEN *n=(TOKEN *)malloc(sizeof(TOKEN));

  bzero(n,sizeof(TOKEN));

  return n;
}

	      
	  


load_key(char *s)
{
  TOKEN *cur=root,*prev=NULL;
  char *cw;
  int i;
  
  for (i=1; (*(cw=word(s,i))) != '\0'; i++)
    {
      if (cur==NULL)
	{
	  cur=new_token();

	  if (root==NULL) root=cur;
	  
	  if (prev!=NULL) prev->shift=cur;
	  
	  strcpy(cur->word,cw);
	  prev=cur;
	  cur=cur->shift;
	}
      else
	{
	  while ( strcmp(cur->word,cw) !=0 && cur->next != NULL &&
		  cur->word[0]!=0 )
	    cur=cur->next;

	  if (strcmp(cur->word,cw)==0 || cur->word[0]==0)
	    {
	      strcpy(cur->word,cw);
	      prev=cur;
	      cur=cur->shift;
	      continue; /* Take next word from string */
	    }

	  cur->next=new_token();
	  cur=cur->next;
	  strcpy(cur->word,cw);
	  prev=cur;
	  cur=cur->shift;
	  
	}
    }
}

load_link(char *s1,char *s2)
{
  TOKEN *t1,*t2;

  load_key(s1);
  load_key(s2);
  
  t1=get_pointer(root,s1);
  t2=get_pointer(root,s2);

  if (t1->shift==NULL) t1->shift=new_token();
  if (t2->shift!=NULL) free(t2->shift);
  t2->shift=t1->shift;
}

compctl_hook(char *buf, int plen, int *loc)
{
  TOKEN *cur=root;
  int i,ii,lc;
  TOKEN *nw;
  LIST *list=NULL;
  char *cw,*p;
  

  for (i=1;/**/;i++)
    {
      FtpString r={0};
      
      cw=word(buf,i);
      
      if ((nw=get_pointer(cur,cw))!=NULL &&
	  (*word(buf,i+1)!=0 || buf[strlen(buf)-1]==' '))
	{
	  cur=nw->shift;
	  continue;
	}

      if (*word(buf,i+1)!=0)
	return;
      
      for (;cur!=NULL;cur=cur->next)
	if (strncmp(cur->word,cw,strlen(cw))==0)
	  list_add(&list,cur->word);

      lc = list_count(&list);
      
      if (lc==0)
	return;


      if (lc==1)
	{
	  for (ii=1;ii<i;ii++) strcpy(r,makestr(r,word(buf,ii),NULL));
	  strcpy(r,makestr(r,list->item,NULL));

	  for (ii=i+1;*(p=word(buf,ii))!=0;ii++)
	    strcpy(r,makestr(r,word(buf,ii),NULL));

	  strcpy(buf,r);
	  strcat(buf," ");
	  break;
	}
      
      if (*(char*)word(buf,i+1)!=0)
	{
	  cur=nw->shift;
	  continue;
	}
      

      printlist(&list);

      for (ii=1;ii<i;ii++) strcpy(r,makestr(r,word(buf,ii),NULL));

      strcpy(buf,makestr(r,p=common_space(list,cw),NULL));

      if (*p==0 && *cw==0 ) strcat(buf," ");
  
      break;
    }

  if ( strlen(buf) == 1 && buf[0]==' ') buf[0]=0;
  
  *loc=strlen(buf);
  
  gl_redraw_r();
  return -2;

}

print_ctl(char *msg1, TOKEN *t)
{
  FtpString tmp;

  if (t==NULL) t=root;

  for (;t!=NULL;t=t->next)
    {
      sprintf(tmp,"%s %s",msg1,t->word);
      
      if (t->shift!=0)
	print_ctl(tmp,t->shift);
      else
	puts(tmp);
    }
}



TOKEN *get_pointer(TOKEN *cur,char *cw)
{
  while (cur!=NULL)
    {
      if (strcmp(cur->word,cw)==0) return cur;
      cur=cur->next;
    }
  return NULL;
}

char *common_space(LIST *valid, char *item)
{
  LIST *new=NULL;
  LIST *t;
  int i,item_len=strlen(item);
  static FtpString result;

  result[0]=0;
  
  for (t=valid;t!=NULL;t=t->next)
    if (!strncmp(item,t->item,item_len))
      list_add(&new,t->item);


  if (new!=NULL)
    for (i=0; ;i++) /* Compared space */
      for(t=new;t!=NULL;t=t->next)
	if (new->item[i]!=t->item[i] || t->item[i]==0 )
	  {
	    if (i>0)
	      {
		strcpy(result,new->item);
		result[i]=0;
	      }
	    list_init(&new);
	    return result;
	  }
  return result;
}
	

printlist(LIST **list)
{
  LIST *t;
  int maxlen,n,ncols;
  FtpString format;

  gl_char_cleanup();
  fputc ('\n',stderr);

  list_sort(list);
  
  for(maxlen=0,t=(*list);t!=NULL;t=t->next)
    if ((n=strlen(t->item))>maxlen) maxlen=n;

  maxlen++;
  
  ncols=winsize/maxlen;

  if (ncols==0) ncols=1;

  sprintf(format,"%%-%ds",maxlen);
  
  for (n=0,t=(*list);t!=NULL;t=t->next,n++,n%=ncols)
    {
      fprintf(stderr,format,t->item);
      if (n+1==ncols) fputc('\n',stderr);
    }

  if (n!=0) fputc('\n',stderr);
  
  gl_char_init();
  gl_redraw_r();
}





