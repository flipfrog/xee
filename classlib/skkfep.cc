//
// skkfep.cc: ＳＫＫサーバを用いたフロントエンド機能を提供する。
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <local_types.h>
#include <list.h>
#include <xfontpair.h>
#include <skkserv.h>
#include <skkfep.h>

// ローマ字→仮名、変換テーブル。

struct Henkan_ix {
  char* alph;
  char* hira;
  char* kata;
};

static Henkan_ix romkan_a[] = {
  { "", "あ", "ア" }, { "b", "ば", "バ" }, { "by", "びゃ", "ビャ" },
  { "ch", "ちゃ", "チャ" }, { "cy", "ちゃ", "チャ" }, { "d", "だ", "ダ" },
  { "dh", "でゃ", "デャ" }, { "dy", "ぢゃ", "ヂャ" }, { "f", "ふぁ", "ファ" },
  { "fy", "ふゃ", "フャ" }, { "g", "が", "ガ" }, { "gy", "ぎゃ", "ギャ" },
  { "h", "は", "ハ" }, { "hy", "ひゃ", "ヒャ" }, { "j", "じゃ", "ジャ" },
  { "jy", "じゃ", "ジャ" }, { "k", "か", "カ" }, { "ky", "きゃ", "キャ" },
  { "m", "ま", "マ" }, { "my", "みゃ", "ミャ" }, { "n", "な", "ナ" },
  { "ny", "にゃ", "ニャ" }, { "p", "ぱ", "パ" }, { "py", "ぴゃ", "ピャ" },
  { "r", "ら", "ラ" }, { "ry", "りゃ", "リャ" }, { "s", "さ", "サ" },
  { "sh", "しゃ", "シャ" }, { "sy", "しゃ", "シャ" }, { "t", "た", "タ" },
  { "th", "てぁ", "テァ" }, { "ty", "ちゃ", "チャ" },
  { "v", "う゛ぁ", "ヴァ" }, { "w", "わ", "ワ" }, { "x", "ぁ", "ァ" },
  { "xk", "か", "ヵ" }, { "xw", "ゎ", "ヮ" }, { "xy", "ゃ", "ャ" },
  { "y", "や", "ヤ" }, { "z", "ざ", "ザ" }, { "zy", "じゃ", "ジャ" }
};

static Henkan_ix romkan_i[] = {
  { "", "い", "イ" }, { "b", "び", "ビ" }, { "by", "びぃ", "ビィ" },
  { "ch", "ち", "チ" }, { "cy", "ちぃ", "チィ" }, { "d", "ぢ", "ヂ" },
  { "dh", "でぃ", "ディ" }, { "dy", "ぢぃ", "ヂィ" }, { "f", "ふぃ", "フィ" },
  { "fy", "ふぃ", "フィ" }, { "g", "ぎ", "ギ" }, { "gy", "ぎぃ", "ギィ" },
  { "h", "ひ", "ヒ" }, { "hy", "ひぃ", "ヒィ" }, { "j", "じ", "ジ" },
  { "jy", "じぃ", "ジィ" }, { "k", "き", "キ" }, { "ky", "きぃ", "キィ" },
  { "m", "み", "ミ" }, { "my", "みぃ", "ミィ" }, { "n", "に", "ニ" },
  { "ny", "にぃ", "ニィ" }, { "p", "ぴ", "ピ" }, { "py", "ぴぃ", "ピィ" },
  { "r", "り", "リ" }, { "ry", "りぃ", "リィ" }, { "s", "し", "シ" },
  { "sh", "し", "シ" }, { "sy", "しぃ", "シィ" }, { "t", "ち", "チ" },
  { "th", "てぃ", "ティ" }, { "ty", "ちぃ", "チィ" },
  { "v", "う゛ぃ", "ヴィ" }, { "w", "うぃ", "ウィ" }, { "x", "ぃ", "ィ" },
  { "xw", "ゐ", "ヰ" }, { "z", "じ", "ジ" }, { "zy", "じぃ", "ジィ" }
};

static Henkan_ix romkan_u[] = {
  { "", "う", "ウ" }, { "b", "ぶ", "ブ" }, { "by", "びゅ", "ビュ" },
  { "ch", "ちゅ", "チュ" }, { "cy", "ちゅ", "チュ" }, { "d", "づ", "ヅ" },
  { "dh", "でゅ", "デュ" }, { "dy", "ぢゅ", "ヂュ" }, { "f", "ふ", "フ" },
  { "fy", "ふゅ", "フュ" }, { "g", "ぐ", "グ" }, { "gy", "ぎゅ", "ギュ" },
  { "h", "ふ", "フ" }, { "hy", "ひゅ", "ヒュ" }, { "j", "じゅ", "ジュ" },
  { "jy", "じゅ", "ジュ" }, { "k", "く", "ク" }, { "ky", "きゅ", "キュ" },
  { "m", "む", "ム" }, { "my", "みゅ", "ミュ" }, { "n", "ぬ", "ヌ" },
  { "ny", "にゅ", "ニュ" }, { "p", "ぷ", "プ" }, { "py", "ぴゅ", "ピュ" },
  { "r", "る", "ル" }, { "ry", "りゅ", "リュ" }, { "s", "す", "ス" },
  { "sh", "しゅ", "シュ" }, { "sy", "しゅ", "シュ" }, { "t", "つ", "ツ" },
  { "th", "てゅ", "テュ" }, { "ts", "つ", "ツ" }, { "ty", "ちゅ", "チュ" },
  { "v", "う゛", "ヴ" }, { "w", "う", "ウ" }, { "x", "ぅ", "ゥ" },
  { "xt", "っ", "ッ" }, { "xts", "っ", "ッ" }, { "xy", "ゅ", "ュ" },
  { "y", "ゆ", "ユ" }, { "z", "ず", "ズ" }, { "zy", "じゅ", "ジュ" }
};

