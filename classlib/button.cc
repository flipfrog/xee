//
// button.cc: ボタンウィジェット
//
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <button.h>

#define BW WIDGET_BORDER_WIDTH
#define TEXT_BORDER 6

//
// ButtonWidget(): コンストラクタ
//
ButtonWidget::ButtonWidget( Widget* p, char* n ) :Widget( p, n )
{
  XFontPair* xfp = get_default_xfp();
  _w = xfp->text_width( name ) + (TEXT_BORDER+BW)*2;
  _h = xfp->height() + (TEXT_BORDER+BW)*2;
  XResizeWindow( dpy, window, _w, _h );
  widget_type = WT_BUTTON;
  cursor = XCreateFontCursor( dpy, XC_hand2 );
  XDefineCursor( dpy, window, cursor );
}

//
// ButtonWidget(): コピーコンストラクタ
//
ButtonWidget::ButtonWidget( ButtonWidget* button ): Widget( button )
{
  cursor = XCreateFontCursor( dpy, XC_hand2 );
  XDefineCursor( dpy, window, cursor );
}

//
// デストラクタ
//
ButtonWidget::~ButtonWidget()
{
  XFreeCursor( dpy, cursor );
}

//
// interactive(): イベント情報を参照してインタラクティブ処理を行う。
//
void ButtonWidget::event_proc( XEvent* event )
{
  switch( event->type ){
  case Expose:
    if( event->xexpose.window == window )
      draw();
    break;
  case KeyPress:
    if( status & WIDGET_ACTIVE_MASK == 0 || event->xkey.window != window )
      return;
    if( event->xkey.state == Mod1Mask ){ /* M-<short cut> */
      char buf[2+1];
      int cnt;
      if( (cnt = XLookupString( (XKeyEvent*)event, buf, 2, 0, 0 )) > 0 ){
	buf[ cnt ] = 0;
	if( toupper(shortcut_key) == toupper(buf[0]) ){
	  revers();
	  while( 1 ){ // キーを離すのを待つ。
	    XEvent tmp_event;
	    XNextEvent( dpy, &tmp_event );
	    if( tmp_event.type == KeyRelease )
	      break;
	  }
	  if( callback != NULL )
	    callback( this );
	  draw();
	}
      }
    }
    break;
  case ButtonPress:
    if( status & WIDGET_ACTIVE_MASK == 0 || event->xbutton.window != window )
      return;
    revers();
    while( 1 ){ // ボタンを離すのを待つ。
      XEvent tmp_event;
      XNextEvent( dpy, &tmp_event );
      if( tmp_event.type == ButtonRelease )
	break;
    }
    if( callback != NULL )
      callback( this );
    draw();
    break;
  }
}

//
// draw(): ボタンを描画する。
//
void ButtonWidget::draw()
{
  char buf[ 64 ];
  XFontPair* xfp = get_default_xfp();
  char* str = label==NULL?name:label;
  if( shortcut_key )
    sprintf( buf, "%s(%c)", str, shortcut_key );
  else
    strcpy( buf, str );
  draw_3d_box( 0, 0, _w, _h );

  XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );

  xfp->draw_string( window, gc, (TEXT_BORDER+BW), (TEXT_BORDER+BW), buf );
  if( shortcut_key ){
    char tmp[strlen(buf)-2+1];
    memcpy( tmp, buf, strlen(buf)-2 );
    tmp[ strlen(buf)-2 ] = 0;
    int ulx = xfp->text_width( tmp )+BW+TEXT_BORDER;
    int uly = xfp->height()+BW+TEXT_BORDER;
    XDrawLine(dpy,window,gc,ulx,uly,ulx+xfp->text_width("Z"),uly);
  }
}

//
// revers(): 表示を反転する。
//
void ButtonWidget::revers()
{
  char buf[ 64 ];
  XFontPair* xfp = get_default_xfp();
  char* str = label==NULL?name:label;
  if( shortcut_key )
    sprintf( buf, "%s(%c)", str, shortcut_key );
  else
    strcpy( buf, str );

  draw_pushed_3d_box( 0, 0, _w, _h );
  XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );

  xfp->draw_string( window, gc, (TEXT_BORDER+BW), (TEXT_BORDER+BW), buf );
  if( shortcut_key ){
    char tmp[strlen(buf)-2+1];
    memcpy( tmp, buf, strlen(buf)-2 );
    tmp[ strlen(buf)-2 ] = 0;
    int ulx = (TEXT_BORDER+BW) + xfp->text_width( tmp );
    int uly = (TEXT_BORDER+BW) + xfp->height();
    XDrawLine(dpy,window,gc,ulx,uly,ulx+xfp->text_width("Z"),uly);
  }
  XFlush( dpy );
}

//
// set_shortcut_key(): ショートカットキーを設定する。
//
void ButtonWidget::set_shortcut_key( char c )
{
  Widget::set_shortcut_key( c );
  char buf[256];
  char* str = label==NULL?name:label;
  sprintf( buf, "%s(%c)", str, shortcut_key );
  XFontPair* xfp = get_default_xfp();
  resize( xfp->text_width( buf ) + (TEXT_BORDER+BW)*2, _h );
}

//
// set_label():
//
void ButtonWidget::set_label( char* l )
{
  if( l != NULL ){
    XFontPair* xfp = get_default_xfp();
    Widget::set_label( l );
    resize( get_default_xfp()->text_width( label ) + (TEXT_BORDER+BW)*2, _h );
  }else{
    free( label );
    label = NULL;
  }
}
