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

STATUS FtpChmod(FTP *ftp,char *file,int mode)
{
  FtpString msg;

  sprintf(msg,"SITE CHMOD %03o %s",mode,file); 
  return FtpCommand(ftp,msg,"",200,EOF);
  
}
