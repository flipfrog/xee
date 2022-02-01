//
// panel.h:
//
#ifndef __panel_h
#define __panel_h

class Frame;

class PanelWidget: public Widget{
  List<Widget> widget_slot;
  Boolean threeD_edge;
public:
  PanelWidget( Display* d, Window p, char* name );
  PanelWidget( Widget* parent, char* name );
  PanelWidget( Frame* frame, char* name );
  void event_proc( XEvent* event );
  void append_widget( Widget* widget );
  void realize();
  void resize( int w, int y );
  void set_3d_edge(){ threeD_edge = True; }
  Widget* search_by_name_widget( char* n );
};

#endif // __panel_h
