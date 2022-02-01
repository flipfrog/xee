//
// line.cc:
//
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
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
#include <line.h>
#include <misc.h>
#include <geometry.h>

#include <xfig.h>

#define PAI 3.141592
#define RADIAN(x) (((x)/360.0)*PAI*2.0)

//
// コンストラクタ
//
Line::Line( Board* board ): Shape( board )
{
  shape_type = ST_LINE;
  point_slot.set_virtual_link( LIST_REAL );
  line_mode = board->line_mode;
  line_style = board->line_style;
  line_width = board->line_width;
  arrow_size = board->arrow_size;
  ball_size = board->ball_size;
}

//
// draw():
//
void Line::draw( Window window, int x, int y )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;

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

  XPoint points[ point_slot.count() ];
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* p = point_slot.get(i);
    points[ i ].x = ROUND(p->x)+x, points[ i ].y = ROUND(p->y)+y;
  }
  XDrawLines( dpy, window, gc, points, point_slot.count(), CoordModeOrigin );

  Point* point1 = point_slot.get( point_slot.count()-1 );
  Point* point2 = point_slot.get( point_slot.count()-2 );
  double dx = point1->x - point2->x;
  double dy = point1->y - point2->y;
  double angle = get_angle( dx, dy );

  XSetLineAttributes( dpy, gc, line_width, LineSolid, CapButt, JoinMiter );
  switch( line_mode ){
  case LM_FARROW:
  case LM_BARROW:
    draw_arrow( dpy, window, gc, ROUND(point1->x)+x, ROUND(point1->y)+y,
	       angle, arrow_size );
    break;
  case LM_MMASSOC:
  case LM_OMASSOC:
    draw_ball( dpy, window, gc, ROUND(point1->x)+x, ROUND(point1->y)+y,
	      angle, ball_size/2 );
    break;
  }

  point1 = point_slot.get( 0 );
  point2 = point_slot.get( 1 );
  dx = point1->x - point2->x;
  dy = point1->y - point2->y;
  angle = get_angle( dx, dy );

  switch( line_mode ){
  case LM_RARROW:
  case LM_BARROW:
    draw_arrow( dpy, window, gc, ROUND(point1->x)+x, ROUND(point1->y)+y,
	       angle, arrow_size );
    break;
  case LM_MMASSOC:
    draw_ball( dpy, window, gc, ROUND(point1->x)+x, ROUND(point1->y)+y,
	      angle, ball_size/2 );
    break;
  }
  XSetLineAttributes( dpy, gc, 1, LineSolid, CapButt, JoinMiter );
}

//
// hit():
//
Boolean Line::hit( int x, int y, int hw )
{
  int last_x, last_y;
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    int ix = ROUND(point->x), iy = ROUND(point->y);
    if( contain_chk( ix, iy, x, y, hw ) )
      return True;
    if( i > 0 )
      if( check_on_the_line( x, y, ix, iy, last_x, last_y, hw ) )
	return True;
    last_x = ix, last_y = iy;
  }
  return False;
}

