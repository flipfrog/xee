//
// rect.cc:
//
#include <stdio.h>
#include <stdlib.h>
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
#include <point.h>
#include <rect.h>
#include <misc.h>
#include <geometry.h>

#include <xfig.h>

//
// コンストラクタ
//
Rect::Rect( Board* board ): Shape( board )
{
  shape_type = ST_RECT;
  line_style = board->line_style;
  line_width = board->line_width;
  fill_mode = board->fill_mode;
  _x = _y = _w = _h = 0;
}

//
// コピーコンストラクタ
//
Rect::Rect( Rect* rect ): Shape( rect )
{
  shape_type = ST_LINE;
  line_style = rect->line_style;
  line_width = rect->line_width;
  fill_mode = rect->fill_mode;
  _x = rect->_x, _y = rect->_y;
  _w = rect->_w, _h = rect->_h;
}

//
// draw():
//
void Rect::draw( Window window, int x, int y )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;
  switch( line_style ){
  case LS_SOLID:
    XSetLineAttributes( dpy, gc, line_width, LineSolid, CapButt, JoinMiter );
    break;
  case LS_DOT:
    XSetDashes( dpy, gc, 0, dot_pat, 2 );
    XSetLineAttributes(dpy,gc,line_width,LineOnOffDash,CapButt,JoinMiter);
    break;
  case LS_DASH:
    XSetDashes( dpy, gc, 0, dash_pat, 2 );
    XSetLineAttributes(dpy,gc,line_width,LineOnOffDash,CapButt,JoinMiter);
    break;
  }
  if( fill_mode ){
    XFillRectangle( dpy, window, gc,
		   ROUND(_x)+x, ROUND(_y)+y, ROUND(_w)+1, ROUND(_h)+1  );
  }else{
    XDrawRectangle( dpy, window, gc,
		   ROUND(_x)+x, ROUND(_y)+y, ROUND(_w)+1, ROUND(_h)+1  );
  }
  XSetLineAttributes( dpy, gc, 0, LineSolid, CapButt, JoinMiter );
}

//
// hit():
//
Boolean Rect::hit( int x, int y, int hw )
{
  int l = ROUND(_x), u = ROUND(_y), r = ROUND(_x+_w)-1, b = ROUND(_y+_h)-1;
  if( contain_chk( l, u, x, y, hw ) )
    return True;
  if( contain_chk( r, u, x, y, hw ) )
    return True;
  if( contain_chk( r, b, x, y, hw) )
    return True;
  if( contain_chk( l, b, x, y, hw ) )
    return True;
  if( check_on_the_line( x, y, l, u, r, u, hw ) )
    return True;
  if( check_on_the_line( x, y, r, u, r, b, hw ) )
    return True;
  if( check_on_the_line( x, y, r, b, l, b, hw ) )
    return True;
  if( check_on_the_line( x, y, l, b, l, u, hw ) )
    return True;
  return False;
}

