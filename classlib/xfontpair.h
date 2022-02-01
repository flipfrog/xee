//
// fontpair.h: フォントペアクラス。
//
#ifndef __fontpair_h
#define __fontpair_h

class XFontPair{
  Boolean fixed_width_f; // 固定幅フォントのときは、真にする。
  XFontStruct* _hankaku; // 半角フォント。
  XFontStruct* _zenkaku; // 全角フォント。
  Display* _dpy;
public:
  char *_hname, *_zname; // 半角全角それぞれのフォント名。
  XFontPair( Display* dpy, char* hname, char* zname );
  XFontPair( Display* dpy, char* hname );
  ~XFontPair();
  inline XFontStruct* hankaku(){ return _hankaku; }
  inline XFontStruct* zenkaku(){ return _zenkaku; }
  inline void set_fixed_width( Boolean f ){ fixed_width_f = f; }
  inline int height()
    { return _hankaku->max_bounds.ascent+_hankaku->max_bounds.descent; }
  int text_width( char* name );
  void draw_string( Window w, GC gc, int x, int y, char* s );
  void draw_image_string( Window w, GC gc, int x, int y, char* s );
};

#endif // __fontpair_h
