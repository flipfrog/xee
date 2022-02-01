//
// str.cc:
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <local_types.h>
#include <list.h>

#include <eedefs.h>
#include <shape.h>
#include <shapeset.h>

#include <xfontpair.h>
#include <skkserv.h>
#include <skkfep.h>

#include <frame.h>
#include <xcontext.h>
#include <board.h>
#include <point.h>
#include <str.h>
#include <misc.h>
#include <geometry.h>

#include <xfig.h>

//
// コンストラクタ
//
Str::Str( Board* board ): Shape( board )
{
  shape_type = ST_STRING;
  round_style = board->round_style;
  border_width = board->border_width;
  font_size = board->font_size;
  line_style = board->line_style;
  line_width = board->line_width;
  _x = _y = _w = _h = 0;
  buf[0] = 0;
}

//
// コピーコンストラクタ
//
Str::Str( Str* str ): Shape( str )
{
  shape_type = ST_STRING;
  round_style = str->round_style;
  border_width = str->border_width;
  font_size = str->font_size;
  line_style = str->line_style;
  line_width = str->line_width;
  _x = str->_x, _y = str->_y;
  _w = str->_w, _h = str->_h;
  strncpy( buf, str->buf, sizeof(buf) );
  buf[ sizeof(buf)-1 ] = 0;
}

//
// draw():
//
void Str::draw( Window window, int x, int y )
{
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;
  double xx = _x+x, yy = _y+y;
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
  switch( round_style ){
  case RS_NONE:
    board->xfp->draw_string( window, gc, ROUND(xx), ROUND(yy), buf );
    break;
  case RS_RECT:
    board->xfp->draw_string( window, gc,
			    ROUND(xx+border_width),
			    ROUND(yy+border_width), buf );
    XDrawRectangle(dpy,window,gc,ROUND(xx),ROUND(yy),ROUND(_w),ROUND(_h));
    break;
  case RS_CAPSULE:
    double sx = _x + _h/2 + border_width + x;
    double sy = _y + border_width + y;
    board->xfp->draw_string( window, gc, ROUND(sx), ROUND(sy), buf );
    sx = _x + _h/2 + x;
    double ex = _x + _w - _h/2 + x;
    XDrawLine(dpy,window,gc,ROUND(sx),ROUND(_y+y),ROUND(ex),ROUND(_y+y));
    XDrawLine(dpy,window,gc,ROUND(sx),ROUND(_y+_h+y),ROUND(ex),ROUND(_y+_h+y));
    XDrawArc( dpy, window, gc, ROUND(xx), ROUND(yy),
	     ROUND(_h), ROUND(_h), 90*64,180*64 );
    XDrawArc( dpy, window, gc, ROUND(_x+_w-_h+x), ROUND(_y+y),
	     ROUND(_h), ROUND(_h), 270*64, 180*64 );
    break;
  }
  XSetLineAttributes( dpy, gc, 1, LineSolid, CapButt, JoinMiter );
}

//
// hit():
//
Boolean Str::hit( int x, int y, int hw )
{
  if( x>=ROUND(_x) && x<ROUND(_x+_w)-1 && y>=ROUND(_y) && y<ROUND(_y+_h)-1 )
    return True;
  return False;
}

