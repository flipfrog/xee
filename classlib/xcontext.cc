//
// xcontext.cc:
//
#include <stdio.h>
#include <string.h>
#include <errno.h>
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

//
// コンストラクタ
//
Xcontext::Xcontext( char* dpy_name )
{
  dpy = XOpenDisplay( dpy_name );
  if( dpy == NULL ){
    fprintf( stderr, "Xcontext::cannot open display %s\n", dpy_name );
    exit(errno);
  }
  skkserv = NULL;
}

//
// do_dialog_loop():
//
void Xcontext::do_dialog_loop( Dialog* dialog, int x, int y )
{
  dialog->realize();
  dialog->set_dialog_loop_f();
  while( dialog->get_dialog_loop_f() ){
    XEvent event;
    XNextEvent( dpy, &event );
    for( int i = 0 ; i < frame_slot.count() ; i++ )
      frame_slot.get(i)->event_proc( &event );
    dialog->event_proc( &event );
  }
  dialog->unrealize();
}

//
// do_main_loop():
//
void Xcontext::do_main_loop()
{
  for( int i = 0 ; i < frame_slot.count() ; i++ )
    frame_slot.get(i)->realize();
  while(1){
    XEvent event;
    XNextEvent( dpy, &event );
    for( i = 0 ; i < frame_slot.count() ; i++ )
      frame_slot.get(i)->event_proc( &event );
  }
}

//
// append_frame():
//
void Xcontext::append_frame( Frame* frame )
{
  frame_slot.append( frame );
}

//
// get_dpy():
//
Display* Xcontext::get_dpy()
{
  return dpy;
}

//
// get_skkserv():
//
SkkServ* Xcontext::get_skkserv()
{
  return skkserv;
}

//
// connect_skkserv():
//
void Xcontext::connect_skkserv( char* skk_host )
{
  skkserv = new SkkServ( skk_host!=NULL?skk_host:"localhost" );
}
