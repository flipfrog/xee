//
// tile.c: タイルウィジェットクラス
//
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <tile.h>

#define BW WIDGET_BORDER_WIDTH

//
// TileWidget(): コンストラクタ
//
TileWidget::TileWidget( Widget* p, char* _name ) :Widget( p, _name )
{
  value = 0;
  column = 100;
}

//
// デストラクタ
//
TileWidget::~TileWidget()
{
  for( int i = 0 ; i < pixmap_slot.count() ; i++ )
    XFreePixmap( dpy, pixmap_slot.get(i)->pixmap );
}

//
// draw(): タイルウィジェットを描画する。
//
void TileWidget::draw()
{
  XSetPlaneMask( dpy, gc, ~0 );
  XSetFunction( dpy, gc, GXcopy );
  int xxx = 0, yyy = 0;
  for( int i = 0 ; i < pixmap_slot.count() ; i += column ){
    int h_skip = 0;
    for( int j = i ; j < i+column && j < pixmap_slot.count() ; j++ ){
      PixmapTab* pt = pixmap_slot.get(j);
      int bw = BW*2;
      xxx = (pt->w+bw) * ( j % column );
      yyy = (pt->h+bw) * ( j / column );
      if( j == value ){
	draw_pushed_3d_box( xxx, yyy, pt->w+bw, pt->h+bw );
      }else{
	draw_3d_box( xxx, yyy, pt->w+bw, pt->h+bw );
      }
      XSetBackground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
      XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );
      if( pt->xpm_f ){
	XCopyArea( dpy, pt->pixmap, window, gc, 0, 0,
		  pt->w, pt->h, xxx+bw/2, yyy+bw/2 );
      }else{
	XCopyPlane( dpy, pt->pixmap, window, gc, 0, 0,
		   pt->w, pt->h, xxx+bw/2, yyy+bw/2, 1 );
      }
    }
  }
  widget_type = WT_TILE;
}

//
// event_proc():
//
void TileWidget::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case Expose:
      draw();
      break;
    case ButtonPress:
      if( status & WIDGET_ACTIVE_MASK == 0 )
	return;
      PixmapTab* pt = pixmap_slot.get(0);
      int xxx, yyy;
      int ex = event->xbutton.x, ey = event->xbutton.y;
      int bw = BW*2;
      int sel = ey/(pt->h+bw)*column + ex/(pt->w+bw);
      if( sel != value && sel < pixmap_slot.count() ){
	PixmapTab* pt = pixmap_slot.get(value);
	xxx = (pt->w+bw) * ( value % column );
	yyy = (pt->h+bw) * ( value / column );
	draw_3d_box( xxx, yyy, pt->w+bw, pt->h+bw );
	XSetBackground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
	XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );
	if( pt->xpm_f ){
	  XCopyArea( dpy, pt->pixmap, window, gc, 0, 0,
		    pt->w, pt->h, xxx+bw/2, yyy+bw/2 );
	}else{
	  XCopyPlane( dpy, pt->pixmap, window, gc, 0, 0,
		     pt->w, pt->h, xxx+bw/2, yyy+bw/2, 1 );
	}
	value = sel;
	pt = pixmap_slot.get(value);
	xxx = (pt->w+bw) * ( value % column );
	yyy = (pt->h+bw) * ( value / column );
	draw_pushed_3d_box( xxx, yyy, pt->w+bw, pt->h+bw );
	XSetBackground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
	XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );
	if( pt->xpm_f ){
	  XCopyArea( dpy, pt->pixmap, window, gc, 0, 0,
		    pt->w, pt->h, xxx+bw/2, yyy+bw/2 );
	}else{
	  XCopyPlane( dpy, pt->pixmap, window, gc, 0, 0,
		     pt->w, pt->h, xxx+bw/2, yyy+bw/2, 1 );
	}
	if( callback != NULL )
	  callback( this );
      }
      break;
    }
  }
}

//
// set_column(): カラム数を設定する。
//
void TileWidget::set_column( int c )
{
  column = c;
}

//
// append_bitmap(): ビットマップを追加する。
//
void TileWidget::append_bitmap( char* _data, int data_w, int data_h )
{
  PixmapTab* pt = new PixmapTab;
  pt->w = data_w, pt->h = data_h, pt->xpm_f = False;
  pt->pixmap = XCreateBitmapFromData( dpy, window, _data, pt->w, pt->h );
  pixmap_slot.append( pt );
  if( pixmap_slot.count() > column )
    _w = (data_w+BW*2) * column;
  else
    _w = (data_w+BW*2) * pixmap_slot.count();
  _h = (data_h+BW*2) * ((pixmap_slot.count()+column-1)/column);
  resize( _w, _h );
}

//
// append_pixmap(): ピックスマップを追加する。
//
void TileWidget::append_pixmap( char** data )
{
  PixmapTab* pt = new PixmapTab;
  XpmColorSymbol colorsymbol =
    { "mask", NULL, pixels[WIDGET_BACKGROUND_PIXEL] };
  XpmAttributes attr;
  attr.colormap = DefaultColormap( dpy, DefaultScreen(dpy) );
  attr.colorsymbols = &colorsymbol;
  attr.numsymbols = 1;
  attr.valuemask = XpmColormap|XpmColorSymbols|XpmSize;
  XpmCreatePixmapFromData( dpy, window, data, &(pt->pixmap), NULL, &attr );
  if( pt->pixmap == 0 ){
    fprintf( stderr, "IconWidget::set_pixmap(): cannot allocate pixmap.\n");
    exit(1);
  }
  pt->w = attr.width, pt->h = attr.height, pt->xpm_f = True;
  pixmap_slot.append( pt );
  if( pixmap_slot.count() > column )
    _w = (pt->w+BW*2) * column;
  else
    _w = (pt->w+BW*2) * pixmap_slot.count();
  _h = (pt->h+BW*2) * ((pixmap_slot.count()+column-1)/column);
  resize( _w, _h );
}
