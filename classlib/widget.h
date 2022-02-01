//
// widget.h: ガジェットのコアクラス。
//
#ifndef __widget_h
#define __widget_h

#define WIDGET_ACTIVE_MASK 1

#define WIDGET_FOREGROUND_PIXEL 0
#define WIDGET_BACKGROUND_PIXEL 1
#define WIDGET_HIGHLIGHT_PIXEL  2
#define WIDGET_SHADOW_PIXEL     3
#define WIDGET_CHECK_PIXEL      4

#define WIDGET_BORDER_WIDTH1 2
#define WIDGET_BORDER_WIDTH2 1
#define WIDGET_BORDER_WIDTH (WIDGET_BORDER_WIDTH1+WIDGET_BORDER_WIDTH2)
#define WIDGET_DEFAULT_INPUT_MASKS (KeyPressMask|KeyReleaseMask| \
				    ButtonPressMask|ButtonReleaseMask| \
				    OwnerGrabButtonMask|ExposureMask)

typedef enum {
  WT_BUTTON, WT_CANVAS, WT_CHECK, WT_ICON, WT_MENUBAR,
  WT_MESSAGE, WT_PANEL, WT_SCROLLBAR, WT_TEXT, WT_TILE,
} WidgetType;


class Widget{
protected:
  XFontPair* find_font_pair( char* hname, char* zname );
  XFontPair* get_default_xfp()
    { return find_font_pair( default_hname, default_zname ); }
  Window parent;
  int status;
  char shortcut_key;
  char* label;
  char* name;
  int value;
  int _x, _y, _w, _h;
  char *default_hname, *default_zname;
  void draw_3d_box( int x, int y, int w, int h )
    { draw_3d_box( window, x, y, w, h ); }
  void draw_pushed_3d_box( int x, int y, int w, int h )
    { draw_pushed_3d_box( window, x, y, w, h ); }
  void draw_3d_frame( int x, int y, int w, int h )
    { draw_3d_frame( window, x, y, w, h ); }
  void draw_pushed_3d_frame( int x, int y, int w, int h )
    { draw_pushed_3d_frame( window, x, y, w, h ); }
  void draw_3d_box( Window w, int x, int y, int w, int h );
  void draw_pushed_3d_box( Window w, int x, int y, int w, int h );
  void draw_3d_frame( Window w, int x, int y, int w, int h );
  void draw_pushed_3d_frame( Window w, int x, int y, int w, int h );
  void draw_Utriangle( Window w, int x, int y, int w, int h );
  void draw_Dtriangle( Window w, int x, int y, int w, int h );
  void draw_Ltriangle( Window w, int x, int y, int w, int h );
  void draw_Rtriangle( Window w, int x, int y, int w, int h );
  void create_pixels();
public:
  WidgetType widget_type;
  Window window;
  Display* dpy;
  GC gc;
  unsigned long pixels[5];
  Widget( Display* dpy, Window parent, char* name );
  Widget( Widget* parent, char* name );
  Widget( Widget* widget );
  virtual ~Widget();
  void (*callback)( Widget* widget );
  void* client_data;
  inline char* get_name(){ return name; }
  virtual void event_proc( XEvent* event ){}
  virtual void set_label( char* l );
  virtual void set_shortcut_key( char c );
  inline virtual int get_value(){ return value; }
  virtual void set_value(int v );
  inline virtual int& x(){ return _x; }
  inline virtual int& y(){ return _y; }
  inline virtual int& w(){ return _w; }
  inline virtual int& h(){ return _h; }
  inline char* get_label(){ return label; }
  int get_status(){ return status; }
  void set_status( int s ){ status = s; }
  virtual void move( int x, int y );
  virtual void resize( int w, int h );
  virtual void move_resize( int x, int y, int w, int h );
  virtual void realize(){ XMapWindow( dpy, window ); }
};

#endif // __widget_h
