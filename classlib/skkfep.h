//
// skkfep.h: �ӣˣˤ��Ѥ����ե��ȥ���ɵ�ǽ���󶡤��롣
//
#ifndef __skkfep_h
#define __skkfep_h

// ʸ�����ϥ⡼�ɡ�
enum input_mode {
  Hankaku,   // Ⱦ�ѥ��޻�
  Zenkaku,   // ���ѥ��޻�
  Hirakana,  // ���ѤҤ鲾̾
  Katakana,  // �����Ҳ�̾
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
  int henkan_mode;           // 0:�̾����ϡ�1:���⡼�ɡ�2:���⡼�ɡ�
  char kana[1024];           // �����⡼�ɤ�ʸ���Хåե���
  char rom[256];
  char okuri[8];             // ���꤬�ʤ��Ǽ����Хåե���
  List<char> *word_slot;     // �Ѵ���̤��Ǽ����ꥹ�ȡ�
  int current_ref;           // ��������Ѵ���ɤ򺹤���
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
