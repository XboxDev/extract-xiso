static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1.1.1  2004/04/30 22:17:54  brian
	initial import
	
 * Revision 5.0  1995/12/10  10:28:38  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *
 * Revision 4.1  1995/09/09  09:49:42  orel
 *  Change type String to FtpString for clean conflicting with
 * X11's type string which is pointer to character only.
 * Thanks for MIT for this good name of type.
 *
 * Revision 4.1  1995/09/09  09:49:42  orel
 *  Change type String to FtpString for clean conflicting with
 * X11's type string which is pointer to character only.
 * Thanks for MIT for this good name of type.
 *
 * Revision 4.0  1995/07/11  07:00:26  orel
 * Libftp Version 4.0
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 2.4  1995/03/20  05:18:13  orel
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.2  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.2  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.1  1995/02/04  09:01:40  orel
 * add rcsid
 *
 * Revision 2.1  1995/02/04  09:01:40  orel
 * add rcsid
 *

*/
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

STATUS FtpCommand( FTP *in_ftp, ...)
{
  FTP *con;
  char *command, *param;
  int Answer[MAX_ANSWERS];
  va_list args;
  FtpString S1;
  int i,counter=0;
  
  va_start(args, in_ftp);

  con = in_ftp;
  command = va_arg(args,char *);
  param = va_arg(args, char *);

  while ( 1 )
    {
      if (counter == MAX_ANSWERS)
	return EXIT(con,QUIT);
      Answer[counter] = va_arg(args,int);
      if (Answer[counter] == EOF ) break;
      counter++;
    }

  va_end(args);
  
  
  sprintf(S1,command,param);

  if ( FtpSendMessage(con,S1) == QUIT )
    return EXIT(con,QUIT);
  
  if  ( (i=FtpGetMessage(con,S1)) == QUIT )
    return EXIT(con,QUIT);
  
  if ( ! FtpGood1 ( i , Answer ))
    return EXIT(con,-i);

  return EXIT(con,i);
}
  
  
   
