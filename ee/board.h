//
// board.h:
//
#ifndef __board_h
#define __board_h

class Shape;
class Widget;
class Frame;
class Dialog;
class PanelWidget;
class CanvasWidget;

class Xcontext;
class SkkFep;
class XFontPair;

class Board {
  void make_panel();

  void layout_shape( XEvent* event, int mx, int my );
  void ungroup_shape( XEvent* event, int mx, int my );
  void move_shape( XEvent* event, int mx, int my );
  void delete_shape( XEvent* event, int mx, int my );
  void copy_shape( XEvent* event, int mx, int my );
  void resize_shape( XEvent* event, int mx, int my );
  void reversx_shape( XEvent* event, int mx, int my );
  void reversy_shape( XEvent* event, int mx, int my );
  void rotate_shape( XEvent* event, int mx, int my );
  void update_shape( XEvent* event, int mx, int my );
  friend void
    board_event_func( XEvent* event, CanvasWidget* c, Display* dpy,
		     GC gc, Window w, void* client_data );

public:
  CanvasWidget* canvas;
  Dialog* tex_dialog;
  Dialog* fig_dialog;
  Dialog* loadsave_dialog;
  Dialog* setup_dialog;
  GC gc;
  Xcontext* xcontext;
  Frame* frame;
  ShapeSet shapeset;
  ShapeSet deleted_shape;
  unsigned long grid_pixel, bg_pixel;
  unsigned long shape_mask, layout_mask, cursor_mask;

  Board( Xcontext* _xcontext );
  ~Board();
  XFontPair* xfp;
  SkkFep* skkfep;

  void map_window();

  int _x, _y, _w, _h;
  int paper_w, paper_h;
  Boolean batch_mode;
  ShapeType insert_type;
  EditMode edit_mode;
  Boolean fill_mode;
  LineMode line_mode;
  LineStyle line_style;
  RoundStyle round_style;
  CursorMode cursor_mode;
  int line_width;
  int arrow_size;
  int ball_size;
  int grip_radious;
  int grid_width;
  int border_width;
  FontSize font_size;
  Boolean grid_disp;
  Boolean grip_disp;
  Boolean cursor_quontize;
  Boolean update_f;
  Boolean disp_small_str;
  Boolean msdos_compatibility;
  Boolean real_moving;
  double scale_rate;

  char last_epc[256];

  void event_proc( XEvent* event );
  void draw();
  void draw_gp();
  void scale( double rate );
  void alldel();

  void quontize( int& x, int& y );
  Boolean load( char* fname );
  Boolean save( char* fname );
  Boolean tex( char* fname );
  Boolean xfig( char* fname );
};

#endif /* __board_h */
