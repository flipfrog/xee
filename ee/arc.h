//
// arc.h:
//
#ifndef __arc_h
#define __arc_h

class Arc: public Shape {
  double _x, _y, r;
  double sdx, sdy, edx, edy;
  double xx[3],yy[3];
  Boolean rrot;
  Boolean compute_geom();
  int line_width;
  LineMode line_mode;
  LineStyle line_style;
  double arrow_size;
  double ball_size;
public:

  Arc( Board* board );
  Arc( Arc* arc );
  virtual Shape* duplicate();
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

#endif /* __arc_h */
