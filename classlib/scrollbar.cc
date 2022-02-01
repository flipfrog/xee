//
// scrollbar.cc
//
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <scrollbar.h>

#define BW WIDGET_BORDER_WIDTH
#define ROUND(x) ((int)(x+0.5))

//
// ScrollbarWidget(): コンストラクタ
//
ScrollbarWidget::ScrollbarWidget( Widget* p, char* n ):Widget( p, n )
{
  widget_type = WT_SCROLLBAR;
  horizontal = True;
  object_len = 100;
  view_len = 10;
  position = 0;
  move_step = 10;
  _w = _h = 25;
  XSelectInput( dpy, window, WIDGET_DEFAULT_INPUT_MASKS|ButtonMotionMask );

  XColor color, exact;
  Colormap cmap = DefaultColormap( dpy, DefaultScreen(dpy) );
  if( !XAllocNamedColor( dpy, cmap, "gray30", &exact, &color ) )
    abort();
  gray_pixel = color.pixel;
}

//
// draw():
//
void ScrollbarWidget::draw()
{
  draw_pushed_3d_frame( 0, 0, _w, _h );
  XSetForeground( dpy, gc, gray_pixel );
  XFillRectangle( dpy, window, gc, BW, BW, _w-BW*2, _h-BW*2 );

  if( horizontal ){
    draw_Ltriangle( window, BW*2, BW*2, _h-BW*4, _h-BW*4 );
    draw_Rtriangle( window, _w-_h+BW*2-1, BW*2, _h-BW*4, _h-BW*4 );
    if( status & WIDGET_ACTIVE_MASK ){
      double sld_width = (double)(_w-_h*2)*(double)view_len/(double)object_len;
      double draw_pos =
	((double)(_w-_h*2)-sld_width)*(double)position/
	  (double)(object_len-view_len)+_h;
      draw_3d_frame( ROUND(draw_pos), BW*2, ROUND(sld_width), _h-BW*4 );
    }
  }else{
    draw_Utriangle( window, BW*2, BW*2, _w-BW*4, _w-BW*4 );
    draw_Dtriangle( window, BW*2, _h-_w+BW*2-1, _w-BW*4, _w-BW*4 );
    if( status & WIDGET_ACTIVE_MASK ){
      double sld_width = (double)(_h-_w*2)*(double)view_len/(double)object_len;
      double draw_pos =
	((double)(_h-_w*2)-sld_width)*(double)position/
	  (double)(object_len-view_len)+_w;
      draw_3d_frame( BW*2, ROUND(draw_pos), _w-BW*4, ROUND(sld_width) );
    }
  }
}

//
// event_proc():
//
void ScrollbarWidget::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case Expose:
      draw();
      break;
    case ButtonPress:
      if( !(status & WIDGET_ACTIVE_MASK) )
	return;
      int mx = event->xbutton.x, my = event->xbutton.y;
      int last_x, last_y, last_position = position;

      // 左(マイナス側)矢印が押されたか調べる。
      if( ( horizontal && mx < _h )||( !horizontal && my < _w ) ){
	position -= move_step;
	if( position < 0 )
	  position = 0;
	draw();
      }

      // 右(プラス側)矢印が押されたか調べる。
      if( ( horizontal && mx >= _w-_h )||( !horizontal && my >= _h-_w ) ){
	position += move_step;
	if( position > object_len-view_len )
	  position = object_len-view_len;
	draw();
      }

      // エレベータが押されたか調べる。

      if( horizontal ){ // 水平方向

	double sld_width = (double)(_w-_h*2)*(double)view_len/
	  (double)object_len;
	double draw_pos = ((double)(_w-_h*2)-sld_width)*(double)position/
	  (double)(object_len-view_len)+_h;
	if( mx >= ROUND(draw_pos) && mx < ROUND(draw_pos+sld_width) ){
	  last_x = ROUND(draw_pos);
	  while(1){ // ボタンを離すまでループする…。
	    XEvent e;
	    XNextEvent( dpy, &e );
	    if( e.type == ButtonRelease )
	      break;
	    if( e.xany.window != window || e.type != MotionNotify )
	      continue;
	    int x = e.xmotion.x - mx + ROUND(draw_pos);
	    if( x >= _w-_h-ROUND(sld_width) )
	      x = _w-_h-ROUND(sld_width)-1;
	    if( x < _h )
	      x = _h;
	    if( x != last_x ){
	      XSetForeground( dpy, gc, gray_pixel );
	      XFillRectangle( dpy, window, gc,
			     last_x, BW*2, ROUND(sld_width), _h-BW*4 );
	      draw_3d_frame( x, BW*2, ROUND(sld_width), _h-BW*4 );
	      last_x = x;
	    }
	  }
	  position = ROUND((double)(last_x-_h)/((double)(_w-_h*2)-sld_width)*
			   (double)(object_len-view_len) );
	  if( position < 0 )
	    position = 0;
	  if( position > object_len-view_len )
	    position = object_len-view_len;
	  draw();
	}else{
	  // 右(マイナス側)のチェック
	  if( mx >= _h && mx < ROUND(draw_pos) ){
	    position -= view_len;
	    if( position < 0 )
	      position = 0;
	    draw();
	  }
	  // 左(プラス側)のチェック
	  if( mx >= ROUND(draw_pos+sld_width) && mx < _w-_h ){
	    position += view_len;
	    if( position > object_len-view_len )
	      position = object_len-view_len;
	    draw();
	  }
	}
      }else{ // 垂直方向

	double sld_width = (double)(_h-_w*2)*(double)view_len/
	  (double)object_len;
	double draw_pos = ((double)(_h-_w*2)-sld_width)*(double)position/
	  (double)(object_len-view_len)+_w;
	if( my >= ROUND(draw_pos) && my < ROUND(draw_pos+sld_width) ){
	  last_y = ROUND(draw_pos);
	  while(1){
	    XEvent e;
	    XNextEvent( dpy, &e );
	    if( e.type == ButtonRelease )
	      break;
	    if( e.xany.window != window || e.type != MotionNotify )
	      continue;
	    int y = e.xmotion.y - my + ROUND(draw_pos);
	    if( y >= _h-_w-ROUND(sld_width) )
	      y = _h-_w-ROUND(sld_width)-1;
	    if( y < _w )
	      y = _w;
	    if( y != last_y ){
	      XSetForeground( dpy, gc, gray_pixel );
	      XFillRectangle( dpy, window, gc,
			     BW*2, last_y, _w-BW*4, ROUND(sld_width) );
	      draw_3d_frame( BW*2, y, _w-BW*4, ROUND(sld_width) );
	      last_y = y;
	    }
	  }
	  position = ROUND((double)(last_y-_w)/((double)(_h-_w*2)-sld_width)*
			   (double)(object_len-view_len) );
	  if( position < 0 )
	    position = 0;
	  if( position > object_len-view_len )
	    position = object_len-view_len;
	  draw();
	}else{
	  // 右(マイナス側)のチェック
	  if( my >= _w && my < ROUND(draw_pos) ){
	    position -= view_len;
	    if( position < 0 )
	      position = 0;
	    draw();
	  }
	  // 左(プラス側)のチェック
	  if( my >= ROUND(draw_pos+sld_width) && my < ROUND(_h-_w) ){
	    position += view_len;
	    if( position > object_len-view_len )
	      position = object_len-view_len;
	    draw();
	  }
	}
      }
      if( position != last_position && callback != NULL )
	callback( this );
    }
  }
}
