//
// shape.cc:
//
#include <stdio.h>
#include <math.h>

#include <X11/Xlib.h>

#include <local_types.h>
#include <list.h>

#include <eedefs.h>
#include <shape.h>
#include <shapeset.h>
#include <frame.h>
#include <xcontext.h>
#include <board.h>

//
// コンストラクタ
//
Shape::Shape( Board* b )
{
  board = b;
  proc_phase = 0;
  static char dot[] = { 3, 6 };
  static char dash[] = { 9, 3 };
  dot_pat = dot, dash_pat = dash;
}
Shape::Shape( Shape* shape )
{
  shape_type = shape->shape_type;
  board = shape->board; proc_phase = 0;
  static char dot[] = { 2, 6 };
  static char dash[] = { 6, 3 };
  dot_pat = dot, dash_pat = dash;
}

//
// move():
//
EditResult Shape::move( XEvent* event )
{
  Display* dpy = event->xany.display;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  static int last_x, last_y, first_x, first_y;
  switch( proc_phase ){
  case 0:
    if( event->xbutton.button != 1 || event->type != ButtonPress )
      return EDIT_CANCEL;
    last_x = event->xbutton.x, last_y = event->xbutton.y;
    board->quontize( last_x, last_y );
    first_x = last_x, first_y = last_y;
    proc_phase = 1;
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
    break;
  case 1:
    switch( event->type ){
    case MotionNotify:
      Window window = event->xmotion.window;
      int mx = event->xmotion.x, my = event->xmotion.y;
      board->quontize( mx, my );
      if( mx != last_x || my != last_y ){
	if( last_x >= 0 ){
	  XSetFunction( dpy, board->gc, GXclear );
	  int dx = last_x - first_x, dy = last_y - first_y;
	  draw( window, dx, dy );
	}
	int dx = mx - first_x, dy = my - first_y;
	XSetFunction( dpy, board->gc, GXset );
	draw( window, dx, dy );
	last_x = mx, last_y = my;
      }
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
      break;
    case ButtonPress:
      window = event->xbutton.window;
      switch( event->xbutton.button ){
      case 1: // 左ボタン
	XSetFunction( dpy, board->gc, GXclear );
	XSetPlaneMask( dpy, board->gc, board->shape_mask );
	draw( window, 0, 0 );
	XSetPlaneMask( dpy, board->gc, board->layout_mask );
	int dx = last_x - first_x, dy = last_y - first_y;
	draw( window, dx, dy );
	translate( dx, dy );
	XSetFunction( dpy, board->gc, GXset );
	XSetPlaneMask( dpy, board->gc, board->shape_mask );
	draw( window, 0, 0 );
	proc_phase = 0;
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_COMPLETE;
	break;
      case 3: // 右ボタン
	XSetFunction( dpy, board->gc, GXclear );
	dx = last_x - first_x, dy = last_y - first_y;
	draw( window, dx, dy );
	proc_phase = 0;
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CANCEL;
	break;
      }
    }
  }
  return EDIT_CONTINUE;
}

//
// t_draw():
//
void
Shape::t_draw( Window  window, double scale_x, double scale_y, int x, int y )
{
  Boolean save_gp_draw_f = board->grip_disp;
  Shape tmp( this );
  tmp.scale( scale_x, scale_y );
  board->grip_disp = False;
  tmp.draw( window, x, y );
  board->grip_disp = save_gp_draw_f;
}