static Henkan_ix romkan_e[] = {
  { "", "え", "エ" }, { "b", "べ", "ベ" }, { "by", "びぇ", "ビェ" },
  { "ch", "ちぇ", "チェ" }, { "cy", "ちぇ", "チェ" }, { "d", "で", "デ" },
  { "dh", "でぇ", "デェ" }, { "dy", "ぢぇ", "ヂェ" }, { "f", "ふぇ", "フェ" },
  { "fy", "ふぇ", "フェ" }, { "g", "げ", "ゲ" }, { "gy", "ぎぇ", "ギェ" },
  { "h", "へ", "ヘ" }, { "hy", "ひぇ", "ヒェ" }, { "j", "じぇ", "ジェ" },
  { "jy", "じぇ", "ジェ" }, { "k", "け", "ケ" }, { "ky", "きぇ", "キェ" },
  { "m", "め", "メ" }, { "my", "みぇ", "ミェ" }, { "n", "ね", "ネ" },
  { "ny", "にぇ", "ニェ" }, { "p", "ぺ", "ペ" }, { "py", "ぴぇ", "ピェ" },
  { "r", "れ", "レ" }, { "ry", "りぇ", "リェ" }, { "s", "せ", "セ" },
  { "sh", "しぇ", "シェ" }, { "sy", "しぇ", "シェ" }, { "t", "て", "テ" },
  { "th", "てぇ", "テェ" }, { "ty", "ちぇ", "チェ" },
  { "v", "う゛ぇ", "ヴェ" }, { "w", "うぇ", "ウェ" }, { "x", "ぇ", "ェ" },
  { "xk", "け", "ヶ" }, { "xw", "ゑ", "ヱ" },  { "y", "いぇ", "イェ" },
  { "z", "ぜ", "ゼ" }, { "zy", "じぇ", "ジェ" }
};

static Henkan_ix romkan_o[] = {
  { "", "お", "オ" }, { "b", "ぼ", "ボ" }, { "by", "びょ", "ビョ" },
  { "ch", "ちょ", "チョ" }, { "cy", "ちょ", "チョ" }, { "d", "ど", "ド" },
  { "dh", "でょ", "デョ" }, { "dy", "ぢょ", "ヂョ" }, { "f", "ふぉ", "フォ" },
  { "fy", "ふょ", "フョ" }, { "g", "ご", "ゴ" }, { "gy", "ぎょ", "ギョ" },
  { "h", "ほ", "ホ" }, { "hy", "ひょ", "ヒョ" }, { "j", "じょ", "ジョ" },
  { "jy", "じょ", "ジョ" }, { "k", "こ", "コ" }, { "ky", "きょ", "キョ" },
  { "m", "も", "モ" }, { "my", "みょ", "ミョ" }, { "n", "の", "ノ" },
  { "ny", "にょ", "ニョ" }, { "p", "ぽ", "ポ" }, { "py", "ぴょ", "ピョ" },
  { "r", "ろ", "ロ" }, { "ry", "りょ", "リョ" }, { "s", "そ", "ソ" },
  { "sh", "しょ", "ショ" }, { "sy", "しょ", "ショ" }, { "t", "と", "ト" },
  { "th", "てょ", "テョ" }, { "ty", "ちょ", "チョ" },
  { "v", "う゛ぉ", "ヴォ" }, { "w", "を", "ヲ" }, { "x", "ぉ", "ォ" },
  { "xy", "ょ", "ョ" }, { "y", "よ", "ヨ" }, { "z", "ぞ", "ゾ" },
  { "zy", "じょ", "ジョ" }
};

// ３文字で変換可能なローマ字のリスト。
static char* prefix_list[] = {
  "by", "ch", "cy", "dh", "dy", "fy", "gy", "hy", "jy", "ky", "my", "py",
  "ry", "sh", "sy", "th", "ts", "ty", "xk", "xt", "xts", "xw", "xy", "zy",
  "ny" };

// 半角→全角、変換テーブル。

static char* zenkaku_tbl[] = {
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  " ", "！", "”", "＃", "＄", "％", "＆", "’",
  "（", "）", "＊", "＋", "，", "−", "．", "／",
  "０", "１", "２", "３", "４", "５", "６", "７",
  "８", "９", "：", "；", "＜", "＝", "＞", "？",
  "＠", "Ａ", "Ｂ", "Ｃ", "Ｄ", "Ｅ", "Ｆ", "Ｇ",
  "Ｈ", "Ｉ", "Ｊ", "Ｋ", "Ｌ", "Ｍ", "Ｎ", "Ｏ",
  "Ｐ", "Ｑ", "Ｒ", "Ｓ", "Ｔ", "Ｕ", "Ｖ", "Ｗ",
  "Ｘ", "Ｙ", "Ｚ", "［", "＼", "］", "＾", "＿",
  "‘", "ａ", "ｂ", "ｃ", "ｄ", "ｅ", "ｆ", "ｇ",
  "ｈ", "ｉ", "ｊ", "ｋ", "ｌ", "ｍ", "ｎ", "ｏ",
  "ｐ", "ｑ", "ｒ", "ｓ", "ｔ", "ｕ", "ｖ", "ｗ",
  "ｘ", "ｙ", "ｚ", "｛", "｜", "｝", "〜", ""
  };

// 仮名入力モードで、出力する記号の変換テーブル。

static char* jmode_tbl[] = {
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  " ", "！", "\"", "#", "$", "%", "&", "'",
  "(", ")", "*", "+", "、", "ー", "。", "",
  "0", "1", "2", "3", "4", "5", "6", "7",
  "8", "9", "：", "；", "<", "=", ">", "？",
  "@", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "「", "\\", "」", "^", "_",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "",
  "", "", "", "{", "|", "}", "~", ""
  };

