//
// oval.h:
//
#ifndef __oval_h
#define __oval_h

class Oval: public Shape {
  double _x, _y, _w, _h;
  int line_width;
  LineStyle line_style;
  Boolean fill_mode;
  void _draw( Window window, int x1, int y1, int x2, int y2 );
public:
  Oval( Board* board );
  Oval( Oval* oval );
  Shape* duplicate();
  void draw( Window window, int x, int y );
  void draw_gp( Window window, int x, int y );
  Boolean hit( int x, int y, int hw );
  Boolean contain( int x, int y, int w, int h );
  EditResult layout( XEvent* event );
  void update();
//  EditResult resize( XEvent* event );
  void rotate( XEvent* event );
  void bound( double& x, double& y, double& w, double& h );
  void translate( double x, double y );
  void scale( double rx, double ry );
  void save( FILE* fp );
  void tex( FILE* fp, double bh, double x, double y );
  void xfig( FILE* fp, double x, double y );
  Boolean load_file( FILE* fp );
};

#endif /* __oval_h */
