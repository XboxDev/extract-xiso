static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1.1.1  2004/04/30 22:17:54  brian
	initial import
	
 * Revision 5.0  1995/12/10  10:28:38  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *
 * Revision 4.1  1995/12/02  11:23:07  orel
 * *** empty log message ***
 *
 * Revision 4.0  1995/07/11  07:00:26  orel
 * Libftp Version 4.0
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
 * Revision 1.5  1995/03/20  05:18:13  orel
 * *** empty log message ***
 *
 * Revision 1.4  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 1.4  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 1.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 1.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 1.2  1995/02/04  09:02:53  orel
 * add rcsid
 *
 * Revision 1.2  1995/02/04  09:02:53  orel
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

STATUS FtpMove(FTP *con,char * oldname , char * newname )
{
  STATUS i;

  if ((i=FtpCommand(con,"RNFR %s",oldname,200,350,EOF)) > 1 )
    return FtpCommand(con,"RNTO %s",newname,200,250,EOF);
  else
    return i;
}