//
// layout():
//
EditResult Line::layout( XEvent* event )
{
  Display* dpy = event->xany.display;
  Window window = event->xany.window;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  static int last_x = -1, last_y;
  if( event->type == MotionNotify && proc_phase > 0 ){
    int mx = event->xmotion.x, my = event->xmotion.y;
    board->quontize( mx, my );
    if( last_x != mx || last_y != my ){
      Point* lp = point_slot.get( point_slot.count()-1 );
      if( last_x >= 0 ){
	XSetFunction( dpy, board->gc, GXclear );
	XDrawLine( dpy, window, board->gc,
		  ROUND(lp->x), ROUND(lp->y), last_x, last_y );
	XSetFunction( dpy, board->gc, GXset );
      }
      XDrawLine( dpy, window, board->gc, ROUND(lp->x), ROUND(lp->y), mx, my );
      last_x = mx, last_y = my;
    }
    return EDIT_CONTINUE;
  }
  switch( proc_phase ){
  case 0: // 一回目のクリック。
    if( event->type != ButtonPress && event->xbutton.button != 1 )
      return EDIT_CONTINUE;
    int mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    Point* point = new Point( (double)mx, (double)my );
    point_slot.append( point );
    proc_phase = 1;
    return EDIT_CONTINUE;
    break;
  case 1: // 二回目以降のクリック。
    if( event->type != ButtonPress )
      return EDIT_CONTINUE;
    switch( event->xbutton.button ){
    case 1: // 左ボタン(点の追加)
      mx = event->xbutton.x, my = event->xbutton.y;
      board->quontize( mx, my );
      point = point_slot.get( point_slot.count()-1 );
      if( ROUND(point->x) == mx && ROUND(point->y) == my ){
	XBell( dpy, 0 );
	return EDIT_CONTINUE;
      }
      point = new Point( (double)mx, (double)my );
      point_slot.append( point );
      return EDIT_CONTINUE;
      break;
    case 2: // 中ボタン(全ての点の指定完了)
      mx = event->xbutton.x, my = event->xbutton.y;
      board->quontize( mx, my );
      point = point_slot.get( point_slot.count()-1 );
      if( ROUND(point->x) == mx && ROUND(point->y) == my ){
	XBell( dpy, 0 );
	return EDIT_CONTINUE;
      }
      XSetFunction( dpy, board->gc, GXclear );
      if( last_x >= 0 ){
	XDrawLine( dpy, event->xmotion.window, board->gc,
		  ROUND(point->x), ROUND(point->y), last_x, last_y );
	if( point_slot.count() >= 2 )
	  draw( event->xbutton.window, 0, 0 );
      }
      for( int i = 1 ; i < point_slot.count() ; i++ ){
	Point* p1 = point_slot.get(i-1);
	Point* p2 = point_slot.get(i-0);
	XDrawLine( dpy, event->xmotion.window, board->gc,
		  ROUND(p1->x), ROUND(p1->y), ROUND(p2->x), ROUND(p2->y) );
      }
      point = new Point( (double)mx, (double)my );
      point_slot.append( point );
      XSetFunction( dpy, board->gc, GXset );
      XSetPlaneMask( dpy, board->gc, board->shape_mask );
      draw( event->xbutton.window, 0, 0 );      
      last_x = -1;
      proc_phase = 0;
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン(点を一つキャンセル)
      point = point_slot.get( point_slot.count()-1 );
      XSetFunction( dpy, board->gc, GXclear );
      if( last_x >= 0 )
	XDrawLine( dpy, event->xmotion.window, board->gc,
		  ROUND(point->x), ROUND(point->y), last_x, last_y );

      if( point_slot.count() > 1 ){
	XSetFunction( dpy, board->gc, GXclear );
	XDrawLine( dpy, window, board->gc,
		  ROUND(point_slot.get( point_slot.count()-2 )->x),
		  ROUND(point_slot.get( point_slot.count()-2 )->y),
		  ROUND(point->x), ROUND(point->y) );
      }
      point_slot.unlink( LIST_TOP, point_slot.count()-1, 1 );
      if( point_slot.count() == 0 ){
	last_x = -1;
	proc_phase = 0;
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
// save():
//
void Line::save( FILE* fp )
{
  fprintf( fp, "line{\n" );
  fprintf( fp, "  line_style = %s;\n", ls_string(line_style) );
  fprintf( fp, "  line_mode = %s;\n", lm_string(line_mode) );
  fprintf( fp, "  arrow_size = %g;\n", arrow_size );
  fprintf( fp, "  ball_size = %g;\n", ball_size );
  if( !board->msdos_compatibility )
    fprintf( fp, "  line_width = %d;\n", line_width );
  fprintf( fp, "  node = " );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
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
void Line::tex( FILE* fp, double h, double x, double y )
{
  switch( line_style ){
  case LS_SOLID: fprintf( fp, "\\path" ); break;
  case LS_DOT:   fprintf( fp, "\\dottedline{5}" ); break;
  case LS_DASH:  fprintf( fp, "\\dashline{4.0}" ); break;
  }
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    fprintf( fp, "(%g,%g)", point->x+x, h - (point->y+y) );
  }
  fprintf( fp, "\n" );

  Point* point1 = point_slot.get( point_slot.count()-1 );
  Point* point2 = point_slot.get( point_slot.count()-2 );
  double dx = point1->x - point2->x;
  double dy = point1->y - point2->y;
  double angle = get_angle( dx, dy );

  switch( line_mode ){
  case LM_FARROW:
  case LM_BARROW:
    tex_arrow( fp, h, point1->x+x, point1->y+y, angle, 8, (double)line_width );
    break;
  case LM_MMASSOC:
  case LM_OMASSOC:
    tex_ball( fp, h, point1->x+x, point1->y+y, angle, 7 );
    break;
  }

  point1 = point_slot.get( 0 );
  point2 = point_slot.get( 1 );

  dx = point1->x - point2->x;
  dy = point1->y - point2->y;
  angle = get_angle( dx, dy );

  switch( line_mode ){
  case LM_RARROW:
  case LM_BARROW:
    tex_arrow( fp, h, point1->x+x, point1->y+y, angle, 8, (double)line_width );
    break;
  case LM_MMASSOC:
    tex_ball( fp, h, point1->x+x, point1->y+y, angle, 6 );
    break;
  }
}

//
// bound():
//
void Line::bound( double& x, double& y, double& w, double& h )
{
  double min_x = INT_MAX, max_x = INT_MIN, min_y = INT_MAX, max_y = INT_MIN;

  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
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
void Line::translate( double x, double y )
{
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get( i );
    point->x += x, point->y += y;
  }
}

//
// duplicate():
//
Shape* Line::duplicate()
{
  return new Line( this );
}

//
// コピーコンストラクタ
//
Line::Line( Line* line ): Shape( line )
{
  shape_type = ST_LINE;
  point_slot.set_virtual_link( LIST_REAL );
  line_mode = line->line_mode;
  line_style = line->line_style;
  line_width = line->line_width;
  arrow_size = line->arrow_size;
  ball_size = line->ball_size;
  for( int i = 0 ; i < line->point_slot.count() ; i++ )
    point_slot.append( new Point( line->point_slot.get(i) ) );
}

//
// contain():
//
Boolean Line::contain( int x, int y, int w, int h )
{
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    if( ( ROUND(point->x) < x || ROUND(point->x) >= x+w ) ||
       ( ROUND(point->y) < y || ROUND(point->y) >= y+h ) )
	return False;
  }
  return True;
}

//
// scale():
//
void Line::scale( double rx, double ry )
{
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    point->x *= rx, point->y *= ry;
  }
  arrow_size *= ry;
  ball_size *= ry;
}

//
// resize():
//
EditResult Line::resize( XEvent* event )
{
  Display* dpy = event->xany.display;
  Window window = event->xany.window;
  GC gc = board->gc;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  static int move_point;
  static int last_x = -1, last_y;
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
      if( move_point > 0 ){
	Point* p = point_slot.get( move_point-1 );
	XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), last_x, last_y );
      }
      if( move_point < point_slot.count()-1 ){
	Point* p = point_slot.get( move_point+1 );
	XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), last_x, last_y );
      }
      XSetFunction( dpy, board->gc, GXset );
    }
    if( move_point > 0 ){
      Point* p = point_slot.get( move_point-1 );
      XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), mx, my );
    }
    if( move_point < point_slot.count()-1 ){
      Point* p = point_slot.get( move_point+1 );
      XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), mx, my );
    }
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
      if( move_point > 0 ){
	Point* p = point_slot.get( move_point-1 );
	XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), mx, my );
      }
      if( move_point < point_slot.count()-1 ){
	Point* p = point_slot.get( move_point+1 );
	XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), mx, my );
      }
      XSetPlaneMask( dpy, gc, board->shape_mask );
      draw( window, 0, 0 );
      Point* point = point_slot.get( move_point );
      point->x = (double)mx, point->y = (double)my;
      XSetFunction( dpy, gc, GXset );
      draw( window, 0, 0 );
      proc_phase = 0;
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン(キャンセル)
      if( last_x > 0 ){
	XSetFunction( dpy, gc, GXclear );
	if( move_point > 0 ){
	  Point* p = point_slot.get( move_point-1 );
	  XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), mx, my );
	}
	if( move_point < point_slot.count()-1 ){
	  Point* p = point_slot.get( move_point+1 );
	  XDrawLine( dpy, window, gc, ROUND(p->x), ROUND(p->y), mx, my );
	}
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
// reversx():
//
void Line::reversx()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    double x = point->x - bx;
    point->x = bx + bw - x;
  }
}
  
