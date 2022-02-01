//
// shape.h:
//
#ifndef __shape_h
#define __shape_h

class Board;

class Shape{
protected:
  ShapeType shape_type;
  Board* board;
  int proc_phase;
  char *dot_pat, *dash_pat;
public:
  List<Shape> shape_slot;
  Shape( Board* b );
  Shape( Shape* shape );
  virtual ~Shape(){}
  virtual Shape* duplicate(){ return new Shape( this ); }
  virtual void draw_gp( Window window, int x, int y ){}
  virtual void draw( Window window, int x, int y ){}
  virtual void t_draw(Window w, double scale_x, double scale_y, int x, int y);
  virtual Boolean hit( int x, int y, int hw ){ return False; }
  virtual Boolean contain( int x, int y, int w, int h ){ return False; }
  virtual EditResult move( XEvent* event );
  virtual EditResult layout( XEvent* event ){ return EDIT_COMPLETE; }
  virtual EditResult resize( XEvent* event ){ return EDIT_COMPLETE; }
  virtual void reversx(){}
  virtual void reversy(){}
  virtual void rotate( XEvent* event ){}
  virtual void update(){}
  virtual void save( FILE* fp ){}
  virtual void tex( FILE*fp, double h, double x, double y ){}
  virtual void xfig( FILE* fp, double x, double y ){}
  virtual void bound( double& x, double& y, double& w, double& h ){}
  virtual void translate( double x, double y ){}
  virtual void scale( double rx, double ry ){}
  virtual void ungroup( Window window ){}
  ShapeType get_shape_type(){ return shape_type; }
  virtual Boolean load_file( FILE* fp ){ return True; };
};

#endif /* __shape_h */
