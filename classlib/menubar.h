//
// menubar.h:
//
#ifndef __menubar_h
#define __menubar_h

class Menuitem{
public:
  char* label;
  void (*callback)( Menuitem* item );
  void* client_data;
  Menuitem( char* _label ){
    if( (label = (char*)strdup( _label )) == NULL )
      exit(1);
    callback = NULL;
    client_data = NULL;
  }
  ~Menuitem(){ free( label ); }
};

class Menu {
public:
  Menu( char* _label ){
    label = strdup( _label );
    _w = _h = 0;
    item_slot.set_virtual_link( LIST_REAL );
  }
  ~Menu(){
    free( label );
  }
  List<Menuitem> item_slot;
  char* label;
  int _x, _y, _w, _h;
};

class MenubarWidget: public Widget{
  List<Menu> menu_slot;
  void menu_proc( Menu* menu, XEvent* prev_event );
  void mark_item( Window win, Menu* menu, int item_no );
  void unmark_item( Window win, Menu* menu, int item_no );
public:
  MenubarWidget( Widget* p, char* n );
  void event_proc( XEvent* event );
  int append_menu( char* label );
  void append_menuitem( int menu, Menuitem* item );
};

#endif /* __menubar_h */
