//
// arc.c:
//
#include <stdio.h>
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
#include <arc.h>
#include <misc.h>
#include <geometry.h>

#include <xfig.h>

#define PAI 3.141592
#define PAI_2 (PAI*2)
#define RADIAN(x) (((double)(x)/360.0)*PAI_2)
#define DEGREE(x) ROUND((x)/PAI_2*360.0)

//
// コンストラクタ
//
Arc::Arc( Board* board ): Shape( board )
{
  shape_type = ST_ARC;
  _x = _y = 0;
  rrot = False;
  line_mode = board->line_mode;
  line_style = board->line_style;
  line_width = board->line_width;
  arrow_size = board->arrow_size;;
  ball_size = board->ball_size;
}

//
// draw():
//
void Arc::draw( Window window, int x, int y )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;
  int sx = ROUND(_x+sdx)+x, sy = ROUND(_y+sdy)+y;
  int ex = ROUND(_x+edx)+x, ey = ROUND(_y+edy)+y;
  double _sdx = sdx, _sdy = sdy;
  double _edx = edx, _edy = edy;

  if( !rrot ){
    dswap( _sdx, _edx );
    dswap( _sdy, _edy );
  }

  double s_angle, e_angle;

  if( rrot )
    s_angle = get_angle( sdx, sdy ) - PAI/2;
  else
    s_angle = get_angle( sdx, sdy ) + PAI/2;

  XSetLineAttributes( dpy, gc, line_width, LineSolid, CapButt, JoinMiter );
  switch( line_mode ){
  case LM_RARROW:
  case LM_BARROW:
    draw_arrow( dpy, window, gc, sx, sy, s_angle, arrow_size  );
    break;
  case LM_OMASSOC:
  case LM_MMASSOC:
    draw_ball( dpy, window, gc, sx, sy, s_angle, ball_size );
    break;
  }

  if( rrot )
    e_angle = get_angle( edx, edy ) + PAI/2;
  else
    e_angle = get_angle( edx, edy ) - PAI/2;

  switch( line_mode ){
  case LM_FARROW:
  case LM_BARROW:
    draw_arrow( dpy, window, gc, ex, ey, e_angle, arrow_size );
    break;
  case LM_MMASSOC:
    draw_ball( dpy, window, gc, ex, ey, e_angle, ball_size );
    break;
  }

  s_angle = get_angle( sdx, -sdy );
  if( s_angle < 0 ) s_angle += PAI_2;
  e_angle = get_angle( edx, -edy );
  if( e_angle < 0 ) e_angle += PAI_2;
  int sa = DEGREE(s_angle)*64;
  int ea = DEGREE(e_angle)*64;
  int da;
  if( !rrot ){
    da = ea - sa;
    if( ea < sa ) da = ea + 360*64 - sa;
  }else{
    da = ea - sa;
    if( ea < sa ) da = ea + 360*64 - sa;
    da = da - 360*64;
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
  XDrawArc( dpy, window, gc, ROUND(_x-r)+x, ROUND(_y-r)+y,
	   ROUND(r*2), ROUND(r*2), sa, da );
  XSetLineAttributes(dpy, gc, 0, LineSolid, CapButt, JoinMiter);
  if( board->grip_disp )
    draw_gp( window, x, y );
}

//
// hit():
//
Boolean Arc::hit( int x, int y, int hw )
{
  for( int i = 0 ; i < 3 ; i++ )
    if( contain_chk( ROUND(xx[i]), ROUND(yy[i]), x, y, hw ) )
      return True;
  return False;
}

