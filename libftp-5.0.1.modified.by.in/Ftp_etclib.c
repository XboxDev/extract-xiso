#include "FtpLibrary.h"

INLINE char *findspace(char *str)
{
  while ( !isspace(*str) && *str != '\0' ) str++;
  return str;
}


    
char *word(char *str, int n)
{
  FtpString new;
  register char *p1, *p2;
  register int i;
  
  strcpy(new,str);
  
  p1=new;

  while (isspace(*p1)) p1++;

  if (n>1 )
    for (i=0;i<n-1;i++) /* Skip n-1 words */
      {
	if ((*p1=='"')||(*p1=='\'')) 
	  {
	    p1=(char *)strchr(p1+1,*p1);
	    if (p1==NULL) return "";
	    p1++;
	    while ( isspace(*p1) ) p1++;
	    continue;
	  }
	p1=findspace(p1);
	if ( *p1=='\0' ) return "";
	p1++;
	while ( isspace(*p1) ) p1++;
      }

  if ((*p1=='"')|(*p1=='\''))
    {
      p2=(char *)strchr(p1+1,*p1);
      if (p2==NULL) return p1+1;
      *p2=0;
      return p1+1;
    }
  
  if  ((p2=findspace(p1)) != NULL )
    {
      *p2=0;
      return p1;
    }
  return "";
}