//
// reversy():
//
void  Line::reversy()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    double y = point->y - by;
    point->y = by + bh - y;
  }
}

//
// rotate():
//
void  Line::rotate( XEvent* event )
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
}

//
// update():
//
void Line::update()
{
  if( line_mode != board->line_mode || line_style != board->line_style ||
     arrow_size != board->arrow_size || ball_size != board->ball_size ||
     line_width != board->line_width ){
    line_mode = board->line_mode;
    line_style = board->line_style;
    arrow_size = board->arrow_size;
    ball_size = board->ball_size;
    line_width = board->line_width;
  }
}

//
// xfig():
//
void Line::xfig( FILE* fp, double x, double y )
{
  fprintf( fp, "%d %d ", O_POLYLINE, T_POLYLINE );
  switch( line_style ){
  case LS_SOLID: fprintf( fp, "%d ", SOLID_LINE ); break;
  case LS_DASH:  fprintf( fp, "%d ", DASHED_LINE ); break;
  case LS_DOT:   fprintf( fp, "%d ", DOTTED_LINE ); break;
  }
  fprintf( fp, "%d -1 0 0 ", line_width ); /* width,color,depth,pen=nouse */
  fprintf( fp, "%d 0 -1 ", UNFILLED ); /* style_val = 0.0, radius = -1 */
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
	    ROUND(XFS(2.0*arrow_size*sin(.35))),   /* arrow_width */
	    ROUND(XFS(arrow_size*cos(.35))) );     /* arrow_height */
  }
  if( line_mode == LM_RARROW || line_mode == LM_BARROW ){
    fprintf( fp, "0 0 1 %d %d\n",
	   /* arrow_type, arrow_style = nouse, arrow_thickness = 1 */
	    ROUND(XFS(2.0*arrow_size*sin(.35))),   /* arrow_width */
	    ROUND(XFS(arrow_size*cos(.35))) );     /* arrow_height */
  }
  for( int i = 0 ; i < point_slot.count() ; i++ ){
    Point* point = point_slot.get(i);
    fprintf( fp, "%d %d ", ROUND(XFS(point->x+x)), ROUND(XFS(point->y+y)) );
  }
  fprintf( fp, "9999 9999\n" );
}
