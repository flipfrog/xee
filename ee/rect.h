//
// rect.h:
//
#ifndef __rect_h
#define __rect_h

class Rect: public Shape {
  double _x, _y;
  double _w, _h;
  int line_width;
  LineStyle line_style;
  Boolean fill_mode;
public:
  Rect( Board* board );
  Rect( Rect* rect );
  Shape* duplicate();
  void draw( Window window, int x, int y );
  Boolean hit( int x, int y, int hw );
  Boolean contain( int x, int y, int w, int h );
  EditResult layout( XEvent* event );
  EditResult resize( XEvent* event );
  void rotate( XEvent* event );
  void update();
  void bound( double& x, double& y, double& w, double& h );
  void translate( double x, double y );
  void scale( double rx, double ry );
  void save( FILE* fp );
  void tex( FILE* fp, double bh, double x, double y );
  void xfig( FILE* fp, double x, double y );
  Boolean load_file( FILE* fp );
};

#endif /* __rect_h */
