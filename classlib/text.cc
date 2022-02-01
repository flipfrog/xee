//
// text.cc:
//
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <skkserv.h>
#include <skkfep.h>
#include <text.h>

#define TEXT_BW 3
#define BW (WIDGET_BORDER_WIDTH+TEXT_BW)
#define DEFAULT_INPUT_LEN 15

//
// TextWidget(): コンストラクタ
//
TextWidget::TextWidget( SkkServ* skkserv, Widget* p, char* n ):Widget( p, n )
{
  xfp = get_default_xfp();
  char tmp[256];
  memset( tmp, 'Z', DEFAULT_INPUT_LEN );
  tmp[ DEFAULT_INPUT_LEN ] = 0;
  resize( xfp->text_width( tmp )+BW*3, xfp->height()+BW*2 );

  input_len = DEFAULT_INPUT_LEN;
  if( (buf = (char*)malloc( input_len+1 )) == NULL ){
    fprintf( stderr, "TextWidget::TextWidget(): cannot allocate memory.\n");
    exit(errno);
  }
  memset( buf, 0, input_len+1 );
  skkfep = new SkkFep( xfp, skkserv );
  skkfep->set_pixels( pixels[WIDGET_FOREGROUND_PIXEL],
		     pixels[WIDGET_BACKGROUND_PIXEL] );
  skkfep->set_buffer( buf, input_len );
  inside_pointer = False;
  XSelectInput( dpy, window,
	       WIDGET_DEFAULT_INPUT_MASKS|EnterWindowMask|LeaveWindowMask );
  cursor = XCreateFontCursor( dpy, XC_xterm );
  XDefineCursor( dpy, window, cursor );
  widget_type = WT_TEXT;
}

//
// デストラクタ
//
TextWidget::~TextWidget()
{
  delete skkfep;
  free( buf );
  XFreeCursor( dpy, cursor );
}

//
// interactive():
//
void TextWidget::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case Expose:
      draw();
      break;
    case KeyPress:
      if( status & WIDGET_ACTIVE_MASK == 0 )
	return;
      if( skkfep->event_proc( gc, event, xfp->text_width(label)+BW*2, BW ) ){
	if( callback != NULL )
	  callback( this );
      }
      break;
    case EnterNotify:
      if( status & WIDGET_ACTIVE_MASK == 0 )
	return;
      if( !inside_pointer ){
	inside_pointer = True;
	draw();
      }
      break;
    case LeaveNotify:
      if( status & WIDGET_ACTIVE_MASK == 0 )
	return;
      if( inside_pointer ){
	inside_pointer = False;
	draw();
      }
      break;
    }
  }
}

//
// draw():
//
void TextWidget::draw()
{
  int xx = xfp->text_width( label )+BW;
  int ww = _w - xfp->text_width( label )-BW*2;
  if( inside_pointer ){
    draw_pushed_3d_frame( xx, 0, ww, _h );
  }else{
    draw_3d_frame( xx, 0, ww, _h );
  }
  skkfep->draw( dpy, gc, window, xfp->text_width(label)+BW*2, BW );
  XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );
  if( label != NULL )
    xfp->draw_string( window, gc, BW, BW, label );
}

//
// set_string():
//
void TextWidget::set_string( char* s )
{
  strncpy( buf, s, input_len );
  buf[input_len] = 0;
  skkfep->set_buffer( buf, input_len );
//  XClearArea( dpy, window, 0, 0, 0, 0, True );
}

//
// set_label():
//
void TextWidget::set_label( char* l )
{
  Widget::set_label( l );
  char tmp[256];
  memset( tmp, 'Z', input_len );
  tmp[ input_len ] = 0;
  resize( xfp->text_width( label )+ xfp->text_width( tmp )+BW*3, _h );
}

//
// set_input_len():
//
void TextWidget::set_input_len( int len )
{
  free( buf );
  input_len = len;
  if( (buf = (char*)malloc( input_len+1 )) == NULL ){
    fprintf( stderr, "TextWidget::TextWidget(): cannot allocate memory.\n");
    exit(errno);
  }
  buf[0] = 0;
  skkfep->set_buffer( buf, input_len );
  char tmp[256];
  memset( tmp, 'Z', input_len );
  tmp[ input_len ] = 0;
  resize( xfp->text_width( label )+ xfp->text_width( tmp )+BW*3, _h );
}
