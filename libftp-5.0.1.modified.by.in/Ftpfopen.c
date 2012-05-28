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

	for ( p = &s_fds; *p && (*p)->file != in_file; p = &(*p)->next ) ;

	if ( ! *p ) {	
		*p = (struct fds_node *) malloc( sizeof(struct fds_node) );
		
		(*p)->file = in_file;
		(*p)->type = in_type;
		(*p)->next = nil;
	}
}


int remove_fds_node( FILE *in_file ) {
	struct fds_node	  **p, *q;
	int					result = -1;
	
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