//
// layout():
//
EditResult Str::layout( XEvent* event )
{
  Display* dpy = event->xany.display;
  XSetFunction( dpy, board->gc, GXset );
  XSetPlaneMask( dpy, board->gc, board->layout_mask );
  switch( proc_phase ){
  case 0 :
    int mx = event->xbutton.x, my = event->xbutton.y;
    board->quontize( mx, my );
    _x = (double)mx, _y = (double)my;
    board->skkfep->set_buffer( buf, sizeof(buf) );
    board->skkfep->set_draw_function( GXset );
    board->skkfep->set_pixels( board->layout_mask, 0 );
    board->skkfep->
      draw( dpy, board->gc, event->xbutton.window, ROUND(_x), ROUND(_y) );
    proc_phase = 1;
    break;
  case 1:
    switch( event->type ){
    case KeyPress:
      board->skkfep->set_draw_function( GXset );
      board->skkfep->set_pixels( board->layout_mask, 0 );
      if( board->skkfep->event_proc(board->gc, event, ROUND(_x), ROUND(_y)) ){
	board->skkfep->set_draw_function( GXclear );
	board->skkfep->set_pixels( board->layout_mask, 0 );
	board->skkfep->
	  draw( dpy, board->gc, event->xkey.window, ROUND(_x), ROUND(_y) );
	if( strlen(buf) == 0 ){
	  XSetFunction( dpy, board->gc, GXcopy );
	  XSetPlaneMask( dpy, board->gc, ~0 );
	  return EDIT_CANCEL;
	}
	_w = (double)(board->xfp->text_width(buf));
	_h = (double)(board->xfp->height());
	if( round_style == RS_CAPSULE || round_style == RS_RECT ){
	  _w += (double)border_width*2;
	  _h += (double)border_width*2;
	}
	if( round_style == RS_CAPSULE )
	  _w += _h;
	proc_phase = 0;
	XSetFunction( dpy, board->gc, GXset );
	XSetPlaneMask( dpy, board->gc, board->shape_mask );
	draw( event->xkey.window, 0, 0 );
	XSetFunction( dpy, board->gc, GXcopy );
	XSetPlaneMask( dpy, board->gc, ~0 );
	return EDIT_COMPLETE;
      }
      break;
    case Expose:
      XSetFunction( dpy, board->gc, GXset );
      XSetPlaneMask( dpy, board->gc, board->layout_mask );
      board->skkfep->
	draw( dpy, board->gc, event->xkey.window, ROUND(_x), ROUND(_y) );
      XSetFunction( dpy, board->gc, GXcopy );
      XSetPlaneMask( dpy, board->gc, ~0 );
      break;
    }
    break;
  }
  return EDIT_CONTINUE;
}

//
// save():
//
void Str::save( FILE* fp )
{
  fprintf( fp, "string{\n" );
  fprintf( fp, "  geom = (%g,%g,%g,%g);\n", _x, _y, _w, _h );
  fprintf( fp, "  string = \"%s\";\n", buf );
  fprintf( fp, "  border_width = %g;\n", border_width );
  fprintf( fp, "  round_style = %s;\n", rs_string(round_style) );
  fprintf( fp, "  font_size = %s;\n", font_size_string( font_size ) );
  if( !board->msdos_compatibility ){
    fprintf( fp, "  line_style = %s;\n", ls_string( line_style ) );
    fprintf( fp, "  line_width = %d;\n", line_width );
  }
  fprintf( fp, "}\n" );
}

//
// tex():
//
void Str::tex( FILE* fp, double bh, double x, double y )
{
  switch( round_style ){
  case RS_NONE:
    fprintf( fp, "\\put(%g,%g){\\makebox(0,0)[lb]{\\%s\\tt %s}}\n",
	    _x+x, bh-(_y+y+_h-1), font_size_string(font_size), buf );
    break;
  case RS_RECT:
    fprintf( fp, "\\put(%g,%g){\\makebox(0,0)[lb]{\\%s\\tt %s}}\n",
	    _x+border_width+x, bh-(_y+y-border_width+_h-1),
	    font_size_string(font_size), buf );
    fprintf( fp, "\\path(%g,%g)(%g,%g)(%g,%g)(%g,%g)(%g,%g)\n",
	    _x+x,      bh - (_y+y),
	    _x+x+_w-1, bh - (_y+y),
	    _x+x+_w-1, bh - (_y+y+_h),
	    _x+x,      bh - (_y+y+_h),
	    _x+x,      bh - (_y+y) );
    break;
  case RS_CAPSULE:
    fprintf( fp, "\\put(%g,%g){\\makebox(0,0)[lb]{\\%s\\tt %s}}\n",
	    _x+x+_h/2+border_width, bh-(_y+y-border_width+_h-1),
	    font_size_string(font_size), buf );
    fprintf( fp, "\\path(%g,%g)(%g,%g)\n",
	    _x+x+_h/2,    bh-(_y+y), _x+x-_h/2+_w, bh-(_y+y) );
    fprintf( fp, "\\path(%g,%g)(%g,%g)\n",
	    _x+x+_h/2,    bh-(_y+y+_h), _x+x-_h/2+_w, bh-(_y+y+_h) );
    fprintf( fp, "\\put(%g,%g){\\arc{%g}{1.571}{4.712}}\n",
	    _x+x+_h/2,   bh-(_y+y+_h/2), _h );
    fprintf( fp, "\\put(%g,%g){\\arc{%g}{4.712}{7.854}}\n",
	    _x+x-_h/2+_w, bh-(_y+y+_h/2), _h );
    break;
  }
}

