//
// splaine.cc:
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
#include <spline.h>
#include <misc.h>
#include <geometry.h>

#include <xfig.h>

#define SPLIT 8
#define PAI 3.141592
#define RADIAN(x) (((x)/360.0)*PAI*2.0)

//
// コンストラクタ
//
Spline::Spline( Board* board ): Shape( board )
{
  point_slot.set_virtual_link( LIST_REAL );
  wpoint_slot.set_virtual_link( LIST_REAL );
  ipoint_slot.set_virtual_link( LIST_REAL );
  line_mode = board->line_mode;
  line_style = board->line_style;
  line_width = board->line_width;
  arrow_size = board->arrow_size;
  ball_size = board->ball_size;
}

//
// コピーコンストラクタ
//
Spline::Spline( Spline* spline ): Shape( spline )
{
  point_slot = spline->point_slot;
  for( int i = 0 ; i < point_slot.count() ; i++ )
    point_slot[i] = new Point( point_slot[i] );
  wpoint_slot = spline->wpoint_slot;
  for( i = 0 ; i < wpoint_slot.count() ; i++ )
    wpoint_slot[i] = new Point( wpoint_slot[i] );
  line_mode = spline->line_mode;
  line_style = spline->line_style;
  line_width = spline->line_width;
  arrow_size = spline->arrow_size;
  ball_size = spline->ball_size;
}

//
// draw():
//
void Spline::draw( Window window, int x, int y )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;

  XSetLineAttributes( dpy, gc, line_width, LineSolid, CapButt, JoinMiter );

  Point* point1 = wpoint_slot.get( wpoint_slot.count()-1 );
  Point* point2 = wpoint_slot.get( wpoint_slot.count()-2 );
  double dx = point1->x - point2->x;
  double dy = point1->y - point2->y;
  double angle = get_angle( dx, dy );

  switch( line_mode ){
  case LM_FARROW:
  case LM_BARROW:
    draw_arrow( dpy, window, gc,
	       ROUND(point1->x)+x, ROUND(point1->y)+y, angle, arrow_size );
    break;
  case LM_MMASSOC:
  case LM_OMASSOC:
    draw_ball( dpy, window, gc,
	      ROUND(point1->x)+x, ROUND(point1->y)+y, angle, ball_size );
    break;
  }

  point1 = wpoint_slot.get( 0 );
  point2 = wpoint_slot.get( 1 );
  dx = point1->x - point2->x;
  dy = point1->y - point2->y;
  angle = get_angle( dx, dy );

  switch( line_mode ){
  case LM_RARROW:
  case LM_BARROW:
    draw_arrow( dpy, window, gc,
	       ROUND(point1->x)+x, ROUND(point1->y)+y, angle, arrow_size );
    break;
  case LM_MMASSOC:
    draw_ball( dpy, window, gc,
	      ROUND(point1->x)+x, ROUND(point1->y)+y, angle, ball_size );
    break;
  }

  switch( line_style ){
  case LS_SOLID:
    XSetLineAttributes( dpy, gc, line_width, LineSolid, CapButt, JoinMiter );
    break;
  case LS_DOT:
    XSetDashes( dpy, gc, 0, dot_pat, 2 );
    XSetLineAttributes(dpy, gc, line_width, LineOnOffDash, CapButt, JoinMiter);
    break;
  case LS_DASH:
    XSetDashes( dpy, gc, 0, dash_pat, 2 );
    XSetLineAttributes(dpy, gc, line_width, LineOnOffDash, CapButt, JoinMiter);
    break;
  }

  XPoint points[ wpoint_slot.count() ];
  for( int i = 0 ; i < wpoint_slot.count() ; i++ ){
    Point* p = wpoint_slot.get(i);
    points[ i ].x = ROUND(p->x)+x, points[ i ].y = ROUND(p->y)+y;
  }
  XDrawLines( dpy, window, gc, points, wpoint_slot.count(), CoordModeOrigin );
  XSetLineAttributes( dpy, gc, 1, LineSolid, CapButt, JoinMiter );
  if( board->grip_disp )
    draw_gp( window, x, y );
}

