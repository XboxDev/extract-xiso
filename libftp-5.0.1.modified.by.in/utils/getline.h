#ifndef GETLINE_H
#define GETLINE_H

/* unix systems can #define POSIX to use termios, otherwise 
 * the bsd or sysv interface will be used 
 */

char           *getline();		/* read a line of input */
void            gl_setwidth();		/* specify width of screen */
void            gl_histadd();		/* adds entries to hist */
void		gl_strwidth();		/* to bind gl_strlen */

extern int 	(*gl_in_hook)();
extern int 	(*gl_out_hook)();
extern int	(*gl_tab_hook)();

#endif /* GETLINE_H */
