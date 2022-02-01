//
// line.h:
//
#ifndef __line_h
#define __line_h

class Point;

class Line: public Shape{
  List<Point> point_slot;
  LineStyle line_style;
  LineMode line_mode;
  double arrow_size;
  double ball_size;
  int line_width;
public:
  Line( Board* board );
  Line( Line* line );
  Shape* duplicate();
  void draw( Window window, int x, int y );
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

#endif /* __line_h */