//
// hit():
//
Boolean Spline::hit( int x, int y, int hw )
{
  int last_x, last_y;
  for( int i = 0 ; i < wpoint_slot.count() ; i++ ){
    Point* point = wpoint_slot.get( i );
    if( contain_chk( ROUND(point->x), ROUND(point->y), x, y, hw ) )
      return True;
  }
  for( i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    if( contain_chk( ROUND(point->x), ROUND(point->y), x, y, hw ) )
      return True;
  }
  return False;
}

//
// layout():
//
EditResult Spline::layout( XEvent* event )
{
  Display* dpy = event->xany.display;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  static int last_x = -1, last_y;
  if( event->type == MotionNotify && proc_phase > 0 ){
    Window window = event->xmotion.window;
    int mx = event->xmotion.x, my = event->xmotion.y;
    board->quontize( mx, my );
    if( last_x != mx || last_y != my ){
      Point* lp = point_slot.get( point_slot.count()-1 );
      if( last_x >= 0 ){
	XSetFunction( dpy, board->gc, GXclear );
	draw_ipoint( window );
	XSetFunction( dpy, board->gc, GXset );
      }
      if( point_slot.count() >= 1 ){
	point_slot.append( new Point( (double)mx, (double)my ) );
	make_spline( &point_slot, &ipoint_slot );
	draw_ipoint( window );
	point_slot.unlink( LIST_END, 0, 1 );
      }
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
    Point* p = new Point( (double)mx, (double)my );
    point_slot.append( p );
    proc_phase = 1;
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
    break;
  case 1: // 二回目以降のクリック。
    if( event->type != ButtonPress ){
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
    }
    window = event->xbutton.window;
    switch( event->xbutton.button ){
    case 1: // 左ボタン(点の追加)
      mx = event->xbutton.x, my = event->xbutton.y;
      board->quontize( mx, my );
      p = point_slot.get( point_slot.count()-1 );
      if( ROUND(p->x) == mx && ROUND(p->y) == my ){
	XBell( dpy, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CONTINUE;
      }
      p = new Point( (double)mx, (double)my );
      point_slot.append( p );
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
      break;
    case 2: // 中ボタン(全ての点の指定完了)
      mx = event->xbutton.x, my = event->xbutton.y;
      board->quontize( mx, my );
      p = point_slot.get( point_slot.count()-1 );
      if( (ROUND(p->x) == mx && ROUND(p->y) == my)||point_slot.count() < 2 ){
	XBell( dpy, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CONTINUE;
      }
      XSetFunction( dpy, board->gc, GXclear );
      draw_ipoint( window );
      ipoint_slot.unlink( LIST_TOP, 0, ipoint_slot.count() );
      XSetFunction( dpy, board->gc, GXset );
      XSetPlaneMask( dpy, board->gc, board->shape_mask );
      p = new Point( (double)mx, (double)my );
      point_slot.append( p );
      make_spline( &point_slot, &wpoint_slot );
      draw( event->xbutton.window, 0, 0 );
      last_x = -1;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      proc_phase = 0;
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン(点を一つキャンセル)
      p = point_slot.get( point_slot.count()-1 );
      XSetFunction( dpy, board->gc, GXclear );
      DrawX( dpy, window, board->gc, ROUND(p->x), ROUND(p->y) );
      if( last_x >= 0 )
	DrawX( dpy, window, board->gc, last_x, last_y );
      draw_ipoint( window );
      ipoint_slot.unlink( LIST_TOP, 0, ipoint_slot.count() );
      XSetFunction( dpy, board->gc, GXset );
      last_x = -1;
      point_slot.unlink( LIST_END, 0, 1 );
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      if( point_slot.count() == 0 ){
	proc_phase = 0, last_x = -1;
	return EDIT_CANCEL;
      }
      return EDIT_CONTINUE;
      break;
    }
    break;
  }
  return EDIT_CONTINUE;
}

//
// resize():
//
EditResult Spline::resize( XEvent* event )
{
  Display* dpy = event->xany.display;
  Window window = event->xany.window;
  GC gc = board->gc;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  static int move_point;
  static int last_x = -1, last_y;
  static double save_x, save_y;
  if( proc_phase == 1 && event->type == MotionNotify ){
    int mx = event->xmotion.x, my = event->xmotion.y;
    board->quontize( mx, my );
    if( last_x == mx && last_y == my ){
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      return EDIT_CONTINUE;
    }
    if( last_x > 0 ){
      XSetFunction( dpy, gc, GXclear );
      draw_ipoint( window );
      XSetFunction( dpy, board->gc, GXset );
    }
    Point* p = point_slot.get( move_point );
    p->x = (double)mx, p->y = (double)my;
    make_spline( &point_slot, &ipoint_slot );
    draw_ipoint( window );
    last_x = mx, last_y = my;
    XSetFunction( dpy, gc, GXcopy );
    XSetPlaneMask( dpy, gc, ~0 );
    return EDIT_CONTINUE;
  }

  switch( proc_phase ){
  case 0:
    int mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    for( int i = 1 ; i < 8 ; i++ ){
      for( move_point = 0 ; move_point < point_slot.count() ; move_point++ ){
	Point* p = point_slot.get(move_point);
	if( contain_chk( ROUND(p->x), ROUND(p->y), mx, my, i ) )
	  break;
      }
      if( move_point < point_slot.count() )
	break;
    }
    XSetFunction( dpy, gc, GXcopy );
    XSetPlaneMask( dpy, gc, ~0 );
    last_x = -1;
    if( i >= 8 )
      return EDIT_CANCEL;
    Point* p = point_slot.get( move_point );
    save_x = p->x, save_y = p->y;
    make_spline( &point_slot, &ipoint_slot );
    proc_phase = 1;
    return EDIT_CONTINUE;
    break;
  case 1:
    if( event->type != ButtonPress ){
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      return EDIT_CONTINUE;
    }
    mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    switch( event->xbutton.button ){
    case 1: // 左ボタン(決定)
      if( last_x < 0 ){
	XBell( dpy, 0 );
	XSetFunction( dpy, gc, GXcopy );
	XSetPlaneMask( dpy, gc, ~0 );
	return EDIT_CONTINUE;
      }
      XSetFunction( dpy, gc, GXclear );
      draw_ipoint( window );
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, board->shape_mask );
      draw( window, 0, 0 );
      Point* point = point_slot.get( move_point );
      point->x = (double)mx, point->y = (double)my;
      XSetFunction( dpy, gc, GXset );
      make_spline( &point_slot, &wpoint_slot );
      draw( window, 0, 0 );
      proc_phase = 0;
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン(キャンセル)
      if( last_x > 0 ){
	XSetFunction( dpy, gc, GXclear );
	draw_ipoint( window );
	Point* point = point_slot.get( move_point );
	point->x = save_x, point->y = save_y;
      }
      proc_phase = 0;
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      return EDIT_CANCEL;
      break;
    }
  }
  return EDIT_CONTINUE;
}


//
// save():
//
void Spline::save( FILE* fp )
{
  fprintf( fp, "spline{\n" );
  fprintf( fp, "  line_style = %s;\n", ls_string(line_style) );
  fprintf( fp, "  line_mode = %s;\n", lm_string(line_mode) );
  fprintf( fp, "  arrow_size = %g;\n", arrow_size );
  fprintf( fp, "  ball_size = %g;\n", ball_size );
  if( !board->msdos_compatibility ){
    fprintf( fp, "  line_width = %d;\n", line_width );
  }
  fprintf( fp, "  node = " );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    fprintf( fp, "(%g,%g)", point->x, point->y );
    if( i > 0 && i % 8 == 0 )
      fprintf( fp, "\n           " );
  }
  fprintf( fp, ";\n" );
  fprintf( fp, "}\n");
}

//
// tex():
//
void Spline::tex( FILE* fp, double h, double x, double y )
{
  fprintf( fp, "\\spline" );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    fprintf( fp, "(%g,%g)", point->x+x, h - (point->y+y) );
  }
  fprintf( fp, "\n" );

  Point* point1 = wpoint_slot.get( wpoint_slot.count()-1 );
  Point* point2 = wpoint_slot.get( wpoint_slot.count()-2 );
  double dx = point1->x - point2->x;
  double dy = point1->y - point2->y;
  double angle = get_angle( dx, dy );

  switch( line_mode ){
  case LM_FARROW:
  case LM_BARROW:
    tex_arrow( fp, h, point1->x+x, point1->y+y, angle, 8, ROUND(line_width) );
    break;
  case LM_MMASSOC:
  case LM_OMASSOC:
    tex_ball( fp, h, point1->x+x, point1->y+y, angle, 6 );
    break;
  }

  point1 = wpoint_slot.get( 0 );
  point2 = wpoint_slot.get( 1 );
  dx = point1->x - point2->x;
  dy = point1->y - point2->y;
  angle = get_angle( dx, dy );

  switch( line_mode ){
  case LM_RARROW:
  case LM_BARROW:
    tex_arrow( fp, h, point1->x+x, point1->y+y, angle, 8, ROUND(line_width) );
    break;
  case LM_MMASSOC:
    tex_ball( fp, h, point1->x+x, point1->y+y, angle, 6 );
    break;
  }
}

