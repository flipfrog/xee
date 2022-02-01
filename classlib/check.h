//
// check.h: チェッククラス
//
#ifndef __check_h
#define __check_h

class CheckWidget: public Widget{
  void draw();
public:
  CheckWidget( Widget* parent, char* name );
  CheckWidget( CheckWidget* check );
  void event_proc( XEvent* event );
  void set_label( char* l );
};

#endif /* __check_h */