static struct {
  char* in;
  char* out;
} opt_convert_table[] = {
  { "z1", "○" }, { "z2", "▽" }, { "z3", "△" }, { "z4", "□" },
  { "z5", "◇" }, { "z6", "☆" }, { "z7", "◎" }, { "z8", "¢" },
  { "z9", "♂" }, { "z0", "♀" }, { "z!", "●" }, { "z@", "▼" },
  { "z#", "▲" }, { "z$", "■" }, { "z%", "◆" }, { "z^", "★" },
  { "z&", "£" }, { "z*", "×" }, { "z(", "【" }, { "z)", "】" },
  { "z\\", "＼" }, { "z-", "〜" }, { "z=", "≠" }, { "z]", "』" },
  { "z[", "『" }, { "z;", "゛" }, { "z'", "‘" }, { "z`", "´" },
  { "z,", "‥" }, { "z.", "…" }, { "z/", "・" }, { "z|", "‖" },
  { "z_", "∴" }, { "z+", "±" }, { "z{", "〔" }, { "z}", "〕" },
  { "z:", "゜" }, { "z\"", "“" }, { "z~", "¨" }, { "z<", "≦" },
  { "z>", "≧" }, { "z?", "∞" }, { "zb", "°" }, { "zc", "〇" },
  { "zd", "ゝ" }, { "zf", "〃" }, { "zg", "‐" }, { "zh", "←" },
  { "zj", "↓" }, { "zk", "↑" }, { "zl", "→" }, { "zm", "″" },
  { "zn", "′" }, { "zp", "〒" }, { "zq", "《" }, { "zr", "々" },
  { "zs", "ヽ" }, { "zt", "〆" }, { "zv", "※" }, { "zw", "》" },
  { "zx", ":-" }, { "zB", "←" }, { "zC", "℃" }, { "zD", "ゞ" },
  { "zF", "→" }, { "zG", "—" }, { "zM", "〓" }, { "zN", "↓" },
  { "zP", "↑" }, { "zQ", "〈" }, { "zR", "仝" }, { "zS", "ヾ" },
  { "zT", "§" }, { "zV", "÷" }, { "zW", "〉" }, { "zX", ":-)" },
};

// 半角コード→全角文字列ポインタ変換マクロ。
#define han2zen( alph ) zenkaku_tbl[ (alph) ]
// 母音を調べるマクロ。
#define isbon( x ) ((x)=='a'||(x)=='i'||(x)=='u'||(x)=='e'||(x)=='o')

// 静的定義関数
static Boolean romkan_convert( char* rom, char* hira, char* kata );
static Boolean check_prefix( char* s );
//static int check_last_size( char* s );
static int check_del_size( char* s, int c );

//
// SkkFep(): コンストラクタ
//
SkkFep::SkkFep( XFontPair* _xfp, SkkServ* _skkserv )
{
  xfp = _xfp;
  skkserv = _skkserv;
  kana[ 0 ] = 0;
  rom[0] = 0;
  cursor_pos = 0;

  draw_function = GXcopy;

  imode = Hankaku;
  last_kana_mode = Hirakana;
  henkan_mode = 0;
  okuri_mode = False;
  alph_mode = False;
}

//
// set_buffer():
//
void SkkFep::set_buffer( char* p, int l )
{
  buf = p, buf_len = l;
  cursor_pos = strlen(buf);
}

