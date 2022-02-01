//
// button.h:
//
#ifndef __button_h
#define __button_h

class ButtonWidget: public Widget{
  Cursor cursor;
  void revers();
  void draw();
public:
  ButtonWidget( Widget* widget, char* name );
  ButtonWidget( ButtonWidget* button );
  ~ButtonWidget();
  void event_proc( XEvent* event );
  void set_shortcut_key( char c );
  void set_label( char* l );
};

#endif /* __button_h */