//
// bound():
//
void Spline::bound( double& x, double& y, double& w, double& h )
{
  double min_x = 10000, max_x = 0, min_y = 10000, max_y = 0;

  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    if( point->x < min_x ) min_x = point->x;
    if( point->y < min_y ) min_y = point->y;
    if( point->x > max_x ) max_x = point->x;
    if( point->y > max_y ) max_y = point->y;
  }
  x = min_x, y = min_y;
  w = max_x - min_x, h = max_y - min_y;
}

//
// translate():
//
void Spline::translate( double x, double y )
{
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    point->x += x, point->y += y;
  }
  for( i = 0 ; i < wpoint_slot.count() ; i++ ){
    Point* point = wpoint_slot.get( i );
    point->x += x, point->y += y;
  }
}

//
// contain():
//
Boolean Spline::contain( int x, int y, int w, int h )
{
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    if( ROUND(point->x) < x || ROUND(point->x) >= x+w )
      return False;
    if( ROUND(point->y) < y || ROUND(point->y) >= y+h )
      return False;
  }
  return True;
}

//
// duplicate():
//
Shape* Spline::duplicate()
{
  return new Spline( this );
}


//
// scale():
//
void Spline::scale( double rx, double ry )
{
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    point->x *= rx, point->y *= ry;
  }
  make_spline( &point_slot, &wpoint_slot );
}

