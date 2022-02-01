//
// canvas.cc:
//
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <scrollbar.h>
#include <canvas.h>

#define BW WIDGET_BORDER_WIDTH
#define CANVAS_DEFAULT_WIDTH  200
#define CANVAS_DEFAULT_HEIGHT 200
#define SCROLLBAR_WIDTH 25
#define INPUT_MASKS WIDGET_DEFAULT_INPUT_MASKS

//
// hscr_callback():
//
static void hscr_callback( ScrollbarWidget* scr )
{
  CanvasWidget* canvas = (CanvasWidget*)(scr->client_data);
  Display* dpy = canvas->dpy;
  Window window = canvas->canvas_win;
  int pos = scr->get_position();
  canvas->canvas_x = scr->get_position();
  XMoveWindow( dpy, window, -canvas->canvas_x, -canvas->canvas_y );
}

//
// vscr_callback():
//
static void vscr_callback( ScrollbarWidget* scr )
{
  CanvasWidget* canvas = (CanvasWidget*)(scr->client_data);
  Display* dpy = canvas->dpy;
  Window window = canvas->canvas_win;
  canvas->canvas_y = scr->get_position();
  XMoveWindow( dpy, window, -canvas->canvas_x, -canvas->canvas_y );
}

//
// コンストラクタ
//
CanvasWidget::CanvasWidget( Widget* p, char* n ): Widget( p, n )
{
  h_scr = new ScrollbarWidget( this, "H-scrollbar" );
  h_scr->set_horizontal();
  h_scr->client_data = this;
  h_scr->callback = hscr_callback;
  v_scr = new ScrollbarWidget( this, "V-scrollbar" );
  v_scr->set_vertical();
  v_scr->client_data = this;
  v_scr->callback = vscr_callback;
  h_scr->
    move_resize( 0, _h-SCROLLBAR_WIDTH, _w-SCROLLBAR_WIDTH, SCROLLBAR_WIDTH );
  v_scr->
    move_resize( _w-SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, _h-SCROLLBAR_WIDTH );

  canvas_width = CANVAS_DEFAULT_WIDTH;
  canvas_height = CANVAS_DEFAULT_HEIGHT;
  canvas_x = canvas_y = 0;
  view_win = XCreateSimpleWindow( dpy, window, 0, 0,
				 _w-SCROLLBAR_WIDTH, _h-SCROLLBAR_WIDTH,
				 1, pixels[WIDGET_FOREGROUND_PIXEL],
				 pixels[WIDGET_BACKGROUND_PIXEL] );
  XSelectInput( dpy, view_win, INPUT_MASKS );
  canvas_win = XCreateSimpleWindow( dpy, view_win, 0, 0,
				   canvas_width, canvas_height,
				   1, pixels[WIDGET_FOREGROUND_PIXEL],
				   pixels[WIDGET_BACKGROUND_PIXEL] );
  XSelectInput( dpy, canvas_win, INPUT_MASKS );
  event_func = NULL;
  widget_type = WT_CANVAS;
}

//
// デストラクタ
//
CanvasWidget::~CanvasWidget()
{
  delete h_scr;
  delete v_scr;
}

//
// event_proc():
//
void CanvasWidget::event_proc( XEvent* event )
{
  if( status & WIDGET_ACTIVE_MASK == 0 )
    return;
  if( (event->xany.window == window || event->xany.window == view_win ||
     event->xany.window == canvas_win)&& event_func != NULL ){
    event_func( event, this, dpy, gc, canvas_win, client_data );
  }else{
    v_scr->event_proc( event );
    h_scr->event_proc( event );
  }
}

//
// realize():
//
void CanvasWidget::realize()
{
  Widget::realize();
  v_scr->realize();
  h_scr->realize();
  XMapWindow( dpy, view_win );
  XMapWindow( dpy, canvas_win );
}

