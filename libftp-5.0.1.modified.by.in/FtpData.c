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
 * Porting to AIX
 *
 * Revision 3.0  1995/03/20  05:26:07  orel
 * *** empty log message ***
 *
 * Revision 2.7  1995/03/20  05:18:13  orel
 * *** empty log message ***
 *
 * Revision 2.6  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.6  1995/02/26  16:46:50  orel
 * *** empty log message ***
 *
 * Revision 2.5  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.5  1995/02/18  15:42:53  orel
 * modify for recurive mget
 *
 * Revision 2.4  1995/02/04  09:01:40  orel
 * add rcsid
 *
 * Revision 2.4  1995/02/04  09:01:40  orel
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

STATUS FtpData(FTP * con,char * command , char * file ,char * mode)
{
  struct sockaddr_in data,from;
  register struct hostent *host;
  FtpString hostname;
  int NewSocket,Accepted_Socket,len=sizeof(data),one=1,fromlen=sizeof(from),i;
  char *a,*b;

  FREE(data);
  FREE(from);
  
  if ( gethostname( hostname , sizeof hostname ) == -1 )
    return EXIT(con,QUIT);
  
  if ((host=(struct hostent *)gethostbyname(hostname))==0)
    return EXIT(con,QUIT);
  
  data.sin_family = host -> h_addrtype;
  
  bcopy(host-> h_addr_list[0],(char *)&data.sin_addr,host->h_length);
		
  if ((NewSocket = socket ( AF_INET  , SOCK_STREAM , 0 ))<0) {
	fprintf( stderr, "socket() failed in FtpData: %s\n", strerror( errno ) );

    return EXIT(con,QUIT);
  }

	if ( setsockopt ( NewSocket , SOL_SOCKET , SO_REUSEADDR , (char *)&one , sizeof(one) ) < 0 ) {
		fprintf( stderr, "setsockopt() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT ( con,QUIT );
	}

	data.sin_port = 0 ;

	if ( bind ( NewSocket , (struct sockaddr*) &data , sizeof data ) < 0 ) {
		fprintf( stderr, "bind() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}

	if ( getsockname ( NewSocket , (struct sockaddr*) &data , &len ) < 0 ) {
		fprintf( stderr, "bind() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}
  
	if ( listen ( NewSocket , 1 ) < 0 ) {
		fprintf( stderr, "listen() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}

	a = ( char * ) & data.sin_addr;
	b = ( char * ) & data.sin_port;

	FtpAssert(con,FtpPort(con,CUT(a[0]),CUT(a[1]),CUT(a[2]),CUT(a[3]),CUT(b[0]),CUT(b[1])));

	if ( con -> seek != 0) {
		if ((i = FtpCommand ( con, "REST %d" , con -> seek , 0, EOF)) != 350 ) {
			close(NewSocket);
			return -i;
		}
	}
  
	FtpAssert(con, i=FtpCommand ( con , command , file , 200, 120 , 150 , 125 , 250 , EOF ));
  
	if (( Accepted_Socket = accept (NewSocket , (struct sockaddr *)&from , &fromlen )) < 0) {
		fprintf( stderr, "accept() failed in FtpData: %s\n", strerror( errno ) );
		close(NewSocket);
		return EXIT(con,QUIT);
	}

	close(NewSocket);

	FTPDATA(con) = winsock_fdopen(Accepted_Socket, "r+");

	return i;
}
