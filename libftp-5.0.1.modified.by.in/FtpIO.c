/*		      Library for ftpd clients.(libftp)
			Copyright by Oleg Orel
			 All rights reserved.
			
This  library is desined  for  free,  non-commercial  software  creation. 
It is changeable and can be improved. The author would greatly appreciate 
any  advises, new  components  and  patches  of  the  exist
ing  programs.
Commercial  usage is  also  possible  with  participation of it's author.



*/

#include "FtpLibrary.h"

int FtpRead(FTP *con)
{
  int c;
  
  if ( con -> mode == 'I' )
    return FtpGetc(con,FTPDATA(con));
  
  if ( con->ch != EOF )
    {
      c=con->ch;
      con->ch=EOF;
      return c;
    }
  
  c=FtpGetc(con,FTPDATA(con));
  
  if ( c == Ctrl('M') )
    {
      c = FtpGetc(con,FTPDATA(con));
      
      if ( c == Ctrl('J') )
	return '\n';
      con->ch = c;
      return Ctrl('M');
    }
  return c;
}

int FtpWrite(FTP *ftp,char c)
{
  
  if ( ftp -> mode == 'I' || c != '\n' )
    return FtpPutc(ftp,FTPDATA(ftp),c);
  
  FtpPutc(ftp,FTPDATA(ftp),Ctrl('M'));
  return FtpPutc(ftp,FTPDATA(ftp),Ctrl('J'));
}

	  
int FtpGetc(FTP *ftp,FILE *fp)
{
  fd_set fds;
  char c;
  
  FD_ZERO(&fds);
  FD_SET(fileno(fp),&fds);

  if (select(getdtablesize(), &fds, 0, 0, &(ftp->timeout))<1)
    return EXIT(ftp,QUIT);

  if (recv(fileno(fp),&c,1,0)<1)
    return EOF;
  
  if (ftp->hash!=NULL) (*ftp->hash)(ftp,1);
  
  return (int)c;
}


STATUS FtpPutc(FTP *ftp,FILE *fp,char c)
{
  fd_set fds;
  
  FD_ZERO(&fds);
  FD_SET(fileno(fp),&fds);
  
  if (select(getdtablesize(), 0, &fds, 0, &(ftp->timeout))<1)
    return EXIT(ftp,QUIT);
  
  if (send(fileno(fp),&c,1,0)!=1)
    return EXIT(ftp,QUIT);

  if (ftp->hash!=NULL) (*ftp->hash)(ftp,1);

	return 0;
}


STATUS FtpReadBlock(FTP *ftp, char *buffer, int size)
{
  fd_set fds;
  register int rsize;
  
  FD_ZERO(&fds);
  FD_SET(fileno(FTPDATA(ftp)),&fds);
  
  if (select(getdtablesize(), &fds,0, 0, &(ftp->timeout))<1)
    return EXIT(ftp,QUIT);
  
  
  if ((rsize=recv(fileno(FTPDATA(ftp)),buffer,size,0))<0)
    return EXIT(ftp,QUIT);
  
  if (ftp->hash!=NULL && rsize!=0) (*ftp->hash)(ftp,rsize);
  
  if (ftp->mode == 'A')
    {
		char	*buffer2;
      register int i,ii;
		
	  if ( ( buffer2 = (char *) malloc( size ) ) == NULL ) return EXIT( ftp, LQUIT );

      for (i=0,ii=0;i<rsize;i++,ii++)
	if (buffer[i]==Ctrl('M')&&buffer[i+1]==Ctrl('J'))
	  buffer2[ii]='\n',i++;
	else
	  buffer2[ii]=buffer[i];
      
      rsize=ii;
      bcopy(buffer2,buffer,rsize);

	  free( buffer2 );
    }
  return rsize;
}


STATUS FtpWriteBlock(FTP *ftp, char *buffer, int size) {
	fd_set					fds;
	register int			wsize;
	int						err = 0;
	char				   *buffer2;
	
	if ( ( buffer2 = (char *) malloc( size * 2 ) ) == NULL ) return EXIT( ftp, QUIT );
  
	FD_ZERO( &fds );
	FD_SET( fileno( FTPDATA(ftp) ), &fds );
  
  	ftp->timeout.tv_sec = kDefaultFTPTimeoutSeconds;
  
	if ( ( err = select( fileno( FTPDATA( ftp ) ) + 1, 0, &fds, 0, &ftp->timeout ) ) == -1 ) {
		fprintf( stderr, "select failed in FtpWriteBlock(): %s\n", strerror( errno ) );
	} else if ( ! err ) {
		fprintf( stderr, "select timed out in FtpWriteBlock()\n" );
		err = 1;
	} else err = 0;
  
	if ( ! err && ftp->mode == 'A' ) {
		register int		i, ii;

      for( i=0, ii = 0; ! err && i < size; ++i, ++ii )
		if ( buffer[ i ] == '\n' ) buffer2[ ii++ ] = Ctrl( 'M' ), buffer2[ ii ] = Ctrl( 'J' );
		else buffer2[ ii ] = buffer[ i ];

		buffer = buffer2;
		size = ii;
	}
  
	for ( wsize = 0; ! err && ( wsize += send( fileno( FTPDATA(ftp) ), buffer + wsize, size - wsize, 0 ) ) < size; ) {
		if ( wsize == -1 ) err = -1;
	}
  
	if ( ! err && ftp->hash && wsize ) (*ftp->hash)( ftp, wsize );

	free( buffer2 );

    return err ? EXIT( ftp, QUIT ) : wsize;
}
