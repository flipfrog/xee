//
// oval.cc:
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
#include <oval.h>
#include <misc.h>
#include <geometry.h>

#include <xfig.h>

//
// コンストラクタ
//
Oval::Oval( Board* board ): Shape( board )
{
  shape_type = ST_OVAL;
  _x = _y = _w = _h = 0;
  fill_mode = board->fill_mode;
  line_width = board->line_width;
  line_style = board->line_style;
}

//
// コピーコンストラクタ
//
Oval::Oval( Oval* oval ): Shape( oval )
{
  shape_type = ST_OVAL;
  _x = oval->_x, _y = oval->_y;
  _w = oval->_w, _h = oval->_h;
  fill_mode = oval->fill_mode;
  line_width = oval->line_width;
  line_style = oval->line_style;
}  

//
// draw():
//
void Oval::draw( Window window, int x, int y )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;
  if( fill_mode ){
    XFillArc( dpy, window, gc,
	     ROUND(_x)+x, ROUND(_y)+y, ROUND(_w), ROUND(_h), 0, 360*64 );
  }else{
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
    XDrawArc( board->xcontext->get_dpy(), window, board->gc,
	     ROUND(_x)+x, ROUND(_y)+y, ROUND(_w), ROUND(_h), 0, 360*64 );
    XSetLineAttributes( dpy, gc, 0, LineSolid, CapButt, JoinMiter );
  }
  if( board->grip_disp )
    draw_gp( window, x, y );
}

//
// hit():
//
Boolean Oval::hit( int x, int y, int hw )
{
/*
  double cx = _x + _w/2.0, cy = _y + _h/2.0;
  double dx = (double)xx - cx, dy = (double)yy - cy;
  double angle = get_angle( dx, dy );
  double px = cos( angle )*_w/2.0 + cx, py = sin( angle )*_h/2.0 + cy;
  return contain_chk( ROUND(px), ROUND(py), xx, yy, hw );
*/
  int cx = ROUND( _x + _w/2 ), cy = ROUND( _y + _h/2 );
  int rx = ROUND( _w/2 ), ry = ROUND( _h/2 );
  if( contain_chk( cx, cy-ry, x, y, hw ) )
    return True;
  if( contain_chk( cx+rx, cy, x, y, hw ) )
    return True;
  if( contain_chk( cx, cy+ry, x, y, hw ) )
    return True;
  if( contain_chk( cx-rx, cy, x, y, hw ) )
    return True;
  return False;
}

