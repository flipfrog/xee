//
// message.h:
//
#ifndef __message_h
#define __message_h

class MessageWidget: public Widget{
  int column;
  void draw();
public:
  MessageWidget( Widget* p, char* n );
  void event_proc( XEvent* event );
  void set_column( int c );
  void set_label( char* l );
};

#endif // __message_h
