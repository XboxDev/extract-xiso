static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1.1.1  2004/04/30 22:17:53  brian
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

#define C2I(n) ((int)((n)-'0'))

int FtpArchie ( char *what, ARCHIE *result, int len)
{
  FILE *archie;
  FtpString cmd,tmp;
  int i;
 
  bzero(result,sizeof(result[0])*len);
  
  sprintf(cmd,"archie -t -l -m %d %s",len,what);
  
  if ((archie = popen(cmd,"r"))==NULL)
    return 0;

  for(i=0;i<len;i++)
    {
      char *p, *pp;
      
      if (fgets(tmp,sizeof (tmp), archie)==NULL)
	break;


      result[i].createtime.tm_year = C2I (tmp[2 ])*10 - 70 + C2I(tmp[3]);
      result[i].createtime.tm_mday = C2I (tmp[4 ])*10 + C2I(tmp[5]);
      result[i].createtime.tm_hour = C2I (tmp[6 ])*10 + C2I(tmp[7]);
      result[i].createtime.tm_min  = C2I (tmp[8 ])*10 + C2I(tmp[9]);
      result[i].createtime.tm_sec  = C2I (tmp[10])*10 + C2I(tmp[11]);
      
      for(p=tmp; *p!=' '; p++);
      for(; *p==' '; p++);
      
      result[i].size  = atoi(p);

      for(; *p!=' '; p++);
      for(; *p==' '; p++);
      
      for (pp=result[i].host;*p!=' ';p++,pp++) *pp=*p;
      *pp=0;
      for(; *p==' '; p++);
      for (pp=result[i].file;*p!='\n';p++,pp++) *pp=*p;
      *pp=0;

    }
  
  return i;
}