//
// layout():
//
EditResult Rect::layout( XEvent* event )
{
  Display* dpy = event->xany.display;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  static int last_x = -1, last_y;
  if( event->type == MotionNotify && proc_phase > 0 ){
    int mx = event->xmotion.x, my = event->xmotion.y;
    board->quontize( mx, my );
    if( last_x != mx || last_y != my ){
      if( last_x >= 0 ){
	int x1 = ROUND(_x), y1 = ROUND(_y), x2 = last_x, y2 = last_y;
	normal_rect( x1, y1, x2, y2 );
	XSetFunction( dpy, board->gc, GXclear );
	XDrawRectangle( dpy, event->xmotion.window, board->gc,
		       x1, y1, x2-x1, y2-y1 );
	XSetFunction( dpy, board->gc, GXset );
      }
      int x1 = ROUND(_x), y1 = ROUND(_y), x2 = mx, y2 = my;
      normal_rect( x1, y1, x2, y2 );
      XDrawRectangle( dpy, event->xmotion.window, board->gc,
		     x1, y1, x2-x1, y2-y1 );
      last_x = mx, last_y = my;
    }
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
  }
  switch( proc_phase ){
  case 0: // 一回目のクリック。
    if( event->type != ButtonPress && event->xbutton.button != 1 ){
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
    }
    Window window = event->xbutton.window;
    int mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    _x = (double)mx, _y = (double)my;
    last_x = -1;
    proc_phase = 1;
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
    break;
  case 1: // 二回目のクリック。
    if( event->type != ButtonPress ){
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
    }
    window = event->xbutton.window;
    switch( event->xbutton.button ){
    case 1: // 左ボタン矩形の確定。
      mx = event->xbutton.x, my = event->xbutton.y;
      board->quontize( mx, my );
      if( ROUND(_x) == mx && ROUND(_y) == my ){
	XBell( dpy, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CONTINUE;
      }
      if( last_x >= 0 ){
	XSetFunction( dpy, board->gc, GXclear );
	int x1 = ROUND(_x), y1 = ROUND(_y), x2 = last_x, y2 = last_y;
	normal_rect( x1, y1, x2, y2 );
	XDrawRectangle( dpy, event->xmotion.window, board->gc,
		       x1, y1, x2-x1, y2-y1 );
      }
      int x1, x2, y1, y2;
      x1 = ROUND(_x), y1 = ROUND(_y), x2 = mx, y2 = my;
      normal_rect( x1, y1, x2, y2 );
      _x = x1, _y = y1, _w = x2 - x1 - 1, _h = y2 - y1 - 1;
      XSetFunction( dpy, board->gc, GXset );
      XSetPlaneMask( dpy, board->gc, board->shape_mask );
      draw( window, 0, 0 );
      last_x = -1;
      proc_phase = 0;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン(キャンセル)
      XSetFunction( dpy, board->gc, GXclear );
      if( last_x >= 0 ){
	int x1 = ROUND(_x), y1 = ROUND(_y), x2 = last_x, y2 = last_y;
	normal_rect( x1, y1, x2, y2 );
	XDrawRectangle( dpy, event->xmotion.window, board->gc,
		       x1, y1, x2-x1, y2-y1 );
      }
      last_x = -1;
      proc_phase = 0;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CANCEL;
      break;
    }
    break;
  }
  return EDIT_CONTINUE;
}

//
// save():
//
void Rect::save( FILE* fp )
{
  fprintf( fp, "rect{\n" );
  fprintf( fp, "  line_style = %s;\n", ls_string(line_style) );
  fprintf( fp, "  geom = (%g,%g,%g,%g);\n", _x, _y, _w, _h );
  if( !board->msdos_compatibility ){
    fprintf( fp, "  line_width = %d;\n", line_width );
    fprintf( fp, "  fill = %s;\n", bool_string(fill_mode) );
  }
  fprintf( fp, "}\n" );
}

//
// tex():
//
void Rect::tex( FILE* fp, double h, double x, double y )
{
  switch( line_style ){
  case LS_SOLID: fprintf( fp, "\\path" ); break;
  case LS_DOT:   fprintf( fp, "\\dottedline{5}" ); break;
  case LS_DASH:  fprintf( fp, "\\dashline{4.0}" ); break;
  }
  fprintf( fp, "(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)\n",
	  _x+x,      h - (_y+y),
	  _x+x+_w-1, h - (_y+y),
	  _x+x+_w-1, h - (_y+y+_h-1),
	  _x+x,      h - (_y+y+_h-1),
	  _x+x,      h - (_y+y) );
}

//
// bound():
//
void Rect::bound( double& x, double& y, double& w, double& h )
{
  x = _x, y = _y, w = _w, h = _h;
}

//
// translate():
//
void Rect::translate( double x, double y )
{
  _x += x, _y += y;
}

//
// duplicate():
//
Shape* Rect::duplicate()
{
  return new Rect( this );
}

//
// contain():
//
Boolean Rect::contain( int x, int y, int w, int h )
{
  if( ROUND(_x) < x || ROUND(_x+_w) >= x+w )
    return False;
  if( ROUND(_y) < y || ROUND(_y+_h) >= y+h )
    return False;
  return True;
}

//
// scale():
//
void Rect::scale( double rx, double ry )
{
  _x *= rx, _y *= ry;
  _w *= rx, _h *= ry;
}

//
// リサイズ
//
EditResult Rect::resize( XEvent* event )
{
  Display* dpy = event->xany.display;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  static int last_x = -1, last_y;
  static int xx, yy;
  if( proc_phase == 1 && event->type == MotionNotify ){
    int mx = event->xmotion.x, my = event->xmotion.y;
    board->quontize( mx, my );
    if( last_x == mx && last_y == my ){
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
    }
    int x1 = xx, y1 = yy, x2 = last_x, y2 = last_y;
    normal_rect( x1, y1, x2, y2 );
    if( last_x > 0 ){
      XSetFunction( dpy, board->gc, GXclear );
      XDrawRectangle( dpy, event->xbutton.window, board->gc,
		     x1, y1, x2-x1-1, y2-y1-1 );
      XSetFunction( dpy, board->gc, GXset );
    }
    x1 = xx, y1 = yy, x2 = mx, y2 = my;
    normal_rect( x1, y1, x2, y2 );
    XDrawRectangle( dpy, event->xbutton.window, board->gc,
		   x1, y1, x2-x1-1, y2-y1-1 );
    last_x = mx, last_y = my;
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
  }

  switch( proc_phase ){
  case 0:
    int mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    xx = ROUND(_x), yy = ROUND(_y+_h)-1;
    if( contain_chk( ROUND(_x+_w)-1, ROUND(_y+_h)-1, mx, my, 4 ) )
      xx = ROUND(_x), yy = ROUND(_y);
    if( contain_chk( ROUND(_x), ROUND(_y+_h)-1, mx, my, 4 ) )
      xx = ROUND(_x+_w)-1, yy = ROUND(_y);
    if( contain_chk( ROUND(_x), ROUND(_y), mx, my, 4 ) )
      xx = ROUND(_x+_w)-1, yy = ROUND(_y+_h)-1;
    last_x = -1;
    proc_phase = 1;
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
    break;
  case 1:
    if( event->type != ButtonPress ){
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
    }
    mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    switch( event->xbutton.button ){
    case 1: // 左ボタン(決定)
      if( last_x < 0 || ( xx == last_x && yy == last_y ) ){
	XBell( dpy, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CONTINUE;
      }
      int x1 = xx, y1 = yy, x2 = last_x, y2 = last_y;
      normal_rect( x1, y1, x2, y2 );
      XSetFunction( dpy, board->gc, GXclear );
      XDrawRectangle( dpy, event->xbutton.window, board->gc,
		     x1, y1, x2-x1-1, y2-y1-1 );
      XSetPlaneMask( dpy, board->gc, board->shape_mask );
      draw( event->xbutton.window, 0, 0 );
      _x = (double)x1, _y = (double)y1;
      _w = (double)(x2-x1-1), _h = (double)(y2-y1-1);
      XSetFunction( dpy, board->gc, GXset );
      draw( event->xbutton.window, 0, 0 );
      proc_phase = 0;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン(キャンセル)
      if( last_x > 0 ){
	XSetFunction( dpy, board->gc, GXclear );
	x1 = xx, y1 = yy, x2 = last_x, y2 = last_y;
	normal_rect( x1, y1, x2, y2 );
	XDrawRectangle( dpy, event->xbutton.window, board->gc,
		       x1, y1, x2-x1-1, y2-y1-1 );
      }
      proc_phase = 0;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CANCEL;
      break;
    }
  }
  return EDIT_CONTINUE;
}

//
// update():
//
void Rect::update()
{
  if( line_style != board->line_style || line_width != board->line_width ||
     fill_mode != board->fill_mode ){
    line_style = board->line_style;
    line_width = board->line_width;
    fill_mode = board->fill_mode;
  }
}

//
// xfig():
//
void Rect::xfig( FILE* fp, double x, double y )
{
  fprintf( fp, "%d %d ", O_POLYLINE, T_BOX );
  switch( line_style ){
  case LS_SOLID: fprintf( fp, "%d ", SOLID_LINE ); break;
  case LS_DASH:  fprintf( fp, "%d ", DASHED_LINE ); break;
  case LS_DOT:   fprintf( fp, "%d ", DOTTED_LINE ); break;
  }
  fprintf( fp, "1 -1 0 0 " ); /* thickness = 1, color, depth, pen = nouse */
  fprintf( fp, "%d 0.0 -1 ", UNFILLED ); /* style_val = 0.0, radius = -1 */
  fprintf( fp, "0 0 \n" );
  fprintf( fp, "%d %d %d %d %d %d %d %d %d %d 9999 9999\n",
	  ROUND(XFS(_x+x)), ROUND(XFS(_y+y)),
	  ROUND(XFS(_x+_w+x)), ROUND(XFS(_y+y)),
	  ROUND(XFS(_x+_w+x)), ROUND(XFS(_y+_h+y)),
	  ROUND(XFS(_x+x)), ROUND(XFS(_y+_h+y)),
	  ROUND(XFS(_x+x)), ROUND(XFS(_y+y)) );
}

//
// rotate():
//
void Rect::rotate( XEvent* event )
{
  int mx = event->xbutton.x, my = event->xbutton.y;
  board->quontize( mx, my );
  double x_tmp;
  double x1 = _x, y1 = _y, x2 = _x+_w, y2 = _y+_h;
  x1 -= (double)mx, x2 -= (double)mx;
  y1 -= (double)my, y2 -= (double)my;
  x_tmp = x1;
  x1 = y1, y1 = -x_tmp;
  x_tmp = x2;
  x2 = y2, y2 = -x_tmp;
  normal_rect( x1, y1, x2, y2 );
  x1 += (double)mx, x2 += (double)mx;
  y1 += (double)my, y2 += (double)my;
  _x = x1, _y = y1, _w = x2 - x1, _h = y2 - y1;
}
