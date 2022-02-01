//
// frame.cc:
//
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <panel.h>

#include <frame.h>
#include <xcontext.h>

//
// Frame(): コンストラクタ
//
Frame::Frame( Xcontext* _xcontext, char* _name )
{
  panel = NULL;
  _x = _y = 0;
  name = strdup( _name );
  xcontext = _xcontext;
  Display* dpy = xcontext->get_dpy();

  window = XCreateSimpleWindow( dpy, DefaultRootWindow(dpy), 100, 100,
			       100, 100, 0,
			       BlackPixel(dpy,DefaultScreen(dpy)),
			       WhitePixel(dpy,DefaultScreen(dpy)) );
  XSizeHints hint;
  hint.x = 100, hint.y = 100;
  hint.width = _w, hint.height = _h;
  hint.flags = PPosition|PSize;
  XSetNormalHints( dpy, window, &hint );
  XSelectInput( dpy, window, StructureNotifyMask );
}


//
// ~Frame(): デストラクタ
//
Frame::~Frame()
{
  delete name;
  // この呼出しは、delete panel; より先に呼出さなければならない。
  // でないと、DestroyNotify がインフェリアに送信される可能性があるから。
  XDestroyWindow( xcontext->get_dpy(), window );
  if( panel != NULL )
    delete panel;
}

//
// set_panel():
//
void Frame::set_panel( PanelWidget* _panel )
{
  panel = _panel;
}

//
// realize():
//
void Frame::realize( int x, int y )
{
  _x = x, _y = y;
  realize();
}

//
// realize():
//
void Frame::realize()
{
  _w = panel->x()+panel->w(), _h = panel->y()+panel->h();
  XMoveResizeWindow( xcontext->get_dpy(), window, _x, _y, _w, _h );
  XMapWindow( xcontext->get_dpy(), window );
  panel->realize();
}

//
// event_proc():
//
void Frame::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case ConfigureNotify:
      _w = event->xconfigure.width, _h = event->xconfigure.height;
      panel->resize( _w, _h );
      break;
    }
  }
  panel->event_proc( event );
}

//
// unrealize():
//
void Frame::unrealize()
{
  XUnmapWindow( xcontext->get_dpy(), window );
}

//
// get_dpy():
//
Display* Frame::get_dpy()
{
  return xcontext->get_dpy();
}

//
// コンストラクタ
//
Dialog::Dialog( Frame* p, char* _name ): Frame( p->xcontext, _name )
{
  parent = p;
  XSetTransientForHint( xcontext->get_dpy(), window, parent->window );
}

//
// search_by_name_widget():
//
Widget* Frame::search_by_name_widget( char* n )
{
  return panel->search_by_name_widget( n );
}

//
// realize():
//
void Dialog::realize()
{
  // 親フレームを得る。
  // 通常ウィンドウマネージャによりReparentWindow()が行われているため、
  // ルートウィンドウ直下のウィンドウまで、リストを追う。
  Window root, last_win, p_win, curr = parent->window;
  Window def_root = DefaultRootWindow( xcontext->get_dpy() );
  for( curr = parent->window ; p_win != def_root ; curr = p_win ){
    unsigned int n;
    Window *c_wins;
    if( !XQueryTree( xcontext->get_dpy(), curr, &root, &p_win, &c_wins, &n ) ){
      fprintf( stderr, "XQueryTree(): error!\n");
      exit(0);
    }
    XFree( c_wins );
    last_win = curr;
  }

  _w = panel->x()+panel->w(), _h = panel->y()+panel->h();

  int rx, ry;
  unsigned int rw, rh, bw, d;
  XGetGeometry(xcontext->get_dpy(),last_win,&root,&rx,&ry,&rw,&rh,&bw,&d);
  int x = (parent->_w - _w)/2 + rx, y = (parent->_w - _h)/2 + ry;
  Frame::realize( x, y );
}
