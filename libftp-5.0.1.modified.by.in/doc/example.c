static char rcsid[] = "$Id$";

/*
	$Log$
	Revision 1.1.1.1  2004/04/30 22:17:54  brian
	initial import
	
 * Revision 5.0  1995/12/10  10:37:11  orel
 * LIBFTP Version 5.0 (Distributed revision)
 *

*/

/* Include standard libftp's header */

#include <FtpLibrary.h> 



main(int argc, char *argv[])
{

  FILE *input,*output;
  int c;
  

  if (argc<3)
    exit(fprintf(stderr,"Usage: %s input-file output-file\n",argv[0]));

  FtplibDebug(yes);
  
  if ((input=Ftpfopen(argv[1],"r"))==NULL)
    {
      perror(argv[1]);
      exit(1);
    }
  
  if ((output=Ftpfopen(argv[2],"w"))==NULL)
    {
      perror(argv[2]);
      exit(1);
    }
  
  while ( (c=getc(input)) != EOF &&  (putc(c,output)!=EOF) );

  if (ferror(input))
    {
      perror(argv[1]);
      exit(1);
    }
  
  if (ferror(output))
    {
      perror(argv[1]);
      exit(1);
    }

  Ftpfclose(input);
  Ftpfclose(output);

  exit(0);
  
}
