//
// group.cc:
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#include <X11/Xlib.h>

#include <local_types.h>
#include <list.h>

#include <eedefs.h>
#include <shape.h>
#include <shapeset.h>

#include <frame.h>
#include <xcontext.h>
#include <board.h>
#include <group.h>
#include <misc.h>
#include <geometry.h>

#include <xfig.h>

//
// コンストラクタ
//
Group::Group( Board* board ): Shape( board )
{
  shape_type = ST_GROUP;
  _x = _y = _w = _h = 0;
}

//
// コピーコンストラクタ
//
Group::Group( Group* group ): Shape( group )
{
  _x = group->_x, _y = group->_y;
  _w = group->_w, _h = group->_h;
  for( int i = 0 ; i < group->shape_slot.count() ; i++ )
    shape_slot.append( group->shape_slot.get(i)->duplicate() );
}

//
// draw():
//
void Group::draw( Window window, int x, int y )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;
  int save_gp_draw_f = board->grip_disp;
  board->grip_disp = False;
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->draw( window, ROUND(_x)+x, ROUND(_y)+y );
  board->grip_disp = save_gp_draw_f;
  int left  = ROUND(_x) + x - 3;
  int right = ROUND(_x) + ROUND(_w) + x - 3;
  int up    = ROUND(_y) + y - 3;
  int down  = ROUND(_y) + ROUND(_h) + y - 3;
  XDrawRectangle( dpy, window, gc, left,  up,   5, 5 );
  XDrawRectangle( dpy, window, gc, right, up,   5, 5 );
  XDrawRectangle( dpy, window, gc, left,  down, 5, 5 );
  XDrawRectangle( dpy, window, gc, right, down, 5, 5 );
}

//
// hit():
//
Boolean Group::hit( int x, int y, int hw )
{
  int left  = ROUND(_x);
  int right = ROUND(_x) + ROUND(_w);
  int up    = ROUND(_y);
  int down  = ROUND(_y) + ROUND(_h);

  if( contain_chk( left,  up,   x, y, hw ) )
    return True;
  if( contain_chk( right, up,   x, y, hw ) )
    return True;
  if(contain_chk(  right, down, x, y, hw ) )
    return True;
  if( contain_chk( left,  down, x, y, hw ) )
    return True;
  return False;
}

//
// layout():
//
EditResult Group::layout( XEvent* event )
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
	int x1 = ROUND(_x), y1 = ROUND(_y), x2 = last_x, y2 = last_y;
	normal_rect( x1, y1, x2, y2 );
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
    if( event->type != ButtonPress && event->xbutton.button != 1 )
      return EDIT_CONTINUE;
    Window window = event->xbutton.window;
    int mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    _x = (double)mx, _y = (double)my;
    last_x = -1;
    proc_phase = 1;
    return EDIT_CONTINUE;
    break;
  case 1: // 二回目のクリック。
    if( event->type != ButtonPress )
      return EDIT_CONTINUE;
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
	XSetFunction( dpy, board->gc, GXset );
      }
      int x1, x2, y1, y2;
      x1 = ROUND(_x), y1 = ROUND(_y), x2 = mx, y2 = my;
      normal_rect( x1, y1, x2, y2 );
      _x = x1, _y = y1, _w = x2 - x1 - 1, _h = y2 - y1 - 1;

      for( int i = board->shapeset.count_shape()-1 ; i >= 0 ; i-- ){
	Shape* shape = board->shapeset.get_shape(i);
	if( shape->contain( ROUND(_x), ROUND(_y), ROUND(_w), ROUND(_h) ) ){
	  shape_slot.append( shape->duplicate() );
	  XSetFunction( dpy, board->gc, GXclear );
	  XSetPlaneMask( dpy, board->gc, board->shape_mask );
	  shape->draw( event->xbutton.window, 0, 0 );
	  board->shapeset.unlink_shape( LIST_TOP, i, 1 );
	}
      }

      if( shape_slot.count() == 0 ){
	last_x = -1;
	proc_phase = 0;
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CANCEL;
      }

      double min_x=INT_MAX, min_y=INT_MAX, max_x=INT_MIN, max_y=INT_MIN;
      for( i = 0 ; i < shape_slot.count() ; i++ ){
	Shape* shape = shape_slot.get(i);
	double x, y, w, h;
	shape->bound( x, y, w, h );
	if( x < min_x ) min_x = x;
	if( x+w > max_x ) max_x = x+w;
	if( y < min_y ) min_y = y;
	if( y+h > max_y ) max_y = y+h;
      }
      _x = min_x, _y = min_y, _w = max_x - min_x, _h = max_y - min_y;
      for( i = 0 ; i < shape_slot.count() ; i++ )
	shape_slot.get(i)->translate( -_x, -_y );
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
      if( last_x >= 0 ){
	XSetFunction( dpy, board->gc, GXclear );
	XSetPlaneMask( dpy, board->gc, board->layout_mask );
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
void Group::save( FILE* fp )
{
  fprintf( fp, "group{\n" );
  fprintf( fp, "  geom = (%g,%g,%g,%g);\n", _x, _y, _w, _h );
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->save( fp );
  fprintf( fp, "}\n" );
}

//
// tex():
//
void Group::tex( FILE* fp, double h, double x, double y )
{
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->tex( fp, h, _x+x, _y+y );
}

//
// bound():
//
void Group::bound( double& x, double& y, double& w, double& h )
{
  double min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;
  for( int i = 0 ; i < shape_slot.count() ; i++ ){
    Shape* shape = shape_slot.get(i);
    double x, y, w, h;
    shape->bound( x, y, w, h );
    if( x < min_x ) min_x = x;
    if( x+w > max_x ) max_x = x+w;
    if( y < min_y ) min_y = y;
    if( y+h > max_y ) max_y = y+h;
  }
  _w = max_x - min_x, _h = max_y - min_y;
  x = _x, y = _y;
  w = _w, h = _h;
}

//
// translate():
//
void Group::translate( double x, double y )
{
  _x += x, _y += y;
}

//
// duplicate():
//
Shape* Group::duplicate()
{
  return new Group( this );
}

//
// contain():
//
Boolean Group::contain( int x, int y, int w, int h )
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
void Group::scale( double rx, double ry )
{
  _x *= rx, _y *= ry;
  _w *= rx, _h *= ry;
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->scale( rx, ry );
}

//
// リサイズ
//
EditResult Group::resize( XEvent* event )
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
      scale( (double)(x2-x1-1)/_w, (double)(y2-y1-1)/_h );
      _x = (double)x1, _y = (double)y1;
      XSetFunction( dpy, board->gc, GXset );
      draw( event->xbutton.window, 0, 0 );
      proc_phase = 0;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン(キャンセル)
      if( last_x > 0 ){
	XSetFunction( dpy, board->gc, GXxor );
	x1 = xx, y1 = yy, x2 = last_x, y2 = last_y;
	normal_rect( x1, y1, x2, y2 );
	XSetFunction( dpy, board->gc, GXclear );
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
// reversx():
//
void Group::reversx()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < shape_slot.count() ; i++ ){
    double sx, sy, sw, sh;
    Shape* shape = shape_slot.get(i);
    shape->bound( sx, sy, sw, sh );
    shape->translate( -sx, 0 );
    shape->translate( bw, 0 );
    shape->translate( -(sx+sw), 0 );
    shape->reversx();
  }
}

