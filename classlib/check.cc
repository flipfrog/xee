//
// check.cc: チェックウィジェットクラス
//
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <local_types.h>
#include <list.h>
#include <X11/Xlib.h>
#include <xfontpair.h>
#include <widget.h>
#include <check.h>

#define BW WIDGET_BORDER_WIDTH

//
// CheckWidget():コンストラクタ  
//
CheckWidget::CheckWidget( Widget* p, char* n ) :Widget( p, n )
{
  char buf[256];
  sprintf( buf, "%sZZ", name );
  XFontPair* xfp = get_default_xfp();
  _w = xfp->text_width( buf ) + BW*6;
  _h = xfp->height() + BW*4;
  XResizeWindow( dpy, window, _w, _h );
  widget_type = WT_CHECK;
}

CheckWidget::CheckWidget( CheckWidget* check ): Widget( check )
{
  value = check->value;
  return;
}

//
// draw(): チェックウィジェットを描画する。
//
void CheckWidget::draw()
{
  XFontPair* xfp = get_default_xfp();
  int font_height = xfp->height();
  int two_width = xfp->text_width( "Ｚ" );
  draw_pushed_3d_frame( 0, 0, _w, _h );
  if( value ){
    draw_pushed_3d_frame( BW*2, BW*2, two_width, _h-BW*4 );
    XSetForeground( dpy, gc, pixels[WIDGET_CHECK_PIXEL] );
    XFillRectangle( dpy, window, gc, BW*3, BW*3, two_width-BW*2, _h-BW*6 );
  }else{
    draw_3d_frame( BW*2, BW*2, two_width, _h-BW*4 );
  }
  XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );
  xfp->draw_string( window, gc, two_width+BW*4, BW*2, label==NULL?name:label );
}

//
// interactive(): イベント情報を参照してインタラクティブ処理を行う。
//
void CheckWidget::event_proc( XEvent* event )
{
  switch( event->type ){
  case Expose:
    draw();
    break;
  case ButtonPress:
    if( status & WIDGET_ACTIVE_MASK == 0 )
      return;
    if( event->xbutton.window == window ){
      value = !value;
      draw();
      if( callback != NULL )
	callback( this );
    }
    break;
  }
}

//
// set_label():
//
void CheckWidget::set_label( char* l )
{
  Widget::set_label( l );
  char buf[256];
  sprintf( buf, "%sZZ", l );
  XFontPair* xfp = get_default_xfp();
  _w = xfp->text_width( buf ) + BW*6;
  _h = xfp->height() + BW*4;
  XResizeWindow( dpy, window, _w, _h );
}
