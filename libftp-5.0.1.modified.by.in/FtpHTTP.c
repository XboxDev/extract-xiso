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

#if ! defined( _WIN32 )
#include <sys/file.h>
#endif

static char ContextLenght[]="Content-Length: ";


STATUS FtpHttpGet(char* server,int port,char *spec, char *newfile)
{
  struct sockaddr_in unit;
  register struct hostent *host;
  register int new_socket;
  char buf[16*1024];
  register int n;
  FILE *fp, *sock;
  register char *p;
  register int counter;
  int ContextLenghtLenght=strlen(ContextLenght);
  int LenghtOfFile=-1;
  
  if ((host=FtpGetHost(server))==NULL)
    return QUIT;
  
  unit.sin_family = host -> h_addrtype; /* AF_INET */
  
  bcopy(host-> h_addr_list[0],(char *)&unit.sin_addr,host->h_length);
  if ( ( new_socket = socket ( unit.sin_family , SOCK_STREAM , 0)) < 0)
    return QUIT;

  unit.sin_port = htons(port);
  
  while ( connect ( new_socket , (struct sockaddr*) &unit , sizeof unit ) < 0 )
    {
      host -> h_addr_list ++;
      if (host -> h_addr_list[0]==NULL) {
	close(new_socket);
	return QUIT;
      }
      bcopy(host -> h_addr_list[0],(char *)&unit,host->h_length);
      close(new_socket);
      if ( ( new_socket = socket ( unit.sin_family , SOCK_STREAM , 0)) < 0)
	{
	  close(new_socket);
	  return QUIT;
	}
    }

  sock= winsock_fdopen(new_socket,"r+");

  if (fprintf(sock,
	      "GET %s HTTP/1.0\r\nAccept: */*\r\nUserAgent: LIBFTP\r\n\r\n",
	      spec)!=0)
    {
      
      shutdown(new_socket,2);
      fclose(sock);
      return QUIT;
    }

  fflush(sock);
  
  p=strrchr(spec,'/');
  if ( ((newfile==NULL) || !*newfile) && p!=NULL)
    strcpy(newfile,++p);

  if ((fp=fopen(newfile,"w+"))==NULL)
    {
      shutdown(new_socket,2);
      close(new_socket);
      return LQUIT;
    }

  while ( (fgets(buf,sizeof buf,sock))!=NULL )
    {
      if (FtpInit.debug!=NULL)
	printf("%s",buf);

      if (strstr(buf,"Internal error")!=NULL)
	return ENOENT;
      
      if (!strncmp(ContextLenght,buf,ContextLenghtLenght))
	{
	  sscanf (buf,"Context-Lenght: %d",&LenghtOfFile);
	}

      if (buf[0]==Ctrl('M') && buf[1]=='\n' && buf[2]==0)
	break; 
    }

  
  for (counter=0; (n=getc(sock))!=EOF ; counter++)
    if (putc(n,fp)==EOF)
      {
	fclose(fp);
	shutdown(new_socket,2);
	fclose(sock);
	return LQUIT;
      }
  fclose(fp);
  shutdown(new_socket,2);
  fclose(sock);

  if (counter==LenghtOfFile || LenghtOfFile == -1 )
    return OK;

  return QUIT;

}