/*********************** From xfig ******************************************/

/* utilities used by spline drawing routines */

#define		STACK_DEPTH	      20

class Element{
public:
  double x1, y1, x2, y2, x3, y3, x4, y4;
  Element( double _x1, double _y1, double _x2, double _y2,
	  double _x3, double _y3, double _x4, double _y4){
    x1 = _x1, y1 = _y1, x2 = _x2, y2 = _y2;
    x3 = _x3, y3 = _y3, x4 = _x4, y4 = _y4;
  }
};

static Stack<Element> element_stack;

/********************* CURVES FOR SPLINES *****************************

	The following spline drawing routine is from

	"An Algorithm for High-Speed Curve Generation"
	by George Merrill Chaikin,
	Computer Graphics and Image Processing, 3, Academic Press,
	1974, 346-349.

	and

	"On Chaikin's Algorithm" by R. F. Riesenfeld,
	Computer Graphics and Image Processing, 4, Academic Press,
	1975, 304-310.

***********************************************************************/

#define		half(z1, z2)	((z1+z2)/2.0)
#define		THRESHOLD	5

/* iterative version */
/*
 * because we draw the spline with small line segments, the style parameter
 * doesn't work
 */

static void
quadratic_spline(
		 double a1, double b1, double a2, double b2,
		 double a3, double b3, double a4, double b4,
		 List<Point>* wpoint_slot )
{
  element_stack.push( new Element(a1, b1, a2, b2, a3, b3, a4, b4) );
  while ( element_stack.count() > 0 ) {
    Element* e = element_stack.pop();
    double xmid = half(e->x2, e->x3);
    double ymid = half(e->y2, e->y3);
    if (fabs(e->x1 - xmid) < THRESHOLD && fabs(e->y1 - ymid) < THRESHOLD &&
	fabs(xmid - e->x4) < THRESHOLD && fabs(ymid - e->y4) < THRESHOLD) {
      wpoint_slot->append( new Point( e->x1, e->y1 ) );
      wpoint_slot->append( new Point( xmid, ymid ) );
    }else{
      element_stack.push( new Element( xmid, ymid,
				      half(xmid, e->x3), half(ymid, e->y3),
				      half(e->x3, e->x4), half(e->y3, e->y4),
				      e->x4, e->y4 ) );
      element_stack.push( new Element( e->x1, e->y1,
				      half(e->x1, e->x2), half(e->y1, e->y2),
				      half(e->x2, xmid), half(e->y2, ymid),
				      xmid, ymid ) );
    }
    delete e;
  }
}

