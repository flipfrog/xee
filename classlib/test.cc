#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <button.h>
#include <check.h>
#include <tile.h>
#include <message.h>
#include <icon.h>
#include <scrollbar.h>

#include <skkserv.h>
#include <skkfep.h>

#include <text.h>
#include <menubar.h>
#include <canvas.h>
#include <panel.h>
#include <frame.h>

#include <xcontext.h>

#include <bitmaps/aaa.xbm>
#include <bitmaps/circle.xbm>
#include <bitmaps/rect.xbm>
#include <bitmaps/cross.xbm>
#include <bitmaps/triangle.xbm>

#include "xterm.xpm"

static void
canvas_event_func( XEvent* event, CanvasWidget* c,
		  Display* dpy, GC gc, Window w, void* cd )
{
  switch( event->type ){
  case Expose:
    int width = c->get_canvas_width();
    int height = c->get_canvas_height();
    XSetForeground( dpy, gc, BlackPixel(dpy,DefaultScreen(dpy)) );
    XDrawLine( dpy, w, gc, 0, 0, width, height );
    XDrawLine( dpy, w, gc, 0, height, width, 0 );
    break;
  }
}

static void notice_func( Widget* widget )
{
  printf("押された。\n");
}

static void menu_proc( Menuitem* item )
{
  printf("Ouch!\n");
}

static void quit_func( Widget* widget )
{
  exit(0);
}

static void scr_proc( Widget* widget )
{
  ScrollbarWidget* scr = (ScrollbarWidget*)widget;
  printf("pos=%d\n", scr->get_position() );
}

main( int ac, char** av )
{
  Xcontext xcontext( "unix:0" );
  xcontext.connect_skkserv( getenv("SKKSERVER") );

  Frame* frame = new Frame( &xcontext, "frame" );
  PanelWidget* panel = new PanelWidget( frame, "panel" );
  frame->set_panel( panel );

  ButtonWidget* button = new ButtonWidget( panel, "ボタンだす" );
  button->callback = quit_func;
  button->set_shortcut_key( 'q' );
  button->move( 100, 32 );
  panel->set_label( "ボタンだす" );
  panel->append_widget( button );

  CheckWidget* check = new CheckWidget( panel, "チェックだす" );
  check->callback = notice_func;
  check->move( button->x(), button->y()+button->h()+10 );
  panel->append_widget( check );

  TileWidget* tile = new TileWidget( panel, "タイルだす" );
  char* bits[] = { rect_bits, circle_bits, cross_bits, triangle_bits };
  for( int i = 0 ; i < 4 ; i++ )
    tile->append_bitmap( bits[i], 16, 16 );
  tile->move( 100, check->y()+check->h()+10 );
  tile->callback = notice_func;
  panel->append_widget( tile );

  MessageWidget* message = new MessageWidget( panel, "msg1" );
  message->set_label( "This is メッセージだす。" );
  message->move( 100, tile->y()+tile->h()+10 );
  panel->append_widget( message );

  IconWidget* icon = new IconWidget( panel, "アイコンだす。" );
//  icon->set_bitmap( aaa_bits, 64, 64 );
  icon->set_pixmap( tako_xpm );
  icon->move( 200, 200 );
  panel->append_widget( icon );

  ScrollbarWidget* scr = new ScrollbarWidget( panel, "スクロールバー" );
  scr->set_horizontal();
  scr->set_view_len( 10 );
  scr->set_object_len( 100 );
  scr->move_resize( 30, 180, 150, 20 );
//  scr->set_status( scr->get_status()&(~WIDGET_ACTIVE_MASK) );
  scr->callback = scr_proc;
  panel->append_widget( scr );

  TextWidget* text = new TextWidget(xcontext.get_skkserv(), panel, "テキスト");
  text->move( 10, 300 );
  text->set_label( "テキストだす:" );
  panel->append_widget( text );

  CanvasWidget* canvas = new CanvasWidget( panel, "canvas" );
  canvas->move_resize( 200, 350, 150, 150 );
  canvas->set_canvas_size( 300, 300 );
  canvas->event_func = canvas_event_func;
  panel->append_widget( canvas );

  MenubarWidget* menubar = new MenubarWidget( panel, "メニューバー" );
  menubar->set_label("メニューバー");
  panel->append_widget( menubar );

  int menu_no = menubar->append_menu( "メニュー(壱)" );
  for( i = 0 ; i < 4 ; i++ ){
    char buf[32];
    sprintf( buf, "item-1/%d", i );
    Menuitem* item = new Menuitem( buf );
    item->callback = menu_proc;
    menubar->append_menuitem( menu_no, item );
  }

  menu_no = menubar->append_menu( "メニュー(弐)" );
  for( i = 0 ; i < 6 ; i++ ){
    char buf[32];
    sprintf( buf, "いてむ-2/%d", i );
    Menuitem* item = new Menuitem( buf );
    menubar->append_menuitem( menu_no, item );
  }

  xcontext.append_frame( frame );
  xcontext.do_main_loop();
}