//
// bound():
//
void Str::bound( double& x, double& y, double& w, double& h )
{
  x = _x, y = _y, w = _w, h = _h;
}

//
// translate():
//
void Str::translate( double x, double y )
{
  _x += x, _y += y;
}

//
// duplicate():
//
Shape* Str::duplicate()
{
  return new Str( this );
}

//
// contain():
//
Boolean Str::contain( int x, int y, int w, int h )
{
  if( ROUND(_x) < x || ROUND(_x+_w) >= x+w ) return False;
  if( ROUND(_y) < y || ROUND(_y+_h) >= y+h ) return False;
  return True;
}

//
// scale():
//
void Str::scale( double rx, double ry )
{
  _x *= rx, _y *= ry;
  _w *= rx, _h *= ry;
}

//
// update():
//
void Str::update()
{
  if( round_style != board->round_style ||
     border_width != board->border_width || font_size != board->font_size ||
     line_style != board->line_style || line_width != board->line_width ){

    round_style = board->round_style;
    border_width = board->border_width;
    font_size = board->font_size;
    line_style = board->line_style;
    line_width = board->line_width;

    _w = board->xfp->text_width( buf );
    _h = board->xfp->height();

    switch( round_style ){
    case RS_RECT:
      _w += (double)(border_width) * 2;
      _h += (double)(border_width) * 2;
      if( ROUND(_w) % 4 != 0 )
	_w -= (double)(ROUND(_w)%4);
      if( ROUND(_h) % 4 != 0 )
	_h -= (double)(ROUND(_h)%4);
      break;
    case RS_CAPSULE:
      _w += (double)(border_width) * 2;
      _h += (double)(border_width) * 2;
      _w += _h;
      break;
    }
  }
}