/*
 * the style parameter doesn't work for splines because we use small line
 * segments
 */
#if 0
static void bezier_spline(a0, b0, a1, b1, a2, b2, a3, b3, wpoint_slot )
float a0, b0, a1, b1, a2, b2, a3, b3;
ListClass* wpoint_slot;
{
  register float  tx, ty;
  float           x0, y0, x1, y1, x2, y2, x3, y3;
  float           sx1, sy1, sx2, sy2, tx1, ty1, tx2, ty2, xmid, ymid;

  clear_stack();
  push(a0, b0, a1, b1, a2, b2, a3, b3);

  while (pop(&x0, &y0, &x1, &y1, &x2, &y2, &x3, &y3)) {
    if (fabs(x0 - x3) < THRESHOLD && fabs(y0 - y3) < THRESHOLD) {
      wpoint_slot->append( wpoint_slot, Point__create(ROUND(x0),ROUND(y0)) );
    } else {
      tx = half(x1, x2);
      ty = half(y1, y2);
      sx1 = half(x0, x1);
      sy1 = half(y0, y1);
      sx2 = half(sx1, tx);
      sy2 = half(sy1, ty);
      tx2 = half(x2, x3);
      ty2 = half(y2, y3);
      tx1 = half(tx2, tx);
      ty1 = half(ty2, ty);
      xmid = half(sx2, tx1);
      ymid = half(sy2, ty1);

      push(xmid, ymid, tx1, ty1, tx2, ty2, x3, y3);
      push(x0, y0, sx1, sy1, sx2, sy2, xmid, ymid);
    }
  }
}
#endif /* 0 */

//
// make_spline( &point_slot, &wpoint_slot ):
//
void Spline::make_spline( List<Point>* point_slot, List<Point>* wpoint_slot )
{
  Point* p1 = point_slot->get( 0 );
  double x1 = p1->x, y1 = p1->y;
  Point* p2 = point_slot->get( 1 );
  double x2 = p2->x, y2 = p2->y;
  double cx1 = (x1 + x2) / 2, cy1 = (y1 + y2) / 2;
  double cx2 = (cx1 + x2) / 2, cy2 = (cy1 + y2) / 2;

  if( wpoint_slot->count() > 0 )
    wpoint_slot->unlink( LIST_TOP, 0, wpoint_slot->count() );
  wpoint_slot->append( new Point( x1, y1 ) );

  for ( int i = 2 ; i < point_slot->count() ; i++ ){
    Point* p = point_slot->get( i );
    x1 = x2, y1 = y2;
    x2 = p->x, y2 = p->y;
    double cx4 = (x1 + x2) / 2, cy4 = (y1 + y2) / 2;
    double cx3 = (x1 + cx4) / 2, cy3 = (y1 + cy4) / 2;
    quadratic_spline( cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4, wpoint_slot );
    cx1 = cx4;
    cy1 = cy4;
    cx2 = (cx1 + x2) / 2;
    cy2 = (cy1 + y2) / 2;
  }
  wpoint_slot->append( new Point( cx1, cy1 ) );
  wpoint_slot->append( new Point( x2, y2 ) );
}

