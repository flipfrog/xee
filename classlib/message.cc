//
// message.c:
//
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <X11/Xlib.h>
#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <message.h>

#define iskanji(x) ((unsigned char)(x)&(unsigned char)0x80)

//
// MessageWidget(): コンストラクタ
//
MessageWidget::MessageWidget( Widget* p, char* n ) :Widget( p, n )
{
  XFontPair* xfp = get_default_xfp();
  _h = xfp->height();
  _w = xfp->text_width( n );
  set_column( 100 );
  widget_type = WT_MESSAGE;
}

void MessageWidget::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case Expose:
      char buf[256];
      int y = 0, buf_pos = 0;
      XFontPair* xfp = get_default_xfp();
      XSetForeground( dpy, gc, pixels[ WIDGET_FOREGROUND_PIXEL ] );
      XSetBackground( dpy, gc, pixels[ WIDGET_BACKGROUND_PIXEL ] );
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      
      for( int i = 0 ; i < strlen(label) ; ){
	if( iskanji(label[i]) ){
	  if( buf_pos+2 > column ){
	    buf[ buf_pos ] = 0;
	    buf_pos = 0;
	    xfp->draw_string( window, gc, 0, y, buf );
	    y += xfp->height();
	  }
	  memcpy( &buf[ buf_pos ], &label[ i ], 2 );
	  buf_pos += 2;
	  i += 2;
	}else{
	  if( buf_pos+1 > column ){
	    buf[ buf_pos ] = 0;
	    buf_pos = 0;
	    xfp->draw_string( window, gc, 0, y, buf );
	    y += xfp->height();
	  }
	  buf[ buf_pos ] = label[ i ];
	  buf_pos++;
	  i++;
	}
      }
      if( buf_pos > 0 ){
	buf[ buf_pos ] = 0;
	xfp->draw_string( window, gc, 0, y, buf );
      }
      break;
    }
  }
}

//
// set_column(): カラムを設定する。
//
void MessageWidget::set_column( int c )
{
  column = c;

  if( label == NULL )
    return;

  XFontPair* xfp = get_default_xfp();
  int font_height = xfp->height();
  int font_width = xfp->text_width( "Z" );

  int y = 0, buf_pos = 0;
  for( int i = 0 ; i < strlen(label) ; ){
    if( iskanji(label[i]) ){
      if( buf_pos+2 > column ){
	buf_pos = 0;
	y += font_height;
      }
      buf_pos += 2;
      i += 2;
    }else{
      if( buf_pos+1 > column ){
	buf_pos = 0;
	y += font_height;
      }
      buf_pos++;
      i++;
    }
  }
  if( strlen(label) < column )
    _w = strlen(label) * font_width;
  else
    _w = column * font_width;
  _h = y;
  if( buf_pos > 0 )
    _h += font_height;
  resize( _w, _h );
}

//
// set_label(): ラベルを設定する。
//
void MessageWidget::set_label( char* l )
{
  Widget::set_label( l );
  set_column( column );
  resize( _w, _h );
}