//
// layout():
//
EditResult Oval::layout( XEvent* event )
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
	XSetFunction( dpy, board->gc, GXclear );
	_draw( event->xmotion.window, ROUND(_x), ROUND(_y), last_x, last_y );
	XSetFunction( dpy, board->gc, GXset );
      }
      _draw( event->xmotion.window, ROUND(_x), ROUND(_y), mx, my );
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
	_draw( event->xbutton.window, ROUND(_x), ROUND(_y), last_x, last_y );
	XSetFunction( dpy, board->gc, GXset );
      }
      int x1, x2, y1, y2;
      x1 = ROUND(_x), y1 = ROUND(_y), x2 = mx, y2 = my;
      normal_rect( x1, y1, x2, y2 );
      _x = x1, _y = y1, _w = x2 - x1 - 1, _h = y2 - y1 - 1;
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
      if( last_x >= 0 )
	_draw( event->xbutton.window, ROUND(_x), ROUND(_y), last_x, last_y );
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
// リサイズ
//
#if 0
EditResult Oval::resize( XEvent* event )
{
  Display* dpy = event->xany.display;
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  XSetFunction( dpy, board->gc, GXset );
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
    if( last_x > 0 ){
      XSetFunction( dpy, board->gc, GXxor );
      _draw( event->xmotion.window, xx, yy, last_x, last_y );
    }
    XSetFunction( dpy, board->gc, GXclear );
    _draw( event->xmotion.window, xx, yy, mx, my );
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
      XSetFunction( dpy, board->gc, GXclear );
      _draw( event->xbutton.window, xx, yy, last_x, last_y );
      XSetPlaneMask( dpy, board->gc, board->shape_mask );
      draw( event->xbutton.window, 0, 0 );
      int x1 = xx, y1 = yy, x2 = last_x, y2 = last_y;
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
	_draw( event->xbutton.window, xx, yy, last_x, last_y );
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
#endif // 0

//
// save():
//
void Oval::save( FILE* fp )
{
  fprintf( fp, "oval{\n" );
  fprintf( fp, "  geom = (%g,%g,%g,%g);\n",
	  _x+_w/2.0, _y+_h/2.0, _w/2.0, _h/2.0 );
  fprintf( fp, "  fill = %s;\n", bool_string( fill_mode ) );
  if( !board->msdos_compatibility ){
    fprintf( fp, "  line_style = %s;\n", ls_string( line_style ) );
    fprintf( fp, "  line_width = %d;\n", line_width );
  }
  fprintf( fp, "}\n" );
}

//
// tex():
//
void Oval::tex( FILE* fp, double h, double x, double y )
{
  fprintf( fp, "\\put(%g,%g){\\ellipse%s{%g}{%g}}\n",
	  _x+_w/2+x, h - (_y+_h/2+y), fill_mode?"*":"", _w, _h );
}

//
// bound():
//
void Oval::bound( double& x, double& y, double& w, double& h )
{
  x = _x, y = _y, w = _w, h = _h;
}

//
// translate():
//
void Oval::translate( double x, double y )
{
  _x += x, _y += y;
}

//
// draw_gp():
//
void Oval::draw_gp( Window window, int x, int y )
{
  int cx = ROUND( _x + _w/2 ) + x - 2, cy = ROUND( _y + _h/2 ) + y - 2;
  int rx = ROUND( _w/2 ), ry = ROUND( _h/2 );
  XDrawArc( board->xcontext->get_dpy(), window, board->gc,
	   cx, cy-ry, 4, 4, 0, 360*64 );
  XDrawArc( board->xcontext->get_dpy(), window, board->gc,
	   cx+rx, cy, 4, 4, 0, 360*64 );
  XDrawArc( board->xcontext->get_dpy(), window, board->gc,
	   cx, cy+ry, 4, 4, 0, 360*64 );
  XDrawArc( board->xcontext->get_dpy(), window, board->gc,
	   cx-rx, cy, 4, 4, 0, 360*64 );
}

//
// dpulicate():
//
Shape* Oval::duplicate()
{
  return new Oval( this );
}

//
// contain():
//
Boolean Oval::contain( int x, int y, int w, int h )
{
  if( ( ROUND(_x) < x || ROUND(_x+_w) >= x+w ) ||
     ( ROUND(_y) < y || ROUND(_y+_h) >= y+h ) )
    return False;
  return True;
}

//
// scale():
//
void Oval::scale( double rx, double ry )
{
  _x *= rx, _y *= ry;
  _w *= rx, _h *= ry;
}

//
// update():
//
void Oval::update()
{
  if( fill_mode != board->fill_mode || line_width != board->line_width ||
     line_style != board->line_style ){
    fill_mode = board->fill_mode;
    line_width = board->line_width;
    line_style = board->line_style;
  }
}

//
// xfig():
//
void Oval::xfig( FILE* fp, double x, double y )
{
  fprintf( fp, "%d 2 ", O_ELLIPSE ); /* ellipse defined by diameters */
  fprintf( fp, "%d 1 -1 0 ", SOLID_LINE ); /* thickness=1,color,depth=nouse*/
  fprintf( fp, "0 " ); /* pen=nouse */
  if( fill_mode )
    fprintf( fp, "21 " );
  else
    fprintf( fp, "%d ", UNFILLED );
  fprintf( fp, "0.0 1 0.0 " ); /* style_val=0.0, direction=1, angle=0.0 */
  fprintf( fp, "%d %d %d %d %d %d %d %d\n",
	  ROUND(XFS(_x+_w/2+x)), ROUND(XFS(_y+_h/2+y)),
	  ROUND(XFS(_w/2)), ROUND(XFS(_h/2)),
	  ROUND(XFS(_x+x)), ROUND(XFS(_y+y)),
	  ROUND(XFS(_x+_w+x)), ROUND(XFS(_y+_h+y)) );
}

//
// _draw():
//
void Oval::_draw( Window window, int x1, int y1, int x2, int y2 )
{
  normal_rect( x1, y1, x2, y2 );
  int w = x2-x1, h = y2-y1;
  XDrawRectangle( board->xcontext->get_dpy(), window, board->gc, x1, y1, w, h );
  XDrawArc( board->xcontext->get_dpy(), window, board->gc, x1, y1, w, h, 0, 360*64 );
  XDrawLine( board->xcontext->get_dpy(), window, board->gc, x1, y1+h/2, x2, y1+h/2 );
  XDrawLine( board->xcontext->get_dpy(), window, board->gc, x1+w/2, y1, x1+w/2, y2 );
}

//
// rotate():
//
void Oval::rotate( XEvent* event )
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
