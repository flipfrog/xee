//
//
//
#ifndef __canvas_h
#define __canvas_h

class CanvasWidget: public Widget {
  void draw();
  ScrollbarWidget *h_scr, *v_scr;
  void set_scrollbar_geometry();
  int canvas_x, canvas_y, canvas_width, canvas_height;
  Window view_win, canvas_win;
  friend static void hscr_callback( ScrollbarWidget* scr );
  friend static void vscr_callback( ScrollbarWidget* scr );
public:
  CanvasWidget( Widget* parent, char* name );
  ~CanvasWidget();
  void (*event_func)
    ( XEvent* event, CanvasWidget* c,
     Display* dpy, GC gc, Window w, void* client_data );
  void* client_data;
  void add_input_mask( unsigned long mask );
  void event_proc( XEvent* event );
  void resize( int w, int h );
  void move_resize( int x, int y, int w, int h );
  void realize();
  void set_canvas_size( int w, int h );
  int get_canvas_width(){ return canvas_width; }
  int get_canvas_height(){ return canvas_height; }
  int get_complete_width();
  int get_complete_height();
  Window get_canvas_window(){ return canvas_win; }
};

#endif // __canvas_h
