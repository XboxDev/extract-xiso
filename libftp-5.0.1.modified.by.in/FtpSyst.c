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

char * FtpSyst(FTP * con)
{
  FtpString tmp;
  static FtpString tmp1;
  int i;
  
  if ( FtpSendMessage(con,"SYST") == QUIT )
    return (char *) EXIT(con,QUIT);
  if ( (i=FtpGetMessage(con,tmp)) == QUIT )
    return (char *) EXIT(con,QUIT);
  
  if ( i != 215 && i != 200 )
    strcpy(tmp1,"UNKNOWN");
  else
    strcpy(tmp1,word(tmp,2));

  con -> err_no = i;
  return tmp1;
}
