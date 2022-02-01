//
// icon.h:
//
#ifndef __icon_h
#define __icon_h

class IconWidget: public Widget{
  Pixmap pixmap;
  void draw();
  Boolean xpm_f;
public:
  IconWidget( Widget* parent, char* name );
  IconWidget( IconWidget* icon );
  ~IconWidget();
  void set_bitmap( char* data, int data_w, int data_h );
  void set_pixmap( char** data );
  void event_proc( XEvent* event );
};

#endif /* __icon_h */
