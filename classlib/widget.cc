//
// widget.cc: ウィジェットのコアクラス。
//
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <X11/Xlib.h>
#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>

#define DEFAULT_HANKAKU_FONT "7x14"
#define DEFAULT_ZENKAKU_FONT "k14"
/*
#define C_BACKGROUND "lightsteelblue3"
#define C_HIGHLIGHT  "lightsteelblue2"
#define C_SHADOW     "lightsteelblue4"
*/
#define C_BACKGROUND "snow3"
#define C_HIGHLIGHT  "snow2"
#define C_SHADOW     "snow4"


#define C_CHECK      "red"

static List<XFontPair> xfp_slot;

#define BW1 WIDGET_BORDER_WIDTH1
#define BW2 WIDGET_BORDER_WIDTH2
#define BW  WIDGET_BORDER_WIDTH
#define TBW 3

#define INPUT_MASKS WIDGET_DEFAULT_INPUT_MASKS

//
// Widget(): コンストラクタ
//
Widget::Widget( Display* _dpy, Window _parent, char* _name )
{
  dpy = _dpy, parent = _parent;
  create_pixels();
  name = strdup( _name );
  label = NULL;
  callback = NULL;
  client_data = NULL;
  status = WIDGET_ACTIVE_MASK;
  shortcut_key = 0;
  _x = _y = 0, _w = _h = 10;
  gc = XCreateGC( dpy, _parent, 0, 0 );
  window = XCreateSimpleWindow( dpy, _parent, _x, _y, _w, _h,
			       0, pixels[WIDGET_BACKGROUND_PIXEL],
			       pixels[WIDGET_BACKGROUND_PIXEL] );
  XSelectInput( dpy, window, INPUT_MASKS );
  default_hname = DEFAULT_HANKAKU_FONT;
  default_zname = DEFAULT_ZENKAKU_FONT;
  XSetWindowAttributes attr;
  attr.backing_store = WhenMapped;
  attr.save_under = True;
  XChangeWindowAttributes( dpy, window, CWBackingStore|CWSaveUnder, &attr );
}

//
// Widget(): コンストラクタ
//
Widget::Widget( Widget* _parent, char* _name )
{
  dpy = _parent->dpy;
  memcpy( pixels, _parent->pixels, sizeof(pixels) );
  name = strdup( _name );
  label = NULL;
  callback = NULL;
  client_data = NULL;
  status = WIDGET_ACTIVE_MASK;
  shortcut_key = 0;
  _x = 0, _y = 0, _w = 10, _h = 10;
  gc = _parent->gc;
  parent = _parent->window;
  window = XCreateSimpleWindow( dpy, parent, _x, _y, _w, _h,
			       0, pixels[WIDGET_BACKGROUND_PIXEL],
			       pixels[WIDGET_BACKGROUND_PIXEL] );
  XSelectInput( dpy, window, INPUT_MASKS );
  default_hname = DEFAULT_HANKAKU_FONT;
  default_zname = DEFAULT_ZENKAKU_FONT;
  XSetWindowAttributes attr;
  attr.backing_store = WhenMapped;
  attr.save_under = True;
  XChangeWindowAttributes( dpy, window, CWBackingStore|CWSaveUnder, &attr );
}

//
// Widget(): コピーコンストラクタ
//
Widget::Widget( Widget* widget )
{
  widget_type = widget->widget_type;
  dpy = widget->dpy;
  memcpy( pixels, widget->pixels, sizeof(pixels) );
  name = strdup( widget->name );
  if( widget->label != NULL ){
    label = strdup( widget->label );
  }else{
    label = NULL;
  }
  callback = widget->callback;
  client_data = widget->client_data;
  status = widget->status;
  shortcut_key = widget->shortcut_key;
  _x = widget->_x, _y = widget->_y, _w = widget->_w, _h = widget->_h;
  gc = widget->gc;
  parent = widget->window;
  window = XCreateSimpleWindow( dpy, parent, _x, _y, _w, _h,
			       0, pixels[WIDGET_BACKGROUND_PIXEL],
			       pixels[WIDGET_BACKGROUND_PIXEL] );
  XSelectInput( dpy, window, INPUT_MASKS );
  default_hname = DEFAULT_HANKAKU_FONT;
  default_zname = DEFAULT_ZENKAKU_FONT;
  XSetWindowAttributes attr;
  attr.backing_store = WhenMapped;
  attr.save_under = True;
  XChangeWindowAttributes( dpy, window, CWBackingStore|CWSaveUnder, &attr );
}

//
// ~Widget(): デストラクタ
//
Widget::~Widget()
{
  free( name );
  if( label != NULL )
    free( label );
}

