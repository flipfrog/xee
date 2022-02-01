//
// panel.cc:
//
#include <stdio.h>
#include <malloc.h>
#include <X11/Xlib.h>
#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <panel.h>
#include <frame.h>

#define PANEL_BORDER_WIDTH 5

//
// PanelWidget(): コンストラクタ
//
PanelWidget::PanelWidget( Display* d, Window p, char* n ) :Widget( d, p, n )
{
  XSelectInput( dpy, window, WIDGET_DEFAULT_INPUT_MASKS|StructureNotifyMask );
  widget_type = WT_PANEL;
  threeD_edge = False;
  widget_slot.set_virtual_link( LIST_REAL );
}

//
// PanelWidget(): コンストラクタ
//
PanelWidget::PanelWidget( Widget* p, char* n ) :Widget( p, n )
{
  XSelectInput( dpy, window, WIDGET_DEFAULT_INPUT_MASKS|StructureNotifyMask );
  widget_type = WT_PANEL;
  threeD_edge = False;
  widget_slot.set_virtual_link( LIST_REAL );
}

//
// コンストラクタ
//
PanelWidget::PanelWidget( Frame* f, char* n )
: Widget( f->get_dpy(), f->window, n )
{
  XSelectInput( dpy, window, WIDGET_DEFAULT_INPUT_MASKS|StructureNotifyMask );
  widget_type = WT_PANEL;
  threeD_edge = False;
  widget_slot.set_virtual_link( LIST_REAL );
}  

//
// event_proc():
//
void PanelWidget::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case Expose:
      if( threeD_edge )
	draw_3d_frame( 0, 0, _w, _h );
      break;
    case ConfigureNotify:
      break;
    }
  }
  for( int i = 0 ; i < widget_slot.count() ; i++ )
    widget_slot.get(i)->event_proc( event );
}

//
// append_widget():
//
void PanelWidget::append_widget( Widget* _widget )
{
  widget_slot.append( _widget );
  int ww = 0, hh = 0;
  for( int i = 0 ; i < widget_slot.count() ; i++ ){
    Widget* widget = widget_slot.get(i);
    if( widget->x()+widget->w() > ww )
      ww = widget->x()+widget->w();
    if( widget->y()+widget->h() > hh )
      hh = widget->y()+widget->h();
  }
  if( ww != _w || hh != _h )
    resize( ww, hh );
}

//
// realize():
//
void PanelWidget::realize()
{
  Widget::realize();
  for( int i = 0 ; i < widget_slot.count() ; i++ )
    widget_slot.get(i)->realize();
}

//
// resize():
//
void PanelWidget::resize( int w, int h )
{
  for( int i = 0 ; i < widget_slot.count() ; i++ ){
    Widget* widget = widget_slot.get(i);
    switch( widget->widget_type ){
    case WT_CANVAS:
      int cw = widget->w(), ch = widget->h();
      if( widget->x()+widget->w() >= _w )
	cw = w - widget->x();
      if( widget->y()+widget->h() >= _h )
	ch = h - widget->y();
      if( cw != widget->w() || ch != widget->h() )
	widget->resize( cw, ch );
      break;
    case WT_MENUBAR:
      if( widget->x()+widget->w() >= _w )
	widget->resize( w - widget->x(), widget->h() );
      break;
    }
  }
  Widget::resize( w, h );
}

//
// search_by_name_widget():
//
Widget* PanelWidget::search_by_name_widget( char* n )
{
  for( int i = 0 ; i < widget_slot.count() ; i++ ){
    Widget* widget = widget_slot.get(i);
    if( strcmp( n, widget->get_name() ) == 0 )
      return widget;
  }
  return NULL;
}
