//
// frame.h:
//
#ifndef __frame_h
#define __frame_h

class Xcontext;
class Widget;
class PanelWidget;

class Frame{
protected:
  char* name;
  void draw();
  PanelWidget* panel;
public:
  int _x, _y, _w, _h;
  Xcontext* xcontext;
  Window window;
  void *client_data;
  Frame( Xcontext* xcontext, char* _name );
  ~Frame();
  void set_panel( PanelWidget* panel );
  void event_proc( XEvent* event );
  void realize();
  void realize( int x, int y );
  void unrealize();
  Widget* search_by_name_widget( char* n );
  Display* get_dpy();
};

class Dialog :public Frame {
  Boolean do_loop_f;
  Frame* parent;
public:
  Dialog( Frame* parent, char* _name );
  void done_dialog_loop(){ do_loop_f = False; }
  void set_dialog_loop_f(){ do_loop_f = True; }
  Boolean get_dialog_loop_f(){ return do_loop_f; }
  void realize();
};

#endif // __frame_h
