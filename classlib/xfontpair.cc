//
// fontpair.cc: フォントセットクラス。
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include <local_types.h>
#include <xfontpair.h>

#define iskanji(x) ((unsigned char)0x80 & (unsigned char)(x))

//
// XFontPair(): コンストラクタ。
//
XFontPair::XFontPair( Display* dpy, char* hname, char* zname )
{
  _dpy = dpy;
  fixed_width_f = True;
  if( (_hankaku = XLoadQueryFont( dpy, hname )) == NULL )
    fatal();
  if( (_zenkaku = XLoadQueryFont( dpy, zname )) == NULL )
    fatal();
  if( (_hname = (char*)strdup( (const char*)hname )) == NULL )
    fatal();
  if( (_zname = (char*)strdup( (const char*)zname )) == NULL )
    fatal();
}

//
// XFontPair(): コンストラクタ。
//
XFontPair::XFontPair( Display* dpy, char* hname )
{
  _dpy = dpy;
  fixed_width_f = False;
  if( (_hankaku = XLoadQueryFont( dpy, hname )) == NULL )
    fatal();
  _zenkaku = NULL;
  if( (_hname = (char*)strdup( (const char*)hname )) == NULL )
    fatal();
  _zname = NULL;
}

//
// ~XFontPair(): デストラクタ
//
XFontPair::~XFontPair()
{
  if( _hname != NULL ) free( _hname );
  if( _zname != NULL ) free( _zname );
  XFreeFont( _dpy, _hankaku );
  XFreeFont( _dpy, _zenkaku );
}

//
// text_width(): 与えられたテキストの幅をピクセル値で返す。
//
int XFontPair::text_width( char* name )
{
  int p = 0, width = 0;
  // 固定幅フォントのときは、全角と半角キャラクタの数をカウントして後でまとめ
  // て、文字列の幅を計算する。
  if( fixed_width_f ){
    int hcnt = 0, zcnt = 0;
    while( name[p] ){
      if( iskanji(name[p++]) ){
	zcnt++;
	p++;
      }else hcnt++;
    }
    width = hcnt*_hankaku->max_bounds.width + zcnt*_zenkaku->max_bounds.width;
  }else{
    // 可変幅フォントのときは、文字列を始めからサーチして、全角半角それぞれの
    // キャラクタのグループ毎にその幅を計算してゆく。当然こちらの方が遅い。
    while( name[p] ){
      int zen_f = iskanji(name[p]);
      int c_len, c_step = zen_f?2:1;
      for(c_len = 0;zen_f==iskanji(name[p+c_len])&&name[p+c_len];c_len+=c_step)
	;
      char buf[1024];
      memcpy( buf, &name[p], c_len );
      if( !zen_f ) width += XTextWidth( _hankaku, buf, c_len );
      else width += XTextWidth16( _zenkaku, (XChar2b*)buf, c_len/2 );
      p += c_len;
    }
  }
  return width;
}

//
// draw_string(): 文字列を描画する。
//
void XFontPair::draw_string( Window w, GC gc, int x, int y, char* s )
{
  int p = 0;
  y += _hankaku->ascent;
  while( s[p] ){
    int zen_f = iskanji(s[p]);
    int c_len, c_step = zen_f?2:1;
    for(c_len = 0 ; zen_f==iskanji(s[p+c_len]) && s[p+c_len]; c_len += c_step)
      ;
    if( !zen_f ){
      char buf[1024];
      memcpy( buf, &s[p], c_len );
      XSetFont( _dpy, gc, _hankaku->fid );
      XDrawString( _dpy, w, gc, x, y, buf, c_len );
      x += _hankaku->max_bounds.width * c_len;
    }else{
      XChar2b buf[512];
      for( int j = 0 ; j < c_len ; j += 2 ){
	buf[ j/2 ].byte1 = (unsigned char)0x7f & (unsigned char)s[ p+j+0 ];
	buf[ j/2 ].byte2 = (unsigned char)0x7f & (unsigned char)s[ p+j+1 ];
      }
      XSetFont( _dpy, gc, _zenkaku->fid );
      XDrawString16( _dpy, w, gc, x, y, (XChar2b*)buf, c_len/2 );
      x += _zenkaku->max_bounds.width * c_len/2;
    }
    p += c_len;
  }
}

//
// draw_image_string(): 文字列を描画する。
//
void XFontPair::draw_image_string( Window w, GC gc, int x, int y, char* s )
{
  int p = 0;
  y += _hankaku->ascent;
  while( s[p] ){
    int zen_f = iskanji(s[p]);
    int c_len, c_step = zen_f?2:1;
    for(c_len = 0 ; zen_f==iskanji(s[p+c_len]) && s[p+c_len]; c_len += c_step)
      ;
    if( !zen_f ){
      char buf[1024];
      memcpy( buf, &s[p], c_len );
      XSetFont( _dpy, gc, _hankaku->fid );
      XDrawImageString( _dpy, w, gc, x, y, buf, c_len );
      x += _hankaku->max_bounds.width * c_len;
    }else{
      XChar2b buf[512];
      for( int j = 0 ; j < c_len ; j += 2 ){
	buf[ j/2 ].byte1 = (unsigned char)0x7f & (unsigned char)s[ p+j+0 ];
	buf[ j/2 ].byte2 = (unsigned char)0x7f & (unsigned char)s[ p+j+1 ];
      }
      XSetFont( _dpy, gc, _zenkaku->fid );
      XDrawImageString16( _dpy, w, gc, x, y, (XChar2b*)buf, c_len/2 );
      x += _zenkaku->max_bounds.width * c_len/2;
    }
    p += c_len;
  }
}
