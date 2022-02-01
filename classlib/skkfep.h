//
// skkfep.h: ＳＫＫを用いたフロントエンド機能を提供する。
//
#ifndef __skkfep_h
#define __skkfep_h

// 文字入力モード。
enum input_mode {
  Hankaku,   // 半角ローマ字
  Zenkaku,   // 全角ローマ字
  Hirakana,  // 全角ひら仮名
  Katakana,  // 全角片仮名
};

class SkkFep{
  int draw_function;                 // GXset, GXcopy
  unsigned long fg_pixel, bg_pixel;
  SkkServ* skkserv;
  XFontPair* xfp;
  char* buf;
  int buf_len;
  input_mode imode;
  input_mode last_kana_mode;
  int henkan_mode;           // 0:通常入力／1:▽モード／2:▼モード。
  char kana[1024];           // ▽▼モードの文字バッファ。
  char rom[256];
  char okuri[8];             // 送りがなを格納するバッファ。
  List<char> *word_slot;     // 変換結果を格納するリスト。
  int current_ref;           // 参照中の変換ワードを差す。
  Boolean okuri_mode;
  Boolean alph_mode;
  char okuri_char;
  int cursor_pos;

  void kana_out( Display* dpy, GC gc, Window win, char* s, int x, int y );
  void insert_str( Display* dpy, GC gc, Window win, char* s, int x, int y );
  void erase_str( Display* dpy, GC gc, Window win, int c, int x, int y );
  void draw_cursor( Display* dpy, GC gc, Window win, int x, int y );
  void erase_cursor( Display* dpy, GC gc, Window win, int x, int y );
  void forword_cursor( Display* dpy, GC gc, Window win, int x, int y );
  void backword_cursor( Display* dpy, GC gc, Window win, int x, int y );
public:
  SkkFep( XFontPair* _xfp, SkkServ* _skkserv );
  ~SkkFep(){}
  void set_buffer( char* buf, int buf_len );
  Boolean event_proc( GC gc, XEvent* event, int x, int y );
  void set_draw_function( int function ){
    draw_function = function;
  }
  void set_pixels( unsigned long fg, unsigned long bg ){
    fg_pixel = fg, bg_pixel = bg;
  }
  void draw( Display* dpy, GC gc, Window win, int x, int y );
};

#endif // __skkfep
