//
// spline.h:
//
#ifndef __spline_h
#define __spline_h

class Spline: public Shape {
  LineStyle line_style;
  LineMode line_mode;
  double arrow_size;
  double ball_size;
  int line_width;
  List<Point> point_slot;
  List<Point> wpoint_slot;
  List<Point> ipoint_slot;
  void make_spline( List<Point>* point_slot, List<Point>* wpoint_slot );
  void draw_ipoint( Window window );
public:
  Spline( Board* board );
  Spline( Spline* spline );
  Shape* duplicate();
  void draw( Window window, int x, int y );
  void draw_gp( Window window, int x, int y );
  Boolean hit( int x, int y, int hw );
  Boolean contain( int x, int y, int w, int h );
  EditResult layout( XEvent* event );
  EditResult resize( XEvent* event );
  void reversx();
  void reversy();
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

#endif /* __spline_h */