//
// event_proc(): キーストロークを処理する。
//
Boolean SkkFep::event_proc( GC gc, XEvent* event, int x, int y )
{
  int upper_case;

  // キー入力以外のイベントは無視する。
  if( event->type != KeyPress )
    return False;

  Display* dpy = event->xany.display;
  Window win = event->xany.window;

  // キーコードを文字列に変換する。
  char tmp[2+1];
  int cnt = XLookupString( (XKeyEvent*)event, tmp, sizeof(tmp)-1, 0, 0 );
  tmp[cnt] = 0;
  KeySym keysym = XKeycodeToKeysym( dpy, event->xkey.keycode, 0 );
  switch( XKeycodeToKeysym( dpy, event->xkey.keycode, 0 ) ){
  case XK_Delete:
    tmp[0] = 8;
    break;
  case XK_Left:
    backword_cursor( dpy, gc, win, x, y );
    break;
  case XK_Right:
    forword_cursor( dpy, gc, win, x, y );
    break;
  }
  tmp[1] = 0;

  if( strlen(tmp) == 0 )
    return False;

  if( isupper( tmp[0] ) ){
    upper_case = tmp[0];
    tmp[0] = tolower( tmp[0] );
  }else{
    upper_case = 0;
  }

  // MSB がオンのときは無視する(修飾キーのプレスだけ)。
  if( (unsigned char)tmp[0] & 0x80 )
    return False;

  if( henkan_mode == 0 ){
    switch( tmp[0] ){
    case 1: // C-a
      erase_cursor( dpy, gc, win, x, y );
      cursor_pos = 0;
      draw_cursor( dpy, gc, win, x, y );
      break;
    case 2: // C-b
      backword_cursor( dpy, gc, win, x, y );
      break;
    case 4: // C-d
      int tmp_buf_len = strlen(buf);
      if( cursor_pos < tmp_buf_len && tmp_buf_len > 0 ){
	forword_cursor( dpy, gc, win, x, y );
	erase_cursor( dpy, gc, win, x, y );
	erase_str( dpy, gc, win, check_del_size(buf,cursor_pos), x, y );
	draw_cursor( dpy, gc, win, x, y );
      }
      break;
    case 5: // C-e
      erase_cursor( dpy, gc, win, x, y );
      cursor_pos = strlen(buf);
      draw_cursor( dpy, gc, win, x, y );
      break;
    case 6: // C-f
      forword_cursor( dpy, gc, win, x, y );
      break;
    case 11: // C-k
      tmp_buf_len = strlen(buf);
      if( cursor_pos < tmp_buf_len && tmp_buf_len > 0 ){
	erase_cursor( dpy, gc, win, x, y );
	int del_len = tmp_buf_len - cursor_pos;
	cursor_pos = strlen( buf );
	erase_str( dpy, gc, win, del_len, x, y );
	draw_cursor( dpy, gc, win, x, y );
      }
      break;
    }
  }

  // 利用しないコントロールコードは無視する。
  if( !isprint( tmp[0] ) ){
    int match_f = 0;
    static char echars[] = { 7, 8, 9, 10, 13 };
    for( int i = 0 ; i < sizeof(echars)/sizeof(echars[0]) ; i++ )
      if( tmp[0] == echars[i] )
	match_f = 1;
    if( !match_f )
      return False;
  }

  // 全角ローマ字、半角ローマ字モードのときに ^J をタイプされたら仮名モードに。
  if( tmp[0] == 0x0a && ( imode == Zenkaku || imode == Hankaku ) ){
    imode = last_kana_mode;
    return False;
  }

  switch( henkan_mode ){
  case 0: // 通常入力モード ***************************************************
    if( tmp[0] == 8 ){
      if( strlen(buf) > 0 )
	erase_str( dpy, gc, win, check_del_size(buf,cursor_pos), x, y );
      return False;
    }
    if( tmp[0] == 0x0d )
      return True;
    if( strlen(buf) >= buf_len-1 ){
      XBell( dpy, 0 );
      return False;
    }
    switch( imode ){
    case Hirakana:
    case Katakana:
      switch( tmp[0] ){
      case 9:
      case 10:
	return False;
      case 7:
	erase_str( dpy, gc, win, strlen(rom), x, y );
	rom[0] = 0;
	XBell( dpy, 0 );
	return False;
	break;
      case 'l':
	if( rom[0] != 'z' ){
	  last_kana_mode = imode;
	  imode = upper_case?Zenkaku:Hankaku;
	  erase_str( dpy, gc, win, strlen(rom), x, y );
	  if( tmp[0] == 'n' )
	    insert_str( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	  return False;
	}
	break;
      case 'q':
	if( rom[0] != 'z' ){
	  imode = (imode==Hirakana)?Katakana:Hirakana;
	  erase_str( dpy, gc, win, strlen(rom), x, y );
	  if( strcmp( rom, "n" ) == 0 )
	    insert_str( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	  rom[0] = 0;
	  return False;
	}
	break;
      }

      // opt_convert_table を参照して、z*記号の変換を行う。
      char tmp_rom[16];
      strcpy( tmp_rom, rom );
      strcat( tmp_rom, tmp );
      for( int i = 0 ; i < Number(opt_convert_table) ; i++ ){
	if( strcmp( tmp_rom, opt_convert_table[i].in ) == 0 ){
	  erase_str( dpy, gc, win, strlen(rom), x, y );
	  insert_str( dpy, gc, win, opt_convert_table[i].out, x, y );
	  rom[0] = 0;
	  return False;
	}
      }

      // 句読点等の記号を調べる。
      if( strlen( jmode_tbl[ tmp[0] ] ) > 0 ){
	erase_str( dpy, gc, win, strlen(rom), x, y );
	if( strcmp( rom, "n" ) == 0 )
	  insert_str( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	insert_str( dpy, gc, win, jmode_tbl[ tmp[0] ], x, y );
	rom[0] = 0;
	return False;
      }

      // ひらがな確定モード時に、大文字が入力されると、▽モードに切り変える。
      if( upper_case ){
	henkan_mode = 1;
	erase_str( dpy, gc, win, strlen(rom), x, y );
	insert_str( dpy, gc, win, "▽", x, y );
	insert_str( dpy, gc, win, rom, x, y );
      }else if( tmp[0] == '/'  ){
	// 英数字変換モード
	henkan_mode = 1;
	alph_mode = True;
	rom[0] = 0;
	insert_str( dpy, gc, win, "▽", x, y );
	return False;
      }

      // 途中入力のローマ字を出力する。
      insert_str( dpy, gc, win, tmp, x, y );

      // 静的バッファに入力されたローマ字を格納する。
      strcat( rom, tmp );

      if( strlen(rom) > 0 && isbon( rom[ strlen(rom)-1 ] ) ){
	char hira[8], kata[8];
	erase_str( dpy, gc, win, strlen(rom), x, y );
	if( romkan_convert( rom, hira, kata ) ){
	  if( henkan_mode == 0 )
	    insert_str( dpy, gc, win, (imode==Hirakana)?hira:kata, x, y );
	  else
	    kana_out( dpy, gc, win, (imode==Hirakana)?hira:kata, x, y );
	}
	rom[0] = 0;
	return False;
      }

      if( strcmp( rom, "nn" ) == 0 || strcmp( rom, "n'" ) == 0 ){
	erase_str( dpy, gc, win, 2, x, y );
	if( henkan_mode == 0 )
	  insert_str( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	else
	  kana_out( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	rom[0] = 0;
	return False;
      }
      if( strlen(rom) == 2 && rom[0] == 'n' && rom[1] != 'y' ){
	memcpy( &rom[0], &rom[1], strlen(rom)-1 );
	erase_str( dpy, gc, win, strlen(rom), x, y );
	rom[ strlen(rom)-1 ] = 0;
	if( henkan_mode == 0 )
	  insert_str( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	else
	  kana_out( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	insert_str( dpy, gc, win, rom, x, y );
	return False;
      }
      if( strlen(rom) == 2 && rom[0] == rom[1] ){
	memcpy( &rom[0], &rom[1], strlen(rom)-1 );
	erase_str( dpy, gc, win, strlen(rom), x, y );
	rom[ 1 ] = 0;
	if( henkan_mode == 0 )
	  insert_str( dpy, gc, win, (imode==Hirakana)?"っ":"ッ", x, y );
	else
	  kana_out( dpy, gc, win, (imode==Hirakana)?"っ":"ッ", x, y );
	insert_str( dpy, gc, win, rom, x, y );
	return False;
      }
      if( strlen(rom) >= 2 ){
	if( !check_prefix( rom ) ){
	  erase_str( dpy, gc, win, strlen(rom), x, y );
	  rom[0] = 0;
	}
	return False;
      }
      break;
    case Zenkaku: // 全角ローマ字モードのときは、直接テーブルを使って変換する。
    case Hankaku: // 半角ローマ字モードのときは、そのまま出力する。
      switch( tmp[0] ){
      case 8: // バックスペース
	if( strlen(buf) > 0 )
	  erase_str( dpy, gc, win, check_del_size(buf,cursor_pos), x, y );
	break;
      default: // その他
	if( upper_case )
	  tmp[0] = toupper( tmp[0] );
	insert_str( dpy, gc, win, imode==Zenkaku?zenkaku_tbl[ tmp[0] ]:tmp, x, y );
	break;
      }
      return False;
      break;
    }
    break;
  case 1: // ▽モード *********************************************************
    switch( tmp[0] ){
    case 9:
      if( strlen(kana) > 0 ){
	char out[1024];
	if( skkserv->complesion_word( kana, out ) ){
	  erase_str( dpy, gc, win, strlen(rom), x, y );
	  rom[0] = 0;
	  erase_str( dpy, gc, win, strlen(kana), x, y );
	  kana[0] = 0;
	  kana_out( dpy, gc, win, out, x, y );
	}
      }else{
	XBell( dpy, 0 );
      }
      return False;
      break;
    case 8:
      if( strlen(rom) > 0 ){
	erase_str( dpy, gc, win, 1, x, y );
	rom[ strlen(rom)-1 ] = 0;
      }else if( strlen(kana) > 0 ){
	erase_str( dpy, gc, win, check_del_size(kana,strlen(kana)), x, y );
	kana[ strlen(kana)-check_del_size(kana,strlen(kana)) ] = 0;
      }
      return False;
      break;
    case ' ':
      if( strlen( rom ) > 0 ){
	if( strcmp( rom, "n" ) == 0 ){
	  erase_str( dpy, gc, win, strlen(rom), x, y );
	  kana_out( dpy, gc, win, imode==Hirakana?"ん":"ン", x, y );
	}
	rom[0] = 0;
      }
      word_slot = new List<char>;
      word_slot->set_virtual_link( LIST_REAL );
      skkserv->convert_word( kana, word_slot );
      if( word_slot->count() > 0 ){
	erase_str( dpy, gc, win, strlen(kana)+2, x, y );
	insert_str( dpy, gc, win, "▼", x, y );
	insert_str( dpy, gc, win, word_slot->get(0), x, y );
	current_ref = 0;
	henkan_mode++;
      }else{
	XBell( dpy, 0 );
      }
      return False;
      break;
    case 0x0a:
      return False;
    case 0x0d:
      erase_str( dpy, gc, win, strlen(kana)+2, x, y );
      insert_str( dpy, gc, win, kana, x, y );
      alph_mode = False;
      henkan_mode = 0;
      kana[0] = 0;
      return False;
      break;
    case 0x07:
      int erase_cnt = strlen(kana)+strlen(rom)+2;
      if( okuri_mode )
	erase_cnt += strlen(okuri)+1;
      erase_str( dpy, gc, win, erase_cnt, x, y );
      okuri_mode = 0;
      rom[0] = 0;
      alph_mode = False;
      henkan_mode = 0;
      kana[0] = 0;
      XBell( dpy, 0 );
      return False;
      break;
    case 'q':
      erase_str( dpy, gc, win, strlen(rom), x, y );
      if( strcmp( rom, "n" ) == 0 )
	kana_out( dpy, gc, win, imode==Hirakana?"ん":"ン", x, y );
      rom[0] = 0;
      imode = imode==Hirakana?Katakana:Hirakana;
      return False;
      break;
    }
    if( !alph_mode && !okuri_mode &&
       strlen( jmode_tbl[tmp[0]] ) == 0 && upper_case ){
      // 入力された英数字が大文字で、送りがな変換モードでなく、
      // 全角記号に変換不可能だった場合、送りがな変換モードにする。
      // このとき、rom[]=='n' のときは、'ん' を出力してから送りがな変換モード
      // の '*' を出力する。
      // また、tmp[0] が母音だったときは確定したかなを'*'の後に出力する。
      if( strcmp( rom, "n" ) == 0 ){
	erase_str( dpy, gc, win, strlen(rom), x, y );
	kana_out( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	rom[0] = 0;
      }
      if( strlen(rom) > 0 ){
	return False;
      }
      insert_str( dpy, gc, win, "*", x, y );
      okuri_mode = True;
      okuri_char = tmp[0];
      if( isbon( tmp[0] ) ){
	char hira[8],kata[8];
	romkan_convert( tmp, hira, kata );
	  kana_out( dpy, gc, win, (imode==Hirakana)?hira:kata, x, y );
	rom[0] = 0;
      }else{
	insert_str( dpy, gc, win, tmp, x, y );
	strcpy( rom, tmp );
      }
    }else{
      // アルファベット変換モードでなく、記号文字が入力された場合は、rom[]
      // の内容を消して、全角記号を出力する。ただし、rom[]=="n" だったとき
      // は、'ん' を出力してから全角記号を出力する。
      if( !alph_mode && strlen( jmode_tbl[ tmp[0] ] ) > 0 ){
	erase_str( dpy, gc, win, strlen(rom), x, y );
	if( rom[0] == 'n' ){
	  kana_out( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	}
	kana_out( dpy, gc, win, jmode_tbl[ tmp[0] ], x, y );
	rom[0] = 0;
	return False;
      }

      if( alph_mode ){
	kana_out( dpy, gc, win, tmp, x, y );
	return False;
      }
      insert_str( dpy, gc, win, tmp, x, y );
      strcat( rom, tmp );

      // 入力された英数字が母音だった場合、表示された rom[] の内容を削除
      // し、かなに変換する。
      if( isbon( rom[ strlen(rom)-1 ] ) ){
	char hira[8], kata[8];
	erase_str( dpy, gc, win, strlen(rom), x, y );
	if( romkan_convert( rom, hira, kata ) )
	  kana_out( dpy, gc, win, (imode==Hirakana)?hira:kata, x, y );
	rom[0] = 0;
	return False;
      }

      // 入力の結果、rom[]=="nn"|"n'" のときは、'ん' を出力する。
      if( strcmp( rom, "nn" ) == 0 || strcmp( rom, "n'" ) == 0 ){
	erase_str( dpy, gc, win, strlen(rom), x, y );
	kana_out( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	rom[0] = 0;
	return False;
      }

      // 入力の結果、rom[]=="n?" && rom[]<>"?y" の場合、
      // 'ん' を出力して、その次の文字を rom[0] に代入する。
      if( strlen(rom) == 2 && rom[0] == 'n' && rom[1] != 'y' ){
	erase_str( dpy, gc, win, strlen(rom), x, y );
	rom[0] = rom[1];
	rom[1] = 0;
	kana_out( dpy, gc, win, (imode==Hirakana)?"ん":"ン", x, y );
	insert_str( dpy, gc, win, rom, x, y );
	return False;
      }

      // 入力の結果 rom[0] == rom[1] だった場合、促音の処理を行う。
      if( strlen(rom) == 2 && rom[0] == rom[1] ){
	erase_str( dpy, gc, win, strlen(rom), x, y );
	rom[0] = rom[1];
	rom[1] = 0;
	kana_out( dpy, gc, win, (imode==Hirakana)?"っ":"ッ", x, y );
	insert_str( dpy, gc, win, rom, x, y );
	return False;
      }

      // 上記のチェックに掛らなかった、rom[] の内容をテーブルを参照して
      // 正当な英数字文字列かチェックする。
      if( strlen(rom) >= 2 && !check_prefix( rom ) ){
	erase_str( dpy, gc, win, strlen(rom), x, y );
	rom[0] = 0;
	return False;
      }
    }
    break;
  case 2: // ▼モード *********************************************************
    switch( tmp[0] ){
    case 9:
      return False;
    case ' ':
      if( current_ref+1 < word_slot->count() ){
	char* p = word_slot->get(current_ref);
	erase_str( dpy, gc, win, strlen(p), x, y );
	if( okuri_mode )
	  erase_str( dpy, gc, win, strlen(okuri), x, y );
	insert_str( dpy, gc, win, word_slot->get(++current_ref), x, y );
	if( okuri_mode )
	  insert_str( dpy, gc, win, okuri, x, y );
      }
      break;
    case 'x':
      if( current_ref-1 >= 0 ){
	char* p = word_slot->get(current_ref);
	erase_str( dpy, gc, win, strlen(p), x, y );
	if( okuri_mode )
	  erase_str( dpy, gc, win, strlen(okuri), x, y );
	insert_str( dpy, gc, win, word_slot->get(--current_ref), x, y );
	if( okuri_mode )
	  insert_str( dpy, gc, win, okuri, x, y );
      }
      break;
    case 7:
      erase_str( dpy, gc, win, strlen(word_slot->get(current_ref))+2, x, y );
      if( okuri_mode )
	erase_str( dpy, gc, win, strlen( okuri ), x, y );
      insert_str( dpy, gc, win, "▽", x, y );
      if( okuri_mode ){
	kana[ strlen(kana)-1 ] = 0;
	strcat( kana, okuri );
	okuri_mode = False;
      }
      insert_str( dpy, gc, win, kana, x, y );
      henkan_mode = 1;
      delete word_slot;
      break;
    case 0x0d:
      char* p = word_slot->get(current_ref);
      erase_str( dpy, gc, win, strlen(p)+2, x, y );
      if( okuri_mode )
	erase_str( dpy, gc, win, strlen( okuri ), x, y );
      insert_str( dpy, gc, win, p, x, y );
      if( okuri_mode )
	insert_str( dpy, gc, win, okuri, x, y );
      henkan_mode = 0;
      kana[0] = 0;
      alph_mode = False;
      okuri_mode = False;
      delete word_slot;
      break;
    default:
      if( isprint( tmp[0] ) ){
	erase_str( dpy, gc, win, strlen(word_slot->get(current_ref))+2, x, y );
	if( okuri_mode )
	  erase_str( dpy, gc, win, strlen( okuri ), x, y );
	insert_str( dpy, gc, win, word_slot->get(current_ref), x, y );
	if( okuri_mode )
	  insert_str( dpy, gc, win, okuri, x, y );
	henkan_mode = 0;
	kana[0] = 0;
	alph_mode = False;
	okuri_mode = False;
	delete word_slot;

	strcpy( rom, tmp );
	if( tmp[0] != 'l' && upper_case ){
	  henkan_mode = 1;
	  insert_str( dpy, gc, win, "▽", x, y );
	  if( isbon( tmp[0] ) ){
	    char hira[8], kata[8];
	    romkan_convert( tmp, hira, kata );
	    kana_out( dpy, gc, win, imode==Hirakana?hira:kata, x, y );
	    rom[0] = 0;
	  }else{
	    insert_str( dpy, gc, win, rom, x, y );
	  }
	}else if( tmp[0] == '/'  ){
	  // 英数字変換モード
	  henkan_mode = 1;
	  alph_mode = True;
	  rom[0] = 0;
	  insert_str( dpy, gc, win, "▽", x, y );
	}else if( tmp[0] == 'l' && !upper_case ){
	  imode = Hankaku;
	  henkan_mode = 0;
	  alph_mode = False;
	  rom[0] = 0;
	}else if( tmp[0] == 'l' && upper_case ){
	  imode = Zenkaku;
	  henkan_mode = 0;
	  alph_mode = False;
	  rom[0] = 0;
	}else if( tmp[0] == 'q' ){
	  imode = imode==Katakana?Hirakana:Katakana;
	  henkan_mode = 0;
	  alph_mode = False;
	  rom[0] = 0;
	}else{
	  if( isbon( tmp[0] ) ){
	    char hira[8], kata[8];
	    romkan_convert( tmp, hira, kata );
	    kana_out( dpy, gc, win, imode==Hirakana?hira:kata, x, y );
	    rom[0] = 0;
	  }else if( strlen( jmode_tbl[ tmp[0] ] ) > 0 ){
	    insert_str( dpy, gc, win, jmode_tbl[ tmp[0] ], x, y );
	    rom[0] = 0;
	  }else{
	    insert_str( dpy, gc, win, rom, x, y );
	  }
	}
      }else{
	rom[0] = 0;
      }
      break;
    }
    return False;
    break;
  }
  return False;
}

//
// romkan_convert(): 文字列を元にローマ字仮名変換を行う。
//
static Boolean romkan_convert( char* rom, char* hira, char* kata )
{
  // rom パラメータで受け取るローマ字は、それ自身で音を表現するものとする。
  // つまり、子音で始まって母音で終るローマ字である。
  // ここでは、ローマ字列の最後のアルファベットが、あ行の内のいづれかとし、
  // それをもとに、romkan_{a,i,u,e,o} の変換テーブルを使って仮名に変換する。
  // 変換後の結果は、hiraに平仮名を、kataに片仮名を複写する。
  // この変換処理に成功すれば、True を返す。また失敗すれば、False を返す。
  int num_henkan_ix;
  struct Henkan_ix *henkan_ix;
  switch( rom[ strlen(rom)-1 ] ){
  case 'a': // あ段
    henkan_ix = romkan_a;
    num_henkan_ix = Number(romkan_a);
    break;
  case 'i': // い段
    henkan_ix = romkan_i;
    num_henkan_ix = Number(romkan_i);
    break;
  case 'u': // う段
    henkan_ix = romkan_u;
    num_henkan_ix = Number(romkan_u);
    break;
  case 'e': // え段
    henkan_ix = romkan_e;
    num_henkan_ix = Number(romkan_e);
    break;
  case 'o': // お段
    henkan_ix = romkan_o;
    num_henkan_ix = Number(romkan_o);
    break;
  case 'n': // ん は特別な変換規則を用いる。
    strcpy( hira, "ん" );
    strcpy( kata, "ン" );
    return True;
    break;
  default:  // ？？？
    return False;
  }

  int cmp_len = strlen(rom)-1;
  for( int i = 0 ; i < num_henkan_ix ; i++ ){
    struct Henkan_ix *ix = &henkan_ix[i];
    if( strncmp( rom, ix->alph, cmp_len ) == 0 ){
      strcpy( hira, ix->hira );
      strcpy( kata, ix->kata );
      return True;
    }
  }
  return False;
}

//
// check_prefix(): ２文字のローマ字が次の母音の入力で仮名に変換可能かを調べる。
//
static Boolean check_prefix( char* s )
{
  for( int i = 0 ; i < Number(prefix_list) ; i++ )
    if( strcmp( prefix_list[i], s ) == 0 )
      return True;
  return False;
}

//
// kana_out(): 文字を出力する。▽モードのときは、内部バッファに文字を追加する。
//
void SkkFep::kana_out( Display* dpy, GC gc, Window win, char* s, int x, int y )
{
  if( okuri_mode ){
    static char sokuon[3];
    if( strcmp( s, "っ" ) == 0 || strcmp( s, "ッ" ) == 0 ){
      strcpy( sokuon, s );
      insert_str( dpy, gc, win, s, x, y );
      return;
    }else if( strlen( sokuon ) == 0 ){
      sokuon[0] = 0;
    }
    word_slot = new List<char>;
    word_slot->set_virtual_link( LIST_REAL );
    int kana_len = strlen(kana);
    kana[ kana_len+0 ] = okuri_char;
    kana[ kana_len+1 ] = 0;
    skkserv->convert_word( kana, word_slot );
    if( word_slot->count() > 0 ){
      okuri[0] = 0;
      if( strlen(sokuon) > 0 ){
	strcpy( okuri, sokuon );
	erase_str( dpy, gc, win, 2, x, y );
      }
      strcat( okuri, s );
      erase_str( dpy, gc, win, strlen(kana)+2, x, y );
      insert_str( dpy, gc, win, "▼", x, y );
      insert_str( dpy, gc, win, word_slot->get(0), x, y );
      insert_str( dpy, gc, win, okuri, x, y );
      current_ref = 0;
      sokuon[0] = 0;
      henkan_mode++;
    }else{
      delete word_slot;
      kana[ strlen(kana)-1 ] = 0; // 送り仮名のローマ字を消す。
      strcat( kana, s );
      erase_str( dpy, gc, win, 1, x, y );
      insert_str( dpy, gc, win, s, x, y );
      okuri_mode = False;
      XBell( dpy, 0 );
    }
  }else{
    strcat( kana, s );
    insert_str( dpy, gc, win, s, x, y );
  }
}

//
// insert_str(): 文字列をカーソルの直後に挿入する。
//
void SkkFep::insert_str( Display* dpy, GC gc, Window window,
			char* s, int _x, int _y )
{
  int t_width = xfp->text_width( "Z" );
  erase_cursor( dpy, gc, window, _x, _y );
  if( cursor_pos < strlen(buf) ){
    if( draw_function == GXcopy ){
      XSetFunction( dpy, gc, GXcopy );
      XSetForeground( dpy, gc, bg_pixel );
    }else{
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, fg_pixel );
    }
    int x = _x + t_width * cursor_pos;
    xfp->draw_string(window, gc, x, _y, &buf[cursor_pos]);
  }
  if( draw_function == GXcopy ){
    XSetFunction( dpy, gc, GXcopy );
    XSetForeground( dpy, gc, fg_pixel );
  }else{
    XSetFunction( dpy, gc, GXset );
    XSetPlaneMask( dpy, gc, fg_pixel );
  }
  char tmp_buf[ strlen(&buf[cursor_pos])+1 ];
  strcpy( tmp_buf, &buf[cursor_pos] );
  buf[cursor_pos] = 0;
  strcat( buf, s );
  strcat( buf, tmp_buf );
  cursor_pos += strlen( s );
  xfp->draw_string( window, gc, _x, _y, buf );
  draw_cursor( dpy, gc, window, _x, _y );
}

//
// erase_str(): カーソル後の文字列を指定されたバイト数分削除する。
//
void SkkFep::erase_str( Display* dpy, GC gc, Window window,
		       int c, int _x, int _y )
{
  if( c <= 0 || strlen(buf) == 0 )
    return;
  if( c > cursor_pos ){
    printf("SkkFep::erase_str(): warning! '#erase_str > cursor_pos'\n");
    c = cursor_pos;
    abort();
  }

  erase_cursor( dpy, gc, window, _x, _y );

  if( draw_function == GXcopy ){
    XSetFunction( dpy, gc, GXcopy );
    XSetForeground( dpy, gc, bg_pixel );
  }else{
    XSetFunction( dpy, gc, GXclear );
    XSetPlaneMask( dpy, gc, fg_pixel );
  }
  int t_width = xfp->text_width( "Z" );
  int x = t_width * (cursor_pos-c) + _x;
  xfp->draw_string( window, gc, x, _y, &buf[ cursor_pos-c ] );

  int new_len = strlen( buf ) - c;
  int trail_len = strlen( buf ) - cursor_pos;
  memcpy( &buf[ cursor_pos-c ], &buf[ cursor_pos ], trail_len );
  cursor_pos -= c;
  buf[ new_len ] = 0;
  if( draw_function == GXcopy ){
    XSetFunction( dpy, gc, GXcopy );
    XSetForeground( dpy, gc, fg_pixel );
  }else{
    XSetFunction( dpy, gc, GXset );
    XSetPlaneMask( dpy, gc, fg_pixel );
  }
  xfp->draw_string( window, gc, _x, _y, buf );
  draw_cursor( dpy, gc, window, _x, _y );
}

//
// check_del_size():
//
static int check_del_size( char* buf, int cursor_pos )
{
  if( (unsigned char)buf[ cursor_pos-1 ] & 0x80 )
    return 2;
  return 1;
}

//
// draw_cursor():
//
void SkkFep::draw_cursor( Display* dpy, GC gc, Window window, int _x, int _y )
{
  int t_width = xfp->text_width("Z");
  int x = t_width * cursor_pos + _x;
  if( draw_function == GXcopy ){
    XSetFunction( dpy, gc, GXcopy );
    XSetForeground( dpy, gc, fg_pixel );
  }else{
    XSetFunction( dpy, gc, GXset );
    XSetPlaneMask( dpy, gc, fg_pixel );
  }
  XDrawLine( dpy, window, gc, x, _y, x, _y+xfp->height()-1 );
}

//
// erase_cursor():
//
void SkkFep::erase_cursor( Display* dpy, GC gc, Window window, int _x, int _y )
{
  int t_width = xfp->text_width("Z");
  int x = t_width * cursor_pos + _x;
  if( draw_function == GXcopy ){
    XSetFunction( dpy, gc, GXcopy );
    XSetForeground( dpy, gc, bg_pixel );
  }else{
    XSetFunction( dpy, gc, GXclear );
    XSetPlaneMask( dpy, gc, fg_pixel );
  }
  XDrawLine( dpy, window, gc, x, _y, x, _y+xfp->height()-1 );
}

//
// draw():
//
void SkkFep::draw( Display* dpy, GC gc, Window window, int x, int y )
{
  if( draw_function == GXcopy ){
    XSetFunction( dpy, gc, GXcopy );
    XSetForeground( dpy, gc, fg_pixel );
  }else{
    XSetFunction( dpy, gc, draw_function );
    XSetPlaneMask( dpy, gc, fg_pixel );
  }
  xfp->draw_string( window, gc, x, y, buf );
  if( draw_function == GXclear )
    erase_cursor( dpy, gc, window, x, y );
  else
    draw_cursor( dpy, gc, window, x, y );
}

//
// forword_cursor():
//
void SkkFep::forword_cursor( Display* dpy, GC gc, Window win, int x, int y )
{
  if( cursor_pos < strlen(buf) ){
    erase_cursor( dpy, gc, win, x, y );
    cursor_pos++;
    if( (unsigned char)(buf[ cursor_pos-1 ]) & 0x80 )
      cursor_pos++;
    draw_cursor( dpy, gc, win, x, y );
  }
}

//
// backword_cursor():
//
void SkkFep::backword_cursor( Display* dpy, GC gc, Window win, int x, int y )
{
  if( cursor_pos > 0 ){
    erase_cursor( dpy, gc, win, x, y );
    cursor_pos--;
    if( (unsigned char)(buf[ cursor_pos ]) & 0x80 )
      cursor_pos--;
    draw_cursor( dpy, gc, win, x, y );
  }
}
