/*
		      Library for ftpd clients.(libftp)
			Copyright by Oleg Orel
			 All rights reserved.
			
This  library is desined  for  free,  non-commercial  software  creation. 
It is changeable and can be improved. The author would greatly appreciate 
any  advises, new  components  and  patches  of  the  existing  programs.
Commercial  usage is  also  possible  with  participation of it's author.



*/

#include "FtpLibrary.h"

char * FtpPwd(FTP * con)
{
  FtpString tmp;
  static FtpString tmp1;
  int i;
  
  if ( FtpSendMessage(con,"PWD") == QUIT )
    return (char *) EXIT(con,QUIT);
  if ( (i=FtpGetMessage(con,tmp)) == QUIT )
    return (char *) EXIT(con,QUIT);
  
  if ( i != 257 )
    return (char *) EXIT(con,-i);

  strcpy(tmp1,word(tmp,2));
  if (*tmp1=='"')
    {
      strcpy(tmp,tmp1+1);
      strncpy(tmp1,tmp,strlen(tmp)-1);
    }
  con -> err_no = i;
  return tmp1;
}
