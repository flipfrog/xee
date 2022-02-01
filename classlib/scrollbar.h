//
// scrbar.h:
//
#ifndef __scrbar_h
#define __scrbar_h

class ScrollbarWidget: public Widget{
  Boolean horizontal;
  int object_len;
  int view_len;
  int position;
  int move_step;
  unsigned long gray_pixel;
  void draw();
public:
  ScrollbarWidget( Widget* p, char* n );
  void event_proc( XEvent* event );
  void set_object_len( int l ){ object_len = l; }
  void set_view_len( int l ){ view_len = l; }
  void set_move_step( int s ){ move_step = s; }
  void set_position( int p ){ position = p; }
  int get_object_len(){ return object_len; }
  int get_position(){ return position; }
  int get_move_step( int s ){ return move_step; }
  void set_horizontal(){ horizontal = True; }
  void set_vertical(){ horizontal = False; }
  void (*callback)( ScrollbarWidget* widget );
};

#endif // __scrbar_h
