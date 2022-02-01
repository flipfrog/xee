//
// text.h:
//
#ifndef __text_h
#define __text_h

class Xcontext;

class TextWidget: public Widget{
  Cursor cursor;
  XFontPair* xfp;
  SkkFep* skkfep;
  char* buf;
  int input_len;
  Boolean inside_pointer;
  void draw();
public:
  TextWidget( SkkServ* skkserv, Widget* p, char* n );
  ~TextWidget();
  void event_proc( XEvent* event );
  void set_skk_host( char* name );
  inline char* get_string(){ return buf; }
  void set_string( char* s );
  void set_label( char* s );
  void set_input_len( int len );
};

#endif // __text_h