//
// reversy():
//
void Group::reversy()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < shape_slot.count() ; i++ ){
    double sx, sy, sw, sh;
    Shape* shape = shape_slot.get(i);
    shape->bound( sx, sy, sw, sh );
    shape->translate( 0, -sy );
    shape->translate( 0, bh );
    shape->translate( 0, -(sy+sh) );
    shape->reversy();
  }
}

//
// update():
//
void Group::update()
{
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->update();
  double x, y, w, h;
  bound( x, y, w, h );
  _w = w, _h = h;
}

//
// xfig():
//
void Group::xfig( FILE* fp, double x, double y )
{
  fprintf( fp, "%d %d %d %d %d\n", O_COMPOUND,
	  ROUND(XFS(_x+x)), ROUND(XFS(_y+y)),
	  ROUND(XFS(_x+_w+x)), ROUND(XFS(_y+_h+y)) );
  for( int i = 0 ; i < shape_slot.count() ; i++ )
    shape_slot.get(i)->xfig( fp, _x+x, _y+y );
  fprintf( fp, "%d\n", O_END_COMPOUND );
}

//
// ungroup():
//
void Group::ungroup( Window window )
{
  XSetPlaneMask( board->xcontext->get_dpy(), board->gc, board->shape_mask );
  XSetFunction( board->xcontext->get_dpy(), board->gc, GXclear );
  draw( window, 0, 0 );
  XSetFunction( board->xcontext->get_dpy(), board->gc, GXset );
  for( int j = 0 ; j < shape_slot.count() ; j++ ){
    Shape* dup_shape = shape_slot.get(j)->duplicate();
    dup_shape->translate( _x, _y );
    dup_shape->draw( window, 0, 0 );
    board->shapeset.append_shape( dup_shape );
  }
  XSetFunction( board->xcontext->get_dpy(), board->gc, GXcopy );
  XSetPlaneMask( board->xcontext->get_dpy(), board->gc, ~0 );
}  

//
// rotate():
//
void Group::rotate( XEvent* event )
{
  for( int i = 0 ; i < shape_slot.count() ; i++ ){
    Shape* shape = shape_slot.get(i);
    shape->translate( _x, _y );
    shape->rotate( event );
    shape->translate( -_x, -_y );
  }
  double min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;
  for( i = 0 ; i < shape_slot.count() ; i++ ){
    Shape* shape = shape_slot.get(i);
    double x, y, w, h;
    shape->bound( x, y, w, h );
    if( x < min_x ) min_x = x;
    if( x+w > max_x ) max_x = x+w;
    if( y < min_y ) min_y = y;
    if( y+h > max_y ) max_y = y+h;
  }
  _x += min_x, _y += min_y;
  _w = max_x-min_x, _h = max_y-min_y;
  if( min_x != 0 || min_y != 0 )
    for( i = 0 ; i < shape_slot.count() ; i++ )
      shape_slot.get(i)->translate( -min_x, -min_y );
}
