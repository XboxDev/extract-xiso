/* Few functions for woring with envirment........... */

#include <stdio.h>

extern char **environ;

setenv(char *name, char *data)
{
  register char **p;
  int len=strlen(name);
  
  char *new = (char *) malloc ( strlen(name) + strlen(data) + 1 + 1);
  
  for (p=environ; *p!=NULL && strncmp(name,*p,len) ; p++);

  sprintf(new,"%s=%s",name,data);
  
  if (*p==NULL)
    {
      *p++=new;
      *p=NULL;
    }
  else
    *p=new;
  
}

unsetenv(char *name)
{
  register char **p;
  int len=strlen(name);
  
  for (p=environ; *p!=NULL && strncmp(*p,name,len) ; p++);

  for (;*p!=NULL;p++) *p=p[1];
}

printenv()
{
  register char **p;

  for (p=environ; *p!=NULL; p++)
    puts(*p);
}

    

    
  