//
// reversx():
//
void Spline::reversx()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    double x = point->x-bx;
    point->x = bx+bw - x;
  }
  make_spline( &point_slot, &wpoint_slot );
}
  
//
// reversy():
//
void Spline::reversy()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    double y = point->y-by;
    point->y = by+bh - y;
  }
  make_spline( &point_slot, &wpoint_slot );
}

//
// update():
//
void Spline::update()
{
  if( line_mode != board->line_mode || arrow_size != board->arrow_size ||
     ball_size != board->ball_size || line_width != board->line_width ||
     line_style != board->line_style ){
    line_mode = board->line_mode;
    line_style = board->line_style;
    line_width = board->line_width;
    arrow_size = board->arrow_size;
    ball_size = board->ball_size;
  }
}

//
// xfig():
//
void Spline::xfig( FILE* fp, double x, double y )
{
  fprintf( fp, "%d 0 %d ", O_SPLINE, SOLID_LINE );/* open_normal type */
  fprintf( fp, "1 -1 0 0 " ); /* thickness=1, color,depth,pen=nouse */
  fprintf( fp, "%d 0.0 ", UNFILLED ); /* style_val=0.0 */
  switch( line_mode ){
  case LM_MMASSOC:
  case LM_OMASSOC:
  case LM_SOLID:  fprintf( fp, "0 0\n" );break;
  case LM_FARROW: fprintf( fp, "1 0\n" );break;
  case LM_RARROW: fprintf( fp, "0 1\n" );break;
  case LM_BARROW: fprintf( fp, "1 1\n" );break;
  }
  if( line_mode == LM_FARROW || line_mode == LM_BARROW ){
    fprintf( fp, "0 0 1 %d %d\n",
	    /* arrow_type, arrow_style = nouse, arrow_thickness = 1 */
	    ROUND(XFS(2.0*arrow_size*sin(.35))), /* arrow_width */
	    ROUND(XFS(arrow_size*cos(.35))) );   /* arrow_height */
  }
  if( line_mode == LM_RARROW || line_mode == LM_BARROW ){
    fprintf( fp, "0 0 1 %d %d\n",
	    /* arrow_type, arrow_style = nouse, arrow_thickness = 1 */
	    ROUND(XFS(2.0*arrow_size*sin(.35))), /* arrow_width */
	    ROUND(XFS(arrow_size*cos(.35))) );   /* arrow_height */
  }
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    fprintf( fp, "%d %d ", ROUND(XFS(point->x+x)), ROUND(XFS(point->y+y)) );
  }
  fprintf( fp, "9999 9999\n" );
}

//
// draw_ipoint():
//
void Spline::draw_ipoint( Window window )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;
  XPoint points[ ipoint_slot.count() ];
  for( int i = 0 ; i < ipoint_slot.count() ; i++ ){
    Point* p = ipoint_slot.get(i);
    points[ i ].x = ROUND(p->x), points[ i ].y = ROUND(p->y);
  }
  XDrawLines( dpy, window, gc, points, ipoint_slot.count(), CoordModeOrigin );
}

//
// draw_gp():
//
void Spline::draw_gp( Window window, int x, int y )
{
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    XDrawArc( board->xcontext->get_dpy(), window, board->gc,
	     ROUND(point->x)+x-2, ROUND(point->y)+y-2, 4, 4, 0, 360*64 );
  }
}

//
// rotate():
//
void Spline::rotate( XEvent* event )
{
  int mx = event->xbutton.x, my = event->xbutton.y;
  board->quontize( mx, my );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    point->x -= (double)mx;
    point->y -= (double)my;
  }
  for( i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    double tmp_x = point->x;
    point->x = point->y;
    point->y = -tmp_x;
  }
  for( i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    point->x += (double)mx;
    point->y += (double)my;
  }
  make_spline( &point_slot, &wpoint_slot );
}
