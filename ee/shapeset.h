//
// shapeset.h:
//
#ifndef __shapeset_h
#define __shapeset_h

class Shape;

class ShapeSet {
  List<Shape> shape_slot;
public:
  ShapeSet();
  Boolean load_file( FILE* fp );
  Boolean save_file( FILE* fp );
  Boolean tex_file( FILE* fp );
  Boolean fig_file( FILE* fp );
  Shape* find_hit_shape( int x, int y );
  int count_shape()
    { return shape_slot.count(); }
  void append_shape( Shape* shape )
    { shape_slot.append( shape ); }
  Shape* get_shape( int i )
    { return shape_slot.get(i); }
  void unlink_shape( ListOrigin o, int p, int c )
    { shape_slot.unlink( o, p, c ); }
  void draw_shape( Window window, int x, int y );
  void draw_shape_gp( Window window, int x, int y );
  void set_virtual_link( LinkState ls )
    { shape_slot.set_virtual_link( ls ); }
};

#endif // __shapeset_h
