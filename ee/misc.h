//
// misc.h:
//
#ifndef __misc_h
#define __misc_h

char* ls_string( LineStyle ls );
char* lm_string( LineMode lm );
char* rs_string( RoundStyle rs );
char* bool_string( Boolean b );
char* font_size_string( FontSize f_size );
Boolean yesno_box( Frame* parent, char* msg, char* confirm, char* cancel );
Boolean selectABC_box( Frame* parent, char* str,
		      char* a, char* b, char* cancel );
void message_box( Frame* parent, char* msg, char* confirm );
void tex_ball( FILE* fp, double h, double ox, double oy,
	      double angle, double r );
void tex_arrow( FILE* fp, double h, double ox, double oy,
	       double angle, double r, double t );
void draw_arrow( Display* dpy, Window window, GC gc,
		double ox, double oy, double angle, double r );
void draw_ball( Display* dpy, Window window, GC gc,
	       double ox, double oy, double angle, double r );

int get_font_point( FontSize size );
void DrawX( Display* dpy, Window win, GC gc, int x, int y );
void draw_cursor( Display* dpy, Window w, GC gc, int x, int y, Board* board );

#endif /* __misc_h */
