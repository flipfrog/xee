//
// menubar.c: メニューバーウィジェット
//
#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <widget.h>
#include <menubar.h>

#define BW WIDGET_BORDER_WIDTH
#define TEXT_BORDER 2
#define ITEM_WIDTH  (menu->_w-BW*2)
#define ITEM_HEIGHT (xfp->height()+BW*2+TEXT_BORDER*2)
#define ITEM_X      (BW)
#define ITEM_Y(x)   (BW+ITEM_HEIGHT*(x))
#define ITEM_SX     (ITEM_X+BW+TEXT_BORDER)
#define ITEM_SY(x)  (ITEM_Y(x)+BW+TEXT_BORDER)

//
// MenubarWidget(): コンストラクタ
//
MenubarWidget::MenubarWidget( Widget* p, char* _name ) :Widget( p, _name )
{
  XFontPair* xfp = get_default_xfp();
  _h = xfp->height() + BW*2 + TEXT_BORDER*2;
  _w = 100 + BW*2;
  widget_type = WT_MENUBAR;
  menu_slot.set_virtual_link( LIST_REAL );
}

//
// event_proc():
//
void MenubarWidget::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case ButtonPress:
      if( status & WIDGET_ACTIVE_MASK == 0 )
	return;
      int mx = event->xbutton.x, my = event->xbutton.y;
      XFontPair* xfp = get_default_xfp();
      for( int i = 0 ; i < menu_slot.count() ; i++ ){
	Menu* menu = menu_slot.get(i);
	int m_width = xfp->text_width( menu->label );
	if( mx >= menu->_x && mx < menu->_x+m_width ){
	  menu_proc( menu, event );
	}
      }
      break;
    case Expose:
      xfp = get_default_xfp();
      draw_3d_frame( 0, 0, _w, _h );
      XSetFunction( dpy, gc, GXcopy );
      XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );
      for( i = 0 ; i < menu_slot.count() ; i++ ){
	Menu* menu = menu_slot.get(i);
	xfp->draw_string( window, gc,
			 menu->_x, BW+TEXT_BORDER, menu->label );
      }
      break;
    }
  }
}

//
// append_menu():
//
int MenubarWidget::append_menu( char* m_label )
{
  XFontPair* xfp = get_default_xfp();
  Menu* menu = new Menu( m_label );
  int px = BW;
  for( int i = 0 ; i < menu_slot.count() ; i++ )
    px += xfp->text_width( menu_slot.get(i)->label )+10;
  menu->_x = px, menu->_y = _h;
  menu_slot.append( menu );
  resize( px+xfp->text_width(m_label)+10, _h );
  return menu_slot.count()-1;
}

//
// append_menuitem():
//
void MenubarWidget::append_menuitem( int menu_no, Menuitem* item )
{
  XFontPair* xfp = get_default_xfp();
  Menu* menu = menu_slot.get(menu_no);
  menu->item_slot.append( item );
  menu->_h = ITEM_HEIGHT*menu->item_slot.count()+BW*2;
  for( int i = 0 ; i < menu->item_slot.count() ; i++ ){
    int width = xfp->text_width(item->label)+BW*4+TEXT_BORDER*2;
    if( menu->_w < width )
      menu->_w = width;
  }
}

//
// menu_porc():
//
void MenubarWidget::menu_proc( Menu* menu, XEvent* prev_event )
{
  XFontPair* xfp = get_default_xfp();
  Window m_win = XCreateSimpleWindow( dpy, parent,
				     _x+menu->_x, _y+menu->_y,
				     menu->_w, menu->_h,
				     0, pixels[WIDGET_FOREGROUND_PIXEL],
				     pixels[WIDGET_FOREGROUND_PIXEL] );
  XSelectInput( dpy, m_win,
	       WIDGET_DEFAULT_INPUT_MASKS|PointerMotionMask|ButtonMotionMask|
	       LeaveWindowMask );
  XMapRaised( dpy, m_win );
  XUngrabPointer( dpy, prev_event->xbutton.time );

  int last_item = -1, item, mx, my;
  while(1){
    XEvent e;
    XNextEvent( dpy, &e );
    switch( e.type ){
    case Expose:
      if( e.xexpose.window == m_win ){
	draw_3d_frame( m_win, 0, 0, menu->_w, menu->_h );
	XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );  
	for( int i = 0 ; i < menu->item_slot.count() ; i++ )
	  xfp->draw_string( m_win, gc, ITEM_SX, ITEM_SY(i),
			   menu->item_slot.get(i)->label);
	if( last_item >= 0 ) mark_item( m_win, menu, last_item );
      }
      break;
    case LeaveNotify:
      if( last_item >= 0 ){
	unmark_item( m_win, menu, last_item );
	last_item = -1;
      }
      break;
    case MotionNotify:
      if( e.xmotion.window != m_win ){
	if( last_item >= 0 ){
	  unmark_item( m_win, menu, last_item );
	  last_item = -1;
	}
	break;
      }
      item = -1;
      mx = e.xmotion.x, my = e.xmotion.y;
      if( mx >= BW && mx < menu->_w-BW && my >= BW && my < menu->_h-BW )
	item = (my-BW)/ITEM_HEIGHT;

      if( item >= 0 && last_item != item ){
	if( last_item >= 0 )
	  unmark_item( m_win, menu, last_item );
	mark_item( m_win, menu, item );
	last_item = item;
      }else if( item < 0 && last_item >= 0 ){
	unmark_item( m_win, menu, last_item );
	last_item = -1;
      }
      break;
    case ButtonRelease:
      if( e.xbutton.time - prev_event->xbutton.time < 500 )
	break;
      if( e.xbutton.window == m_win ){
	if( last_item >= 0 ){
	  unmark_item( m_win, menu, last_item );
	  usleep( 50000 );
	  mark_item( m_win, menu, last_item );
	  usleep( 50000 );
	  unmark_item( m_win, menu, last_item );
	  usleep( 50000 );
	  mark_item( m_win, menu, last_item );
	  if( menu->item_slot.get(last_item)->callback != NULL ){
	    XDestroyWindow( dpy, m_win );
	    XFlush( dpy );
	    menu->item_slot.get(last_item)->
	      callback( menu->item_slot.get(last_item) );
	    last_item = -1;
	    return;
	  }
	}
      }
      XDestroyWindow( dpy, m_win );
      last_item = -1;
      return;
      break;
    }
  }
}

//
// mark_item():
//
void MenubarWidget::mark_item( Window win, Menu* menu, int item_no )
{
  XFontPair* xfp = get_default_xfp();
  Menuitem* item = menu->item_slot.get(item_no);
  draw_3d_frame( win, ITEM_X, ITEM_Y(item_no), ITEM_WIDTH, ITEM_HEIGHT );
  XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );  
  xfp->draw_string( win, gc, ITEM_SX, ITEM_SY(item_no), item->label );
  XFlush( dpy );
}  

//
// unmark_item():
//
void MenubarWidget::unmark_item( Window win, Menu* menu, int item_no )
{
  XFontPair* xfp = get_default_xfp();
  Menuitem* item = menu->item_slot.get(item_no);
  XSetForeground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
  XFillRectangle( dpy, win, gc, ITEM_X, ITEM_Y(item_no),
		 ITEM_WIDTH, ITEM_HEIGHT );
  XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );  
  xfp->draw_string( win, gc, ITEM_SX, ITEM_SY(item_no), item->label );
  XFlush( dpy );
}  
