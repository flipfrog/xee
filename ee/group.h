//
// group.h:
//
#ifndef __group_h
#define __group_h

class Group: public Shape{
public:
  double _x, _y, _w, _h;
  Group( Board* board );
  Group( Group* group );
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
  void ungroup( Window window );
  Boolean load_file( FILE* fp );
};

#endif /* __group_h */
