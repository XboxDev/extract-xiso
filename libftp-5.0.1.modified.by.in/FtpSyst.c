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
 * Revision 3.1  1995/06/20  15:53:29  orel
 * *** empty log message ***
 *
 * Revision 3.1  1995/06/20  15:53:29  orel
 * *** empty log message ***
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 1.2  1995/03/14  10:47:44  orel
 * add UNKNOWN system type
 *
 * Revision 1.2  1995/03/14  10:47:44  orel
 * add UNKNOWN system type
 *
 * Revision 1.1  1995/03/05  15:02:26  orel
 * Initial revision
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