//
// xfig():
//
void Str::xfig( FILE* fp, double x, double y )
{
  int font_type = 0;

  for( int i = 0 ; i < strlen(buf) ; i++ ){
    if( (unsigned char)(buf[i]) & 0x80 ){
      font_type = 35; /* RyouminLight */
      break;
    }
  }

  switch( round_style ){
  case RS_NONE:
    fprintf( fp, "%d 0 ", O_TEXT ); /* left justified */
    fprintf( fp, "%d %d ", font_type, get_font_point(font_size) );
    fprintf( fp, "0 0 0 " ); /* pen,color,depth=nouse */
    fprintf( fp, "0.0 7 ", PSFONT_TEXT );  /* angle=0 */
    fprintf( fp, "%d %d ", ROUND(XFS(_h)), ROUND(XFS(_w)) );
    fprintf( fp, "%d %d ", ROUND(XFS(_x+x)), ROUND(XFS(_y+_h+y)) );
    fprintf( fp, "%s\1\n", buf );
    break;
  case RS_RECT:
    fprintf( fp, "%d 0 ", O_TEXT ); /* left justified */
    fprintf( fp, "%d %d ", font_type, get_font_point(font_size) );
    fprintf( fp, "0 0 0 " ); /* pen,color,depth=nouse */
    fprintf( fp, "0.0 7 " );  /* angle=0 */
    fprintf( fp, "%d %d ", ROUND(XFS(_h)), ROUND(XFS(_w-border_width*2.0)) );
    fprintf( fp, "%d %d ",
	    ROUND(XFS(_x+border_width+x)), ROUND(XFS(_y+_h-border_width+y)) );
    fprintf( fp, "%s\1\n", buf );
    fprintf( fp, "%d %d %d ", O_POLYLINE, T_BOX, SOLID_LINE );
    fprintf( fp, "1 -1 0 0 %d 0.0 -1 0 0\n", UNFILLED );
    fprintf( fp, "%d %d %d %d %d %d %d %d %d %d 9999 9999\n",
	    ROUND(XFS(_x+x)), ROUND(XFS(_y+y)),
	    ROUND(XFS(_x+_w+x)), ROUND(XFS(_y+y)),
	    ROUND(XFS(_x+_w+x)), ROUND(XFS(_y+_h+y)),
	    ROUND(XFS(_x+x)), ROUND(XFS(_y+_h+y)),
	    ROUND(XFS(_x+x)), ROUND(XFS(_y+y)) );
    break;
  case RS_CAPSULE:
    double sx = _x + _h/2 + border_width + x;
    double sy = _y + _h - border_width + y;
    fprintf( fp, "%d 0 ", O_TEXT ); /* left justified */
    fprintf( fp, "%d %d ", font_type, get_font_point(font_size) );
    fprintf( fp, "0 0 0 " ); /* pen,color,depth=nouse */
    fprintf( fp, "0.0 7 ");  /* angle=0 */
    fprintf( fp, "%d %d ",
	    ROUND(XFS(_h)), ROUND(XFS(_w-_h-border_width*2.0)) );
    fprintf( fp, "%d %d %s\1\n",
	    ROUND(XFS(sx+x)), ROUND(XFS(sy+y)), buf );

    sx = _x + _h/2 + x;
    double ex = _x + _w - _h/2 + x;
    fprintf( fp, "%d %d %d ", O_POLYLINE, T_POLYLINE, SOLID_LINE );
    fprintf( fp, "1 -1 0 0 %d 0.0 -1 0 0\n", UNFILLED );
    fprintf( fp, "%d %d %d %d 9999 9999\n",
	    ROUND(XFS(sx+x)), ROUND(XFS(_y+y)),
	    ROUND(XFS(ex+x)), ROUND(XFS(_y+y)) );
    fprintf( fp, "%d %d %d ", O_POLYLINE, T_POLYLINE, SOLID_LINE );
    fprintf( fp, "1 -1 0 0 %d 0.0 -1 0 0\n", UNFILLED );
    fprintf( fp, "%d %d %d %d 9999 9999\n",
	    ROUND(XFS(sx+x)), ROUND(XFS(_y+_h+y)),
	    ROUND(XFS(ex+x)), ROUND(XFS(_y+_h+y)) );

    fprintf( fp, "%d %d %d 1 -1 0 0 %d ",
	    O_ARC, T_ELLIPS_BY_RAD, SOLID_LINE, UNFILLED );
    fprintf( fp, "0.0 0 0 0 %d %d ",
	    ROUND(XFS(_x+_h/2.0+x)), ROUND(XFS(_y+_h/2.0+y)) );
    fprintf( fp, "%d %d %d %d %d %d\n",
	    ROUND(XFS(_x+_h/2.0+x)), ROUND(XFS(_y+_h+y)),
	    ROUND(XFS(_x+x)), ROUND(XFS(_y+_h/2.0+y)),
	    ROUND(XFS(_x+_h/2.0+x)), ROUND(XFS(_y+y)) );

    fprintf( fp, "%d %d %d 1 -1 0 0 %d ",
	    O_ARC, T_ELLIPS_BY_RAD, SOLID_LINE, UNFILLED );
    fprintf( fp, "0.0 1 0 0 %d %d ",
	    ROUND(XFS(_x+_w-_h/2.0+x)), ROUND(XFS(_y+_h/2.0+y)) );
    fprintf( fp, "%g %g %g %g %g %g\n",
	    ROUND(XFS(_x+_w-_h/2.0+x)), ROUND(XFS(_y+_h+y)),
	    ROUND(XFS(_x+_w+x)), ROUND(XFS(_y+_h/2.0+y)),
	    ROUND(XFS(_x+_w-_h/2.0+x)), ROUND(XFS(_y+y)) );
    break;
  }
}

//
// rotate():
//
void Str::rotate( XEvent* event )
{
  int mx = event->xbutton.x, my = event->xbutton.y;
  board->quontize( mx, my );
  double x_tmp;
  double x = _x, y = _y;
  x -= (double)mx, y -= (double)my;
  x_tmp = x;
  x = y, y = -x_tmp;
  x += (double)mx, y += (double)my;
  _x = x, _y = y;
}