//
// layout():
//
EditResult Arc::layout( XEvent* event )
{
  Display* dpy = event->xany.display;
  static int last_x = -1, last_y = -1;
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  XSetFunction( dpy, board->gc, GXset );
  if( event->type == MotionNotify && proc_phase == 2 ){
    int mx = event->xmotion.x, my = event->xmotion.y;
    board->quontize( mx, my );
    if( last_x != mx || last_y != my ){
      xx[2] = (double)mx, yy[2] = (double)my;
      if( last_x >= 0 ){
	XSetFunction( dpy, board->gc, GXclear );
	draw( event->xmotion.window, 0, 0 );
      }
      if( compute_geom() ){
	XSetFunction( dpy, board->gc, GXset );
	draw( event->xmotion.window, 0, 0 );
	last_x = mx, last_y = my;
      }else{
	last_x = -1, last_y = -1;
      }
    }
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
  }
  if( event->type != ButtonPress ){
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
  }
  switch( proc_phase ){
  case 0:
    if( event->xbutton.button != 1 ){
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CANCEL;
    }
    Window window = event->xbutton.window;
    int mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    xx[0] = (double)mx, yy[0] = (double)my;
    XSetFunction( dpy, board->gc, GXset );
    DrawX( dpy, window, board->gc, mx, my );
    proc_phase = 1;
    XSetFunction( dpy, board->gc, GXcopy );
    XSetPlaneMask( dpy, board->gc, ~0 );
    return EDIT_CONTINUE;
    break;
  case 1:
    window = event->xbutton.window;
    switch( event->xbutton.button ){
    case 1: // 左ボタン
      int mx = event->xbutton.x, my = event->xbutton.y;
      board->quontize( mx, my );
      xx[1] = (double)mx, yy[1] = (double)my;
      if( xx[0] == xx[1] && yy[0] == yy[1] ){
	XBell( dpy, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CONTINUE;
      }
      DrawX( dpy, window, board->gc, mx, my );
      proc_phase = 2;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CONTINUE;
    case 3: // 右ボタン
      XSetFunction( dpy, board->gc, GXclear );
      DrawX( dpy, window, board->gc, ROUND(xx[0]), ROUND(yy[0]) );
      proc_phase = 0;
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      return EDIT_CANCEL;
      break;
    }
    break;
  case 2:
    window = event->xbutton.window;
    switch( event->xbutton.button ){
    case 1: // 左ボタン
      int mx = event->xbutton.x, my = event->xbutton.y;
      board->quontize( mx, my );
      if( (xx[0]==xx[1] && xx[1]==(double)mx)||
	 (yy[0]==yy[1] && yy[1]==(double)my) ){
	XBell( dpy, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CONTINUE;
	break;
      }
      XSetFunction( dpy, board->gc, GXclear );
      draw( window, 0, 0 );
      xx[2] = (double)mx, yy[2] = (double)my;
      if( !compute_geom() ){
	XBell( dpy, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_CONTINUE;
      }
      for( int i = 0 ; i < 2 ; i++ )
	DrawX( dpy, window, board->gc, ROUND(xx[i]), ROUND(yy[i]) );
      XSetPlaneMask( dpy, board->gc, board->shape_mask );
      XSetFunction( dpy, board->gc, GXset );
      draw( window, 0, 0 );
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      proc_phase = 0;
      return EDIT_COMPLETE;
      break;
    case 3: // 右ボタン
      XSetFunction( dpy, board->gc, GXclear );
      if( last_x >= 0 )
	draw( window, 0, 0 );
      DrawX( dpy, window, board->gc, ROUND(xx[1]), ROUND(yy[1]) );
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      proc_phase = 1;
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
void Arc::save( FILE* fp )
{
  fprintf( fp, "arc{\n" );
  fprintf( fp, "  geom2 = (%g,%g,%g,%g,%g,%g);\n",
	  xx[0], yy[0],
	  xx[1], yy[1],
	  xx[2], yy[2] );
  fprintf( fp, "  line_mode = %s;\n", lm_string(line_mode) );
  fprintf( fp, "  arrow_size = %g;\n", arrow_size );
  fprintf( fp, "  ball_size = %g;\n", ball_size );
  if( !board->msdos_compatibility ){
    fprintf( fp, "  line_style = %s;\n", ls_string(line_style) );
    fprintf( fp, "  line_width = %d;\n", line_width );
  }
  fprintf( fp, "}\n" );
}

//
// tex():
//
void Arc::tex( FILE* fp, double bh, double x, double y )
{
  double sx = _x + sdx + x, sy = _y + sdy + y;
  double ex = _x + edx + x, ey = _y + edy + y;
  double _sdx = sdx, _sdy = sdy, _edx = edx, _edy = edy;

  if( !rrot ){
    dswap( _sdx, _edx );
    dswap( _sdy, _edy );
  }

  double angle;
  if( rrot )
    angle = get_angle( _sdx, sdy ) - PAI/2;
  else
    angle = get_angle( _sdx, _sdy ) + PAI/2;
  switch( line_mode ){
  case LM_RARROW:
  case LM_BARROW:
    tex_arrow( fp, bh, sx, sy, angle, arrow_size, (double)line_width );
    break;
  case LM_OMASSOC:
  case LM_MMASSOC:
    tex_ball( fp, bh, sx, sy, angle, ball_size );
    break;
  }

  if( rrot )
    angle = get_angle( _edx, _edy ) + PAI/2;
  else
    angle = get_angle( _edx, _edy ) - PAI/2;
  switch( line_mode ){
  case LM_FARROW:
  case LM_BARROW:
    tex_arrow( fp, bh, ex, ey, angle, arrow_size, (double)line_width );
    break;
  case LM_MMASSOC:
    tex_ball( fp, bh, ex, ey, angle, ball_size );
    break;
  }
  fprintf( fp, "\\put(%g,%g){\\arc{%g}{%g}{%g}}\n",
	  _x+(double)x, bh-(_y+(double)y), r*2,
	  get_angle(_sdx,_sdy), get_angle(_edx,_edy) );
}

//
// bound():
//
void Arc::bound( double& x, double& y, double& w, double& h )
{
  double min_x = INT_MAX, max_x = 0, min_y = INT_MAX, max_y = 0;
  for( int i = 0 ; i < 3 ; i++ ){
    if( xx[i] < min_x ) min_x = xx[i];
    if( xx[i] > max_x ) max_x = xx[i];
    if( yy[i] < min_y ) min_y = yy[i];
    if( yy[i] > max_y ) max_y = yy[i];
  }
  x = min_x, y = min_y;
  w = max_x - min_x, h = max_y - min_y;
}

//
// translate():
//
void Arc::translate( double x, double y )
{
  for( int i = 0 ; i < 3 ; i++ )
    xx[i] += x, yy[i] += y;
  compute_geom();
}

//
// draw_gp():
//
void Arc::draw_gp( Window window, int x, int y )
{
  for( int i = 0 ; i < 3 ; i++ )
    XDrawArc( board->xcontext->get_dpy(), window, board->gc,
	     ROUND(xx[i])+x-2, ROUND(yy[i])+y-2,
	     4, 4,
	     0, 360*64 );
}

//
// duplicate():
//
Shape* Arc::duplicate()
{
  return new Arc( this );
}

//
// コピーコンストラクタ
//
Arc::Arc( Arc* arc ): Shape( arc )
{
  shape_type = ST_ARC;
  _x = arc->_x, _y = arc->_y;
  r = arc->r;
  sdx = arc->sdx, sdy = arc->sdy;
  edx = arc->edx, edy = arc->edy;
  rrot = arc->rrot;
  for( int i = 0 ; i < 3 ; i++ )
    xx[i] = arc->xx[i], yy[i] = arc->yy[i];
  line_width = arc->line_width;
  line_mode = arc->line_mode;
  line_style = arc->line_style;
  arrow_size = arc->arrow_size;
  ball_size = arc->ball_size;
}

//
// contain():
//
Boolean Arc::contain( int x, int y, int w, int h )
{
  for( int i = 0 ; i < 3 ; i++ ){
    int _x = ROUND(xx[i]), _y = ROUND(yy[i]);
    if( ( _x < x || _x >= x+w )||( _y < y || _y >= y+h ) )
      return False;
  }
  return True;
}

//
// scale():
//
void Arc::scale( double rx, double ry )
{
  for( int i = 0 ; i < 3 ; i++ ){
    xx[i] *= rx;
    yy[i] *= ry;
  }
  compute_geom();
  arrow_size *= ry;
  ball_size *= ry;
}

//
// compute_geom():
//
Boolean Arc::compute_geom()
{
  double cx1, cy1, cx2, cy2;
  double a1, b1, a2, b2;
  double dx, dy;
  Boolean a1_inf = False, a2_inf = False;
  double agl[3];

  cx1 = (xx[1]+xx[0])/2.0;
  cy1 = (yy[1]+yy[0])/2.0;
  cx2 = (xx[2]+xx[1])/2.0;
  cy2 = (yy[2]+yy[1])/2.0;

  if( yy[1] == yy[0] ){
    a1_inf = True;
    a1 = 0;
  }else{
    a1 = -(xx[1]-xx[0])/(yy[1]-yy[0]);
  }
  b1 = cy1 - a1 * cx1;

  if( yy[2] == yy[1] ){
    a2_inf = True;
    a2 = 0;
  }else{
    a2 = -(xx[2]-xx[1])/(yy[2]-yy[1]);
  }
  b2 = cy2 - a2 * cx2;

  if( a1_inf ){
    _x = cx1;
    _y = a2 * cx1 + b2;
  }else if( a2_inf ){
    _x = cx2;
    _y = a1 * cx2 + b1;
  }else if( a1 == a2 ){
    return False; /* if a1 = a2, r = ∞ */
  }else{
    _x = (b2 - b1)/(a1 - a2);
    _y = a1 * _x + b1;
  }
  dx = xx[0] - _x, dy = yy[0] - _y;
  sdx = dx, sdy = dy;

  dx = xx[2] - _x, dy = yy[2] - _y;
  edx = dx, edy = dy;

  for( int i = 0 ; i < 3 ; i++ )
    agl[i] = get_angle( _x - xx[i], yy[i] - _y );

  for( i = 1 ; i < 3 ; i++ )
    agl[ i ] -= agl[ 0 ];
  agl[0] = 0;

  while( agl[1] < 0 || agl[2] < 0 )
    agl[1] += PAI_2, agl[2] += PAI_2;

  while( agl[1] > PAI_2 )
    agl[1] -= PAI_2;
  while( agl[2] > PAI_2 )
    agl[2] -= PAI_2;

  if( agl[1] > agl[2] )
    rrot = True;
  else
    rrot = False;

  r = sqrt( pow(dx,2) + pow(dy,2) );
  return True;
}

//
// resize():
//
EditResult Arc::resize( XEvent* event )
{
  return EDIT_CANCEL;
}

//
// reversx():
//
void Arc::reversx()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < 3 ; i++ ){
    double x = xx[i] - bx;
    xx[i] = bx + bw - x;
  }
  compute_geom();
}

//
// reversy():
//
void Arc::reversy()
{
  double bx, by, bw, bh;
  bound( bx, by, bw, bh );
  for( int i = 0 ; i < 3 ; i++ ){
    double y = yy[i] - by;
    yy[i] = by + bh - y;
  }
  compute_geom();
}

//
// rotate():
//
void Arc::rotate( XEvent* event )
{
  int mx = event->xbutton.x, my = event->xbutton.y;
  board->quontize( mx, my );
  for( int i = 0 ; i < 3 ; i++ ){
    xx[i] -= (double)mx;
    yy[i] -= (double)my;
  }
  for( i = 0 ; i < 3 ; i++ ){
    double tmp_x = xx[i];
    xx[i] = yy[i];
    yy[i] = -tmp_x;
  }
  for( i = 0 ; i < 3 ; i++ ){
    xx[i] += (double)mx;
    yy[i] += (double)my;
  }
  compute_geom();
}

//
// update():
//
void Arc::update()
{
  if( line_mode != board->line_mode || arrow_size != board->arrow_size ||
     ball_size != board->ball_size || line_style != board->line_style ||
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
void Arc::xfig( FILE* fp, double x, double y )
{
  fprintf( fp, "%d 1 ", O_ARC ); /* 1=3点指定円弧 */
  fprintf( fp, "%d 1 -1 0 0 ", SOLID_LINE );/*線幅=1, color,depth,pen=nouse*/
  fprintf( fp, "%d 0.0 %d ", UNFILLED, !rrot ); /* style_val=未使用 */
  switch( line_mode ){
  case LM_MMASSOC:
  case LM_OMASSOC:
  case LM_SOLID:  fprintf( fp, "0 0 "); break;
  case LM_FARROW: fprintf( fp, "1 0 "); break;
  case LM_RARROW: fprintf( fp, "0 1 "); break;
  case LM_BARROW: fprintf( fp, "1 1 "); break;
  }
  fprintf( fp, "%d %d ", ROUND(XFS(_x+x)), ROUND(XFS(_y+y)) );
  fprintf( fp, "%d %d %d %d %d %d ",
	  ROUND(XFS(xx[0]+x)), ROUND(XFS(yy[0]+y)),
	  ROUND(XFS(xx[1]+x)), ROUND(XFS(yy[1]+y)),
	  ROUND(XFS(xx[2]+x)), ROUND(XFS(yy[2]+y)) );
  if( line_mode == LM_FARROW || line_mode == LM_BARROW ){
    fprintf( fp, "\n0 0 1 %d %d",
           /* arrow_type, arrow_style = nouse, arrow_thickness = 1 */
	    ROUND(XFS(2.0*arrow_size*sin(.35))),     /* arrow_width */
	    ROUND(XFS(arrow_size*cos(.35))) );       /* arrow_height */
  }
  if( line_mode == LM_RARROW || line_mode == LM_BARROW ){
    fprintf( fp, "\n0 0 1 %d %d",
	   /* arrow_type, arrow_style = nouse, arrow_thickness = 1 */
	    ROUND(XFS(2.0*arrow_size*sin(.35))),     /* arrow_width */
	    ROUND(XFS(arrow_size*cos(.35))) );       /* arrow_height */
  }
  fprintf( fp, "\n" );
}
