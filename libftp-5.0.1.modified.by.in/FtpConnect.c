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
 * Revision 3.1  1995/06/20  15:53:50  orel
 * *** empty log message ***
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 2.5  1995/03/05  15:02:26  orel
 * /
 *
 * Revision 2.5  1995/03/05  15:02:26  orel
 * /
 *
 * Revision 2.4  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.3  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.2  1995/02/04  10:00:21  orel
 * *** empty log message ***
 *
 * Revision 2.2  1995/02/04  10:00:21  orel
 * *** empty log message ***
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

STATUS FtpConnect(FTP **con,char * hostname)
{
  struct sockaddr_in unit;
  register struct hostent *host;
  register int new_socket;
  FtpString S1;
  STATUS x;
  
  *con = FtpCreateObject();
  
  if ((host=FtpGetHost(hostname))==NULL)
    return EXIT((*con),QUIT);
  
  strcpy((*con) -> title,host -> h_name); /* Default title for FtpLog */
  
  unit.sin_family = host -> h_addrtype; /* AF_INET */
  
  bcopy(host-> h_addr_list[0],(char *)&unit.sin_addr,host->h_length);
  if ( ( new_socket = socket ( unit.sin_family , SOCK_STREAM , 0)) < 0)
    return EXIT((*con),QUIT);

  unit.sin_port = htons((*con)->port);
  
  while ( connect ( new_socket , (struct sockaddr*) &unit , sizeof unit ) < 0 )
    {
      host -> h_addr_list ++;
      if (host -> h_addr_list[0]==NULL) {
	close(new_socket);
	return EXIT((*con),QUIT);
      }
      bcopy(host -> h_addr_list[0],(char *)&unit,host->h_length);
      close(new_socket);
      if ( ( new_socket = socket ( unit.sin_family , SOCK_STREAM , 0)) < 0)
	{
	  close(new_socket);
	  return EXIT((*con),QUIT);
	}
    }
  
  FTPCMD(*con) = winsock_fdopen(new_socket,"r+");

  if ( (x=FtpGetMessage(*con,S1)) == QUIT )
    return EXIT((*con),QUIT);
  if ( ! FtpGood(x,120,220,EOF))
    {
      close(new_socket);
      return EXIT((*con),-x);
    }

  if ( (*con)->mode != 'A' )
    {
      FtpString x;

      sprintf (x,"TYPE %c",(*con)->mode);
      FtpSendMessage(*con,x);
      FtpGetMessage(*con,x);
    }

  return EXIT((*con),x);
}

#if defined( _WIN32 )
FILE *winsock_fdopen( int in_fd, char *in_mode ) {
	enum { k_socket_buffer_size = 256 };

	FILE		   *fp;
	char			buf[ k_socket_buffer_size ];
	int				len = k_socket_buffer_size, r;

	r = getsockopt( (SOCKET) in_fd, SOL_SOCKET, SO_TYPE, buf, &len );
	if ( r == SOCKET_ERROR && WSAGetLastError() == WSAENOTSOCK ) return fdopen( in_fd, in_mode );

	if ( ( fp = (FILE *) malloc( sizeof(FILE) ) ) != NULL ) {
		memset( fp, 0, sizeof(FILE) );

		fp->_file = in_fd;
		fp->_flag = ( *in_mode == 'r' ? _IOREAD : _IOWRT );
	} else errno = ENOMEM;

	return fp;
}
#endif
