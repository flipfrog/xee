//
// icon.c: アイコンウィジェット
//
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <local_types.h>
#include <xfontpair.h>
#include <widget.h>
#include <icon.h>

//
// IconWidget(): コンストラクタ
//
IconWidget::IconWidget( Widget* _parent, char* n ) :Widget( _parent, n )
{
  pixmap = 0;
  widget_type = WT_ICON;
}

IconWidget::IconWidget( IconWidget* icon ): Widget( icon )
{
  if( icon->pixmap != 0 ){
    pixmap = XCreatePixmap( dpy, icon->pixmap, _w, _h,
			   DefaultDepth(dpy,DefaultScreen(dpy) ) );
    XSetPlaneMask( dpy, gc, ~0 );
    XSetFunction( dpy, gc, GXcopy );
    XCopyArea( dpy, icon->pixmap, pixmap, gc, 0, 0, _w, _h, 0, 0 );
  }
}

//
// デストラクタ
//
IconWidget::~IconWidget()
{
  if( pixmap != 0 )
    XFreePixmap( dpy, pixmap );
}

//
// draw(): アイコンウィジェットを描画する。
//
void IconWidget::draw()
{
  if( pixmap != 0 ){
    XSetFunction( dpy, gc, GXcopy );
    XSetPlaneMask( dpy, gc, ~0 );
    XSetForeground( dpy, gc, pixels[WIDGET_FOREGROUND_PIXEL] );
    XSetBackground( dpy, gc, pixels[WIDGET_BACKGROUND_PIXEL] );
    if( xpm_f ){
      XCopyArea( dpy, pixmap, window, gc, 0, 0, _w, _h, 0, 0 );
    }else{
      XCopyPlane( dpy, pixmap, window, gc, 0, 0, _w, _h, 0, 0, 1 );
    }
  }
}

//
// set_bitmap(): ビットマップを設定する。
//
void IconWidget::set_bitmap( char* data, int data_w, int data_h )
{
  xpm_f = False;
  pixmap = XCreateBitmapFromData( dpy, window, data, data_w, data_h );
  resize( data_w, data_h );
}

//
// set_pixmap(): ピックスマップを設定する。
//
void IconWidget::set_pixmap( char** data )
{
  xpm_f = True;
  XpmColorSymbol colorsymbol =
    { "mask", NULL, pixels[WIDGET_BACKGROUND_PIXEL] };
  XpmAttributes attr;
  attr.colormap = DefaultColormap( dpy, DefaultScreen(dpy) );
  attr.colorsymbols = &colorsymbol;
  attr.numsymbols = 1;
  attr.valuemask = XpmColormap|XpmColorSymbols|XpmSize;
  XpmCreatePixmapFromData( dpy, window, data, &pixmap, NULL, &attr );
  if( pixmap == 0 ){
    fprintf( stderr, "IconWidget::set_pixmap(): cannot allocate pixmap.\n");
    exit(1);
  }
  resize( attr.width, attr.height );
}

//
// event_proc():
//
void IconWidget::event_proc( XEvent* event )
{
  if( event->xany.window == window ){
    switch( event->type ){
    case Expose:
      draw();
      break;
    }
  }
}