//
// find_font_pair(): 名前を元にフォントペアを得る。
//
XFontPair* Widget::find_font_pair( char* hname, char* zname )
{
  for( int i = 0 ; i < xfp_slot.count() ; i++ ){
    XFontPair* xfp = xfp_slot.get(i);
    if( strcmp( hname, xfp->_hname ) == 0 ){
      if( zname != NULL ){
	if( strcmp( zname, xfp->_zname ) == 0 )
	  return xfp;
	continue;
      }
      return xfp;
    }
  }
  XFontPair* xfp;
  if( zname == NULL )
    xfp = new XFontPair( dpy, hname );
  else
    xfp = new XFontPair( dpy, hname, zname );
  xfp_slot.append( xfp );
  return xfp;
}

//
// set_label(): ラベルを設定する。
//
void Widget::set_label( char* l )
{
  if( label != NULL )
    free( label );
  if( (label = (char*)strdup( l )) == NULL )
    fatal();
}

//
// set_shortcut_key(): ショートカットキーを設定する。
//
void Widget::set_shortcut_key( char c )
{
  shortcut_key = c;
}

//
// ３ｄ表示の箱を描画する。
//
void Widget::draw_3d_box( Window win, int x, int y, int w, int h )
{
  unsigned long blackpixel = BlackPixel(dpy,DefaultScreen(dpy));
  XSetFunction( dpy, gc, GXcopy );
  XSetForeground( dpy, gc, blackpixel );
  XFillRectangle( dpy, win, gc, x, y, w, h );
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  XFillRectangle( dpy, win, gc, x+BW2, y+BW2, w-BW2*2, h-BW2*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  XFillRectangle( dpy, win, gc,
		 x+BW2+BW1, y+BW2+BW1, w-(BW2+BW1)*2, h-(BW2+BW1)*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  for( int i = 1 ; i <= BW1 ; i++ ){
    XDrawLine( dpy, win, gc, x+w-BW2-i, y+BW2+i-1, x+w-BW2-i, y+h-BW2-1 );
    XDrawLine( dpy, win, gc, x+BW2+i-1, y+h-BW2-i, x+w-BW2-1, y+h-BW2-i );
  }
}

//
// 押された状態の３ｄ箱を描画する。
//
void Widget::draw_pushed_3d_box( Window win, int x, int y, int w, int h )
{
  unsigned long blackpixel = BlackPixel(dpy,DefaultScreen(dpy));
  XSetFunction( dpy, gc, GXcopy );
  XSetForeground( dpy, gc, blackpixel );
  XFillRectangle( dpy, win, gc, x, y, w, h );
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  XFillRectangle( dpy, win, gc, x+BW2, y+BW2, w-BW2*2, h-BW2*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  XFillRectangle( dpy, win, gc,
		 x+BW2+BW1, y+BW2+BW1, w-(BW2+BW1)*2, h-(BW2+BW1)*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  for( int i = 1 ; i <= BW1 ; i++ ){
    XDrawLine( dpy, win, gc, x+w-BW2-i, y+BW2+i-1, x+w-BW2-i, y+h-BW2-1 );
    XDrawLine( dpy, win, gc, x+BW2+i-1, y+h-BW2-i, x+w-BW2-1, y+h-BW2-i );
  }
}

//
// ３ｄ表示の枠を描画する。
//
void Widget::draw_3d_frame( Window win, int x, int y, int w, int h )
{
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  XFillRectangle( dpy, win, gc, x+BW2, y+BW2, w-BW2*2, h-BW2*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  XFillRectangle( dpy, win, gc,
		 x+BW2+BW1, y+BW2+BW1, w-(BW2+BW1)*2, h-(BW2+BW1)*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  for( int i = 1 ; i <= BW1 ; i++ ){
    XDrawLine( dpy, win, gc, x+w-BW2-i, y+BW2+i-1, x+w-BW2-i, y+h-BW2-1 );
    XDrawLine( dpy, win, gc, x+BW2+i-1, y+h-BW2-i, x+w-BW2-1, y+h-BW2-i );
  }
}

//
// 凹型の３ｄ表示枠を描画する。
//
void Widget::draw_pushed_3d_frame( Window win, int x, int y, int w, int h )
{
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  XFillRectangle( dpy, win, gc, x+BW2, y+BW2, w-BW2*2, h-BW2*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  XFillRectangle( dpy, win, gc,
		 x+BW2+BW1, y+BW2+BW1, w-(BW2+BW1)*2, h-(BW2+BW1)*2 );
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  for( int i = 1 ; i <= BW1 ; i++ ){
    XDrawLine( dpy, win, gc, x+w-BW2-i, y+BW2+i-1, x+w-BW2-i, y+h-BW2-1 );
    XDrawLine( dpy, win, gc, x+BW2+i-1, y+h-BW2-i, x+w-BW2-1, y+h-BW2-i );
  }
}

void Widget::draw_Dtriangle( Window window, int x, int y, int w, int h )
{
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  XPoint points[3];
  points[0].x = x, points[0].y = y;
  points[1].x = x+w, points[1].y = y;
  points[2].x = x+w/2, points[2].y = y+h;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  points[0].x = x+TBW, points[0].y = y+TBW;
  points[1].x = x+w-TBW, points[1].y = y+TBW;
  points[2].x = x+w/2, points[2].y = y+h-TBW;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  for( int i = 0 ; i < TBW; i++ )
    XDrawLine( dpy, window, gc, x+w-i, y+i, x+w/2, y+h-i );
}

void Widget::draw_Utriangle( Window window, int x, int y, int w, int h )
{
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  XPoint points[3];
  points[0].x = x+w/2, points[0].y = y;
  points[1].x = x+w, points[1].y = y+h;
  points[2].x = x, points[2].y = y+h;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  points[0].x = x+w/2, points[0].y = y+TBW;
  points[1].x = x+w-TBW, points[1].y = y+h-TBW;
  points[2].x = x+TBW, points[2].y = y+h-TBW;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  for( int i = 0 ; i < TBW; i++ )
    XDrawLine( dpy, window, gc, x+w/2, y+i, x+i, y+h-i );
}

void Widget::draw_Rtriangle( Window window, int x, int y, int w, int h )
{
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  XPoint points[3];
  points[0].x = x, points[0].y = y;
  points[1].x = x+w, points[1].y = y+h/2;
  points[2].x = x, points[2].y = y+h;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  points[0].x = x+TBW, points[0].y = y+TBW;
  points[1].x = x+w-TBW, points[1].y = y+h/2;
  points[2].x = x+TBW, points[2].y = y+h-TBW;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  for( int i = 0 ; i < TBW; i++ )
    XDrawLine( dpy, window, gc, x+w-i, y+h/2, x+i, y+h-i );
}

void Widget::draw_Ltriangle( Window window, int x, int y, int w, int h )
{
  XSetForeground( dpy, gc, pixels[WIDGET_SHADOW_PIXEL] );
  XPoint points[3];
  points[0].x = x, points[0].y = y+h/2;
  points[1].x = x+w, points[1].y = y;
  points[2].x = x+w, points[2].y = y+h;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  points[0].x = x+TBW, points[0].y = y+h/2;
  points[1].x = x+w-TBW, points[1].y = y+TBW;
  points[2].x = x+w-TBW, points[2].y = y+h-TBW;
  XFillPolygon( dpy, window, gc, points, 3, Convex, CoordModeOrigin );
  XSetForeground( dpy, gc, pixels[WIDGET_HIGHLIGHT_PIXEL] );
  for( int i = 0 ; i < TBW; i++ )
    XDrawLine( dpy, window, gc, x+i, y+h/2, x+w-i, y+h-i );
}

//
// create_pixels():描画に必要なカラーセルをアロケートする。 
//
void Widget::create_pixels()
{
  XColor color, exact;
  Colormap cmap = DefaultColormap( dpy, DefaultScreen(dpy) );
  if( !XAllocNamedColor( dpy, cmap, C_BACKGROUND, &exact, &color ) )
    abort();
  pixels[WIDGET_BACKGROUND_PIXEL] = color.pixel;
  pixels[WIDGET_FOREGROUND_PIXEL] = BlackPixel( dpy, DefaultScreen(dpy) );
  if( !XAllocNamedColor( dpy, cmap, C_HIGHLIGHT, &exact, &color ) )
    abort();
  pixels[WIDGET_HIGHLIGHT_PIXEL] = color.pixel;
  if( !XAllocNamedColor( dpy, cmap, C_SHADOW, &exact, &color ) )
    abort();
  pixels[WIDGET_SHADOW_PIXEL] = color.pixel;
  if( !XAllocNamedColor( dpy, cmap, C_CHECK, &exact, &color ) )
    abort();
  pixels[WIDGET_CHECK_PIXEL] = color.pixel;
}

//
// move():
//
void Widget::move( int x, int y )
{
  XMoveWindow( dpy, window, x, y );
  _x = x, _y = y;
}

//
// resize():
//
void Widget::resize( int w, int h )
{
  XResizeWindow( dpy, window, w, h );
  _w = w, _h = h;
}

//
// resizemove():
//
void Widget::move_resize( int x, int y, int w, int h )
{
  XMoveResizeWindow( dpy, window, x, y, w, h );
  _x = x, _y = y, _w = w, _h = h;
}

//
// set_value():
//
void Widget::set_value( int v )
{
  value = v;
//  XClearArea( dpy, window, 0, 0, 0, 0, True );
}
