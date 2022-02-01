//
// xcontext.h:
//
#ifndef __xcontext_h
#define __xcontext_h

class Dialog;
class SkkServ;
class Frame;

class Xcontext {
  Display* dpy;
  SkkServ* skkserv;
  List<Frame> frame_slot;
public:
  Xcontext( char* display_name );
  void append_frame( Frame* frame );
  void do_dialog_loop( Dialog* dialog, int x, int y );
  void do_main_loop();
  void connect_skkserv( char* skk_host );
  Display* get_dpy();
  SkkServ* get_skkserv();
};

#endif // __xcontext_h
