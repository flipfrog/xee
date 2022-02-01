//
// str.h:
//
#ifndef __str_h
#define __str_h

class Str: public Shape{
  double _x, _y;
  double _w, _h;
  char buf[256];
  FontSize font_size;
  RoundStyle round_style;
  double border_width;
  LineStyle line_style;
  int line_width;
public:
  Str( Board* board );
  Str( Str* str );
  Shape* duplicate();
  void draw( Window window, int x, int y );
  Boolean hit( int x, int y, int hw );
  Boolean contain( int x, int y, int w, int h );
  EditResult layout( XEvent* event );
  void update();
  void bound( double& x, double& y, double& w, double& h );
  void translate( double x, double y );
  void scale( double rx, double ry );
  void rotate( XEvent* event );
  void save( FILE* fp );
  void tex( FILE* fp, double bh, double x, double y );
  void xfig( FILE* fp, double x, double y );
  Boolean load_file( FILE* fp );
};

#endif /* __str_h */
