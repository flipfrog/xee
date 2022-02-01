//
// tile.h:
//
#ifndef __tile_h
#define __tile_h

struct PixmapTab {
  Boolean xpm_f;
  Pixmap pixmap;
  int w, h;
};

class TileWidget: public Widget{
  List<PixmapTab> pixmap_slot;
  int column;
  int row;
  void draw();
public:
  TileWidget( Widget* p, char* name );
  ~TileWidget();
  void event_proc( XEvent* event );
  void append_bitmap( char* data, int data_w, int data_h );
  void append_pixmap( char** data );
  inline int get_column(){ return column; }
  void set_selected( int c );
  void set_column( int c );
};

#endif /* __tile_h */