//
// resize():
//
void CanvasWidget::resize( int w, int h )
{
  Widget::resize( w, h );
  h_scr->
    move_resize( 0, _h-SCROLLBAR_WIDTH, _w-SCROLLBAR_WIDTH, SCROLLBAR_WIDTH );
  v_scr->
    move_resize( _w-SCROLLBAR_WIDTH, 0, SCROLLBAR_WIDTH, _h-SCROLLBAR_WIDTH );
  XResizeWindow( dpy, view_win, _w-SCROLLBAR_WIDTH-4, _h-SCROLLBAR_WIDTH-4 );
  XMoveWindow( dpy, canvas_win, 0, 0 );
  h_scr->set_view_len( _w - v_scr->w() );
  if( canvas_width <= _w-SCROLLBAR_WIDTH )
    h_scr->set_status( h_scr->get_status()&(~WIDGET_ACTIVE_MASK) );
  else
    h_scr->set_status( h_scr->get_status()|WIDGET_ACTIVE_MASK );

  v_scr->set_view_len( _h - h_scr->h() );
  if( canvas_height <= _h-SCROLLBAR_WIDTH )
    v_scr->set_status( v_scr->get_status()&(~WIDGET_ACTIVE_MASK) );
  else
    v_scr->set_status( v_scr->get_status()|WIDGET_ACTIVE_MASK );

  h_scr->set_position( 0 );
  v_scr->set_position( 0 );
}

//
// move_resize():
//
void CanvasWidget::move_resize( int x, int y, int w, int h )
{
  Widget::move_resize( x, y, w, h );
  h_scr->
    move_resize(0, _h-SCROLLBAR_WIDTH+1, _w-SCROLLBAR_WIDTH, SCROLLBAR_WIDTH);
  v_scr->
    move_resize(_w-SCROLLBAR_WIDTH+1, 0, SCROLLBAR_WIDTH, _h-SCROLLBAR_WIDTH);
  XResizeWindow( dpy, view_win, _w-SCROLLBAR_WIDTH-4, _h-SCROLLBAR_WIDTH-4 );
  XMoveWindow( dpy, canvas_win, 0, 0 );
  h_scr->set_view_len( _w - v_scr->w() );
  if( canvas_width <= _w-SCROLLBAR_WIDTH )
    h_scr->set_status( h_scr->get_status()&(~WIDGET_ACTIVE_MASK) );
  else
    h_scr->set_status( h_scr->get_status()|WIDGET_ACTIVE_MASK );

  v_scr->set_view_len( _h - h_scr->h() );
  if( canvas_height <= _h-SCROLLBAR_WIDTH )
    v_scr->set_status( v_scr->get_status()&(~WIDGET_ACTIVE_MASK) );
  else
    v_scr->set_status( v_scr->get_status()|WIDGET_ACTIVE_MASK );

  h_scr->set_position( 0 );
  v_scr->set_position( 0 );
}

//
// add_input_mask():
//
void CanvasWidget::add_input_mask( unsigned long mask )
{
  XSelectInput( dpy, canvas_win, INPUT_MASKS|mask );
}

//
// set_canvas_size():
//
void CanvasWidget::set_canvas_size( int w, int h )
{
  canvas_width = w, canvas_height = h;
  XMoveResizeWindow( dpy, canvas_win, canvas_x, canvas_y, w, h );
  h_scr->set_object_len( canvas_width );
  h_scr->set_view_len( _w - v_scr->w() );
  h_scr->set_position( 0 );
  if( canvas_width <= _w-SCROLLBAR_WIDTH )
    h_scr->set_status( h_scr->get_status()&(~WIDGET_ACTIVE_MASK) );
  else
    h_scr->set_status( h_scr->get_status()|WIDGET_ACTIVE_MASK );

  v_scr->set_object_len( canvas_height );
  v_scr->set_view_len( _h - h_scr->h() );
  v_scr->set_position( 0 );
  if( canvas_height <= _h-SCROLLBAR_WIDTH )
    v_scr->set_status( v_scr->get_status()&(~WIDGET_ACTIVE_MASK) );
  else
    v_scr->set_status( v_scr->get_status()|WIDGET_ACTIVE_MASK );
}

//
// get_complete_width():
//
int CanvasWidget::get_complete_width()
{
  return canvas_width + SCROLLBAR_WIDTH + 4;
}

//
// get_complete_height():
//
int CanvasWidget::get_complete_height()
{
  return canvas_height + SCROLLBAR_WIDTH + 4;
}
