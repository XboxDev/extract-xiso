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
 * Revision 3.1  1995/06/20  15:53:50  orel
 * Porting to AIX
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
 * Revision 2.1  1995/02/04  09:02:53  orel
 * add rcsid
 *
 * Revision 2.1  1995/02/04  09:02:53  orel
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


enum {T_EMPTY=0,T_FILE,T_STREAM,T_PIPE,T_FULL};


struct fds_node {
	FILE			   *file;
	int					type;
	struct fds_node	   *next;
};


static struct fds_node	   *s_fds;
static pthread_mutex_t		s_fds_mutex;


void add_fds_node( FILE *in_file, int in_type ) {
	struct fds_node	  **p;
	static int init = 0;
	
	if ( ! init ) {
		pthread_mutex_init( &s_fds_mutex, nil );
		init = 1;
	}
	
	pthread_mutex_lock( &s_fds_mutex );
	
	for ( p = &s_fds; *p && (*p)->file != in_file; p = &(*p)->next ) ;

	if ( ! *p ) {	
		*p = (struct fds_node *) malloc( sizeof(struct fds_node) );
		
		(*p)->file = in_file;
		(*p)->type = in_type;
		(*p)->next = nil;
	}
	
	pthread_mutex_unlock( &s_fds_mutex );
}


int remove_fds_node( FILE *in_file ) {
	struct fds_node	  **p, *q;
	int					result = -1;
	
	pthread_mutex_lock( &s_fds_mutex );

	for ( p = &s_fds; *p && (*p)->file != in_file; p = &(*p)->next ) ;
	
	if ( *p ) {
		q = *p;
		*p = (*p)->next;
		
		switch ( q->type ) {
			case T_STREAM:		result = fflush( q->file );		break;
			case T_FILE:		result = fclose( q->file );		break;
			case T_PIPE:		result = pclose( q->file );		break;
			
			default:											break;
		}
		
		free( q );
	}

	pthread_mutex_unlock( &s_fds_mutex );
	
	return result;
}


FILE *Ftpfopen(char *filename,char *mode)
{
  FILE *fp;
  
  if (!strcmp(filename,"*STDIN*") || (!strcmp(filename,"-") && (mode[0]=='r')) )
    {
		add_fds_node( stdin, T_STREAM );
      return stdin;
    }
  
  if (!strcmp(filename,"*STDOUT*") || (!strcmp(filename,"-") && (mode[0]=='w')))
    {
    	add_fds_node( stdout, T_STREAM );
      return stdout;
    }
  
  if (strcmp(filename,"*STDERR*")==0)
    {
		add_fds_node( stderr, T_STREAM );
      return stderr;
    }

  if (*filename=='|') 
    {
      fp=popen(filename+1,mode);
      if (fp==NULL) return fp;
     	add_fds_node( fp, T_PIPE );
      return fp;
    }

  fp=fopen(filename,mode);
  if (fp==NULL) return fp;

	add_fds_node( fp, T_FILE );
	return fp;  
}

int Ftpfclose(FILE *fp)
{
	return remove_fds_node( fp );
}



