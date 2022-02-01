//
// board.c:
//
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include <local_types.h>
#include <list.h>

#include <geometry.h>

#include <eedefs.h>
#include <xfontpair.h>
#include <skkserv.h>
#include <skkfep.h>
#include <widget.h>

#include <panel.h>
#include <frame.h>
#include <button.h>
#include <menubar.h>
#include <tile.h>
#include <text.h>
#include <scrollbar.h>
#include <canvas.h>
#include <check.h>

#include <shape.h>
#include <shapeset.h>
#include <board.h>

#include <misc.h>

#include <point.h>
#include <line.h>
#include <spline.h>
#include <rect.h>
#include <oval.h>
#include <str.h>
#include <arc.h>
#include <group.h>

#include <xcontext.h>
extern Xcontext* xcontext;
#include <bitmaps/cursor.xbm>

void board_event_func( XEvent* event, CanvasWidget* c, Display* dpy, GC gc,
		      Window w, void* client_data );

//
// コンストラクタ
//
Board::Board( Xcontext* _xcontext )
{
  xcontext = _xcontext;
  paper_w = 630, paper_h = 883;
  make_panel();
  gc = canvas->gc;

  unsigned long p[2], pm[3];
  Display* dpy = xcontext->get_dpy();
  Colormap cmap = DefaultColormap( dpy, DefaultScreen(dpy) );
  if( !XAllocColorCells( dpy, cmap, False, pm, 3, p, 2 ) ){
    fprintf( stderr, "Board::Board(): cannot allocate color cells.\n");
    exit(errno);
  }
  grid_pixel = p[0], bg_pixel = p[1];
  shape_mask = pm[0], layout_mask = pm[1], cursor_mask = pm[2];
  struct {
    char* name;
    unsigned long pixel;
  } color_table[] = {
    { "gray",     grid_pixel },
    { "white",    grid_pixel|cursor_mask },
    { "white",    grid_pixel|shape_mask },
    { "yellow",   grid_pixel|layout_mask },
    { "yellow",   grid_pixel|shape_mask|layout_mask },
//    { "red",      grid_pixel|cursor_mask|layout_mask },
    { "yellow",      grid_pixel|cursor_mask|layout_mask },
    { "yellow",   grid_pixel|shape_mask|cursor_mask },
//    { "red",      grid_pixel|layout_mask|shape_mask|cursor_mask },
    { "yellow",      grid_pixel|layout_mask|shape_mask|cursor_mask },
    { "navyblue", bg_pixel },
    { "white",    bg_pixel|cursor_mask },
    { "white",    bg_pixel|shape_mask },
    { "yellow",   bg_pixel|layout_mask },
    { "yellow",   bg_pixel|shape_mask|layout_mask },
//    { "red",      bg_pixel|cursor_mask|layout_mask },
    { "yellow",      bg_pixel|cursor_mask|layout_mask },
    { "yellow",   bg_pixel|shape_mask|cursor_mask },
//    { "red",      bg_pixel|shape_mask|layout_mask|cursor_mask }
    { "yellow",      bg_pixel|shape_mask|layout_mask|cursor_mask }
  };
  for( int i = 0 ; i < Number(color_table) ; i++ )
    XStoreNamedColor( dpy, cmap, color_table[i].name, color_table[i].pixel,
		     DoRed|DoGreen|DoBlue );
  XSetWindowBackground( dpy, canvas->get_canvas_window(), bg_pixel );
  XUndefineCursor( dpy, canvas->get_canvas_window() );
//  xfp = new XFontPair( xcontext->get_dpy(), "fm8x16", "fmk16" );
  xfp = new XFontPair( xcontext->get_dpy(), "7x14", "k14" );
//  {
//    char* han = "-misc-marumoji-medium-r-normal--14-130-75-75-c-70-iso8859-1";
//    char* zen = "-misc-marumoji-medium-r-normal--14-130-75-75-c-140-"
//      "jisx0208.1983-0";
//    xfp = new XFontPair( xcontext->get_dpy(), han, zen );
//  }
  skkfep = new SkkFep( xfp, xcontext->get_skkserv() );
  skkfep->set_pixels( shape_mask, 0 );
  skkfep->set_draw_function( GXset );

  Pixmap pixmap =
    XCreateBitmapFromData( dpy, canvas->get_canvas_window(),
			  cursor_bits, cursor_width, cursor_height );
  XColor fcol, bcol;
  fcol.red = fcol.green = fcol.blue = 65535;
  bcol.red = bcol.green = bcol.blue = 0;
  Cursor cur = XCreatePixmapCursor( dpy, pixmap, pixmap,
				   &fcol, &bcol, cursor_x_hot, cursor_y_hot );
  XDefineCursor( dpy, canvas->get_canvas_window(), cur );

  batch_mode = False;
  insert_type = ST_ARC;
  edit_mode = EM_INSERT;
  fill_mode = False;
  line_mode = LM_SOLID;
  line_style = LS_SOLID;
  round_style = RS_NONE;
  cursor_mode = CM_CROSS;

  line_width = 1;
  arrow_size = 8;
  ball_size = 4;
  grid_width = 16;
  grid_disp = True;
  border_width = 5;
  grip_radious = 8;
  grip_disp = False;
  cursor_quontize = True;
  update_f = False;
  font_size = FONT_normalsize;
  disp_small_str = False;
  msdos_compatibility = False;
  real_moving = False;
  last_epc[0] = 0;
  deleted_shape.set_virtual_link( LIST_VIRTUAL );
}

//
// board_event_func():
//
void board_event_func( XEvent* event, CanvasWidget* c, Display* dpy, GC gc,
		      Window w, void* client_data )
{
  static int last_x = -1, last_y = -1;
  int mx, my;
  Board* board = (Board*)client_data;
  XSetFunction( board->xcontext->get_dpy(), board->gc, GXcopy );
  switch( event->type ){
  case Expose:
    // 背景を描画する。
    if( board->grid_disp ){
      static char dot[] = { 1, 3 };
      XSetLineAttributes( dpy, gc, 0, LineOnOffDash, CapButt, JoinMiter );
      XSetDashes( dpy, gc, 0, dot, Number(dot) );
      XSetForeground( dpy, gc, board->grid_pixel );
      int grid_step = board->grid_width;
      for( int i = 0 ; i < board->paper_w ; i += grid_step )
	XDrawLine( dpy, w, gc, i, 0, i, board->paper_h );
      for( i = 0 ; i < board->paper_h ; i += grid_step )
	XDrawLine( dpy, w, gc, 0, i, board->paper_w, i );
      XSetLineAttributes( dpy, gc, 0, LineSolid, CapButt, JoinMiter );
    }

    // 図形データを順に描画する。
    XSetPlaneMask( dpy, gc, board->shape_mask );
    XSetFunction( dpy, gc, GXset );
    board->shapeset.draw_shape( w, 0, 0 );
    XSetPlaneMask( dpy, gc, ~0 );
    XSetFunction( dpy, gc, GXcopy );
    break;
  case ButtonPress:
  case ButtonRelease:
    mx = event->xbutton.x, my = event->xbutton.y;
    break;
  case MotionNotify:
    mx = event->xmotion.x, my = event->xmotion.y;
    break;
  case KeyPress:
  case KeyRelease:
    mx = event->xkey.x, my = event->xkey.y;
    break;
  case LeaveNotify:
    if( last_x >= 0 ){
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, board->cursor_mask );
      draw_cursor( dpy, w, gc, last_x, last_y, board );
      last_x = last_y = -1;
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
    }
    break;
  }

  if(event->type != ButtonPress &&
     event->type != ButtonRelease &&
     event->type != MotionNotify &&
     event->type != KeyPress &&
     event->type != Expose )
    return;

  board->quontize( mx, my );

  if( (mx != last_x || my != last_y) ){
    if( last_x >= 0 ){
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, board->cursor_mask );
      draw_cursor( dpy, w, gc, last_x, last_y, board );
    }
    last_x = mx, last_y = my;
    XSetFunction( dpy, gc, GXset );
    XSetPlaneMask( dpy, gc, board->cursor_mask );
    draw_cursor( dpy, w, gc, last_x, last_y, board );
    XSetFunction( dpy, gc, GXcopy );
    XSetPlaneMask( dpy, gc, ~0 );
  }

  switch( board->edit_mode ){
  case EM_INSERT:
    board->layout_shape( event, mx, my );
    break;
  case EM_UNGROUP:
    board->ungroup_shape( event, mx, my );
    break;
  case EM_MOVE:
    board->move_shape( event, mx, my );
    break;
  case EM_DELETE:
    board->delete_shape( event, mx, my );
    break;
  case EM_COPY:
    board->copy_shape( event, mx, my );
    break;
  case EM_RESIZE:
    board->resize_shape( event, mx, my );
    break;
  case EM_REVERSX:
    board->reversx_shape( event, mx, my );
    break;
  case EM_REVERSY:
    board->reversy_shape( event, mx, my );
    break;
  case EM_UPDATE:
    board->update_shape( event, mx, my );
    break;
  case EM_ROTATE:
    board->rotate_shape( event, mx, my );
    break;
  }
  XSetFunction( dpy, gc, GXcopy );
}

//
// layout_shape(): 図形の追加配置処理を行う。
//
void Board::layout_shape( XEvent* event, int mx, int my )
{
  Boolean save_grip_disp = grip_disp;
  grip_disp = False;
  static Shape* insert_shape = NULL;
  if( insert_shape == NULL && event->type == ButtonPress ){
    switch( insert_type ){
    case ST_LINE:   insert_shape = new Line( this );   break;
    case ST_SPLINE: insert_shape = new Spline( this ); break;
    case ST_RECT:   insert_shape = new Rect( this );   break;
    case ST_OVAL:   insert_shape = new Oval( this );   break;
    case ST_ARC:    insert_shape = new Arc( this );    break;
    case ST_STRING: insert_shape = new Str( this );    break;
    case ST_GROUP:  insert_shape = new Group( this );  break;
    }
  }
  if( insert_shape != NULL ){
    switch( insert_shape->layout( event ) ){
    case EDIT_CONTINUE:
      break;
    case EDIT_CANCEL:
      delete insert_shape;
      insert_shape = NULL;
      XBell( xcontext->get_dpy(), 0 );
      break;
    case EDIT_COMPLETE:
      shapeset.append_shape( insert_shape );
      update_f = True;
      grip_disp = save_grip_disp;
      XSetFunction( xcontext->get_dpy(), gc, GXset );
      XSetPlaneMask( xcontext->get_dpy(), gc, shape_mask );
      insert_shape->draw( canvas->get_canvas_window(), 0, 0 );
      XSetFunction( xcontext->get_dpy(), gc, GXcopy );
      XSetPlaneMask( xcontext->get_dpy(), gc, ~0 );
      insert_shape = NULL;
      break;
    }
  }
  grip_disp = save_grip_disp;
}

//
// ungroup_shape():
//
void Board::ungroup_shape( XEvent* event, int mx, int my )
{
  if( event->type == ButtonPress ){
    for( int hw = 1 ; hw < 8 ; hw++ ){
      for( int i = 0 ; i < shapeset.count_shape() ; i++ ){
	Shape* ungroup_shape = shapeset.get_shape( i );
	if( ungroup_shape->hit( mx, my, hw ) &&
	   ungroup_shape->get_shape_type() == ST_GROUP ){
	  ungroup_shape->ungroup( event->xany.window );
	  shapeset.unlink_shape( LIST_TOP, i, 1 );
	  update_f = True;
	  return;
	}
      }
    }
  }
}

//
// move_shape():
//
void Board::move_shape( XEvent* event, int mx, int my )
{
  static Shape* move_shape = NULL;
  if( move_shape == NULL && event->type == ButtonPress )
    move_shape = shapeset.find_hit_shape( mx, my );

  if( move_shape != NULL ){
    switch( move_shape->move( event ) ){
    case EDIT_CONTINUE:
      break;
    case EDIT_COMPLETE:
      update_f = True;
    case EDIT_CANCEL:
      move_shape = NULL;
      break;
    }
  }
}

//
// delete_shape(): 図形の削除処理を行う。
//
void Board::delete_shape( XEvent* event, int mx, int my )
{
  Display* dpy = event->xany.display;
  Window window = event->xany.window;
  static int ox, oy, last_x, last_y;
  static proc_phase = 0; // 処理フェーズカウンタ
  switch( event->type ){
  case ButtonPress: // 左ボタンクリックが処理開始のトリガとなる。
    if( event->xbutton.button == 1 ){
      ox = last_x = mx, oy = last_y = my;
      if( event->xbutton.state & ControlMask ){
	// コントロールキーを押しながらのクリックは、範囲指定となる。
	proc_phase = 1;
      }else{
	// コントロールキーを押さない場合は、ポインタの位置にあるオブジェクト
	// を見付けて削除する。
	for( int i = shapeset.count_shape()-1 ; i >= 0 ; i-- ){
	  Shape* shape = shapeset.get_shape(i);
	  if( shape->hit( mx, my, 4 ) ){
	    XSetFunction( dpy, gc, GXclear );
	    XSetPlaneMask( dpy, gc, shape_mask );
	    shape->draw( window, 0, 0 );
	    deleted_shape.append_shape( shape->duplicate() );
	    shapeset.unlink_shape( LIST_TOP, i, 1 );
	    XSetFunction( dpy, gc, GXcopy );
	    XSetPlaneMask( dpy, gc, ~0 );
	    update_f = True;
	    break;
	  }
	}
      }
    }
    break;
  case MotionNotify: // 範囲削除のとき(proc_phase>=1)だけ処理を行う。
    switch( proc_phase ){
    case 1: // 最初の矩形の描画
      last_x = mx, last_y = my;
      int x1 = ox, y1 = oy, x2 = mx, y2 = my;
      normal_rect( x1, y1, x2, y2 );
      XSetPlaneMask( dpy, gc, layout_mask );
      XSetFunction( dpy, gc, GXset );
      XDrawRectangle( dpy, window, gc, x1, y1, x2-x1, y2-y1 );
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      proc_phase = 2;
      break;
    case 2: // ２回目以降の矩形の更新
      if( mx != last_x && my != last_y ){
	int x1 = ox, y1 = oy, x2 = last_x, y2 = last_y;
	XSetFunction( dpy, gc, GXclear );
	XSetPlaneMask( dpy, gc, layout_mask );
	normal_rect( x1, y1, x2, y2 );
	XDrawRectangle( dpy, window, gc, x1, y1, x2-x1, y2-y1 );
	x1 = ox, y1 = oy, x2 = last_x = mx, y2 = last_y = my;
	normal_rect( x1, y1, x2, y2 );
	XSetFunction( dpy, gc, GXset );
	XDrawRectangle( dpy, window, gc, x1, y1, x2-x1, y2-y1 );
	XSetFunction( dpy, gc, GXcopy );
	XSetPlaneMask( dpy, gc, ~0 );
      }
      break;
    }
    break;
  case ButtonRelease: // 範囲削除の終了処理
    if( event->xbutton.button == 1 && proc_phase == 2 ){
      if( ox != last_x || oy != last_y ){
	int x1 = ox, y1 = oy, x2 = last_x, y2 = last_y;
	XSetFunction( dpy, gc, GXclear );
	XSetPlaneMask( dpy, gc, layout_mask );
	normal_rect( x1, y1, x2, y2 );
	XDrawRectangle( dpy, window, gc, x1, y1, x2-x1, y2-y1 );
	XSetPlaneMask( dpy, gc, shape_mask );
	for( int i = shapeset.count_shape() ; i > 0 ; i-- ){
	  Shape* shape = shapeset.get_shape(i-1);
	  if( shape->contain( x1, y1, x2-x1, y2-y1 ) ){
	    shape->draw( window, 0, 0 );
	    deleted_shape.append_shape( shape->duplicate() );
	    shapeset.unlink_shape( LIST_TOP, i-1, 1 );
	    update_f = True;
	  }
	}
	XSetPlaneMask( dpy, gc, ~0 );
	XSetFunction( dpy, gc, GXcopy );
      }
      proc_phase = 0;
    }
    break;
  }
}

//
// copy_shape():
//
void Board::copy_shape( XEvent* event, int mx, int my )
{
  Display* dpy = event->xany.display;
  Window window = event->xany.window;
  static Shape* copy_shape = NULL;
  static Shape* src_shape;
  if( copy_shape == NULL && event->type == ButtonPress ){
    src_shape = shapeset.find_hit_shape( mx, my );
    if( src_shape != NULL ){
      copy_shape = src_shape->duplicate();
      XSetFunction( dpy, gc, GXset );
      XSetPlaneMask( dpy, gc, layout_mask );
      copy_shape->draw( window, 0, 0 );
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
    }
  }
  if( copy_shape != NULL ){
    switch( copy_shape->move( event ) ){
    case EDIT_CONTINUE:
      break;
    case EDIT_CANCEL:
      delete copy_shape;
      copy_shape = NULL;
      break;
    case EDIT_COMPLETE:
      XSetFunction( dpy, gc, GXset );
      XSetPlaneMask( dpy, gc, shape_mask );
      src_shape->draw( window, 0, 0 );
      XSetFunction( dpy, gc, GXcopy );
      XSetPlaneMask( dpy, gc, ~0 );
      shapeset.append_shape( copy_shape );
      copy_shape = NULL;
      update_f = True;
      break;
    }
  }
}

//
// reversx_shape():
//
void Board::reversx_shape( XEvent* event, int mx, int my )
{
  Display* dpy = event->xany.display;
  Shape* revers_shape = NULL;
  if( event->type == ButtonPress ){
    if( (revers_shape = shapeset.find_hit_shape( mx, my )) != NULL ){
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, shape_mask );
      revers_shape->draw( event->xbutton.window, 0, 0 );
      revers_shape->reversx();
      XSetFunction( dpy, gc, GXset );
      revers_shape->draw( event->xbutton.window, 0, 0 );
      XSetPlaneMask( dpy, gc, ~0 );
      XSetFunction( dpy, gc, GXcopy );
      update_f = True;
    }
  }
}

//
// reversy_shape():
//
void Board::reversy_shape( XEvent* event, int mx, int my )
{
  Display* dpy = event->xany.display;
  Shape* revers_shape = NULL;
  if( event->type == ButtonPress ){
    if( (revers_shape = shapeset.find_hit_shape( mx, my )) != NULL ){
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, shape_mask );
      revers_shape->draw( event->xbutton.window, 0, 0 );
      revers_shape->reversy();
      XSetFunction( dpy, gc, GXset );
      revers_shape->draw( event->xbutton.window, 0, 0 );
      XSetPlaneMask( dpy, gc, ~0 );
      XSetFunction( dpy, gc, GXcopy );
      update_f = True;
    }
  }
}

//
// rotate_shape():
//
void Board::rotate_shape( XEvent* event, int mx, int my )
{
  Display* dpy = event->xany.display;
  Shape* rotate_shape = NULL;
  if( event->type == ButtonPress ){
    if( (rotate_shape = shapeset.find_hit_shape( mx, my )) != NULL ){
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, shape_mask );
      rotate_shape->draw( event->xbutton.window, 0, 0 );
      rotate_shape->rotate( event );
      XSetFunction( dpy, gc, GXset );
      rotate_shape->draw( event->xbutton.window, 0, 0 );
      XSetPlaneMask( dpy, gc, ~0 );
      XSetFunction( dpy, gc, GXcopy );
      update_f = True;
    }
  }
}

//
// update_shape():
//
void Board::update_shape( XEvent* event, int mx, int my )
{
  Display* dpy = event->xany.display;
  Shape* update_shape = NULL;
  if( event->type == ButtonPress ){
    if( (update_shape = shapeset.find_hit_shape( mx, my )) != NULL ){
      XSetFunction( dpy, gc, GXclear );
      XSetPlaneMask( dpy, gc, shape_mask );
      update_shape->draw( event->xbutton.window, 0, 0 );
      update_shape->update();
      XSetFunction( dpy, gc, GXset );
      update_shape->draw( event->xbutton.window, 0, 0 );
      XSetPlaneMask( dpy, gc, ~0 );
      XSetFunction( dpy, gc, GXcopy );
      update_f = True;
    }
  }
}

//
// resize_shape():
//
void Board::resize_shape( XEvent* event, int mx, int my )
{
  static Shape* resize_shape = NULL;
  if( resize_shape == NULL && event->type == ButtonPress )
    resize_shape = shapeset.find_hit_shape( mx, my );
  if( resize_shape != NULL ){
    switch( resize_shape->resize( event ) ){
    case EDIT_CONTINUE:
      break;
    case EDIT_COMPLETE:
      update_f = True;
    case EDIT_CANCEL:
      resize_shape = NULL;
      break;
    }
  }
}

//
// save():
//
Boolean Board::save( char* fname )
{
  char buf[256];
  struct stat stbuf;

  if( stat( fname, &stbuf ) != -1 ){
    sprintf( buf, "'%s' が既に存在します。上書きしますか？", fname );
    if( !yesno_box( frame, buf, "上書きする", "キャンセル" ) )
      return False;
  }

  FILE* fp;
  if( (fp = fopen( fname, "w" )) == NULL ){
    sprintf( buf, "'%s' が書き込みオープンできませんでした。", fname );
    message_box( frame, buf, " 確認 " );
    return False;
  }

  update_f = False;
  
  fprintf( fp, "# EE Version %d.%d\n", MAJOR_VERSION, MINOR_VERSION );
  fprintf( fp, "# '%s'\n\n", fname );
  fprintf( fp, "resolution = high;\n\n" );

  shapeset.save_file( fp );

  fclose( fp );

  return True;
}

//
// tex():
//
Boolean Board::tex( char* fname )
{
  struct stat stbuf;
  char buf[256];

  if( stat( fname, &stbuf ) != -1 ){
    sprintf( buf, "'%s' が既に存在します。上書きしますか？", fname );
    if( !yesno_box( frame, buf, "上書きする", "中止" ) )
      return False;
  }

  FILE* fp;
  if( (fp = fopen( fname, "w" )) == NULL ){
    sprintf( buf, "'%s' が書き込みオープンできませんでした。", fname );
    message_box( frame, buf, " 確認 " );
    return False;
  }

  fprintf( fp, "%% Wrote by EE Version %d.%d\n",
	  MAJOR_VERSION, MINOR_VERSION );
  fprintf( fp, "%% '%s' All rights reserved, Copyright(C) 1993"
	  "Fujitsu limited.\n", fname );
  fprintf( fp, "\\setlength{\\unitlength}{0.00553in}\n" );

  shapeset.tex_file( fp );

  fclose( fp );

  return True;
}

//
// xfig():
//
Boolean Board::xfig( char* fname )
{
  struct stat stbuf;
  char buf[256];

  if( stat( fname, &stbuf ) != -1 ){
    sprintf( buf, "'%s' が既に存在します。上書きしますか？", fname );
    if( !yesno_box( frame, buf, "上書きする", "中止" ) )
      return False;
  }

  FILE* fp;
  if( (fp = fopen( fname, "w" )) == NULL ){
    sprintf( buf, "'%s' が書き込みオープンできませんでした。", fname );
    message_box( frame, buf, " 確認 " );
    return False;
  }

  fprintf( fp, "#FIG 2.1\n");
  fprintf( fp, "80 1\n");/* 180dpi->80dpi */
  shapeset.fig_file( fp );
  fclose( fp );

  return True;
}

//
// draw_gp():
//
void Board::draw_gp()
{
/*  shapeset.draw_shape_gp( window, 0, 0 ); */
}

//
// quontize():
//
void Board::quontize( int& x, int& y )
{
  if( cursor_quontize ){
    x = x - x % 4;
    y = y - y % 4;
  }
}

//
// alldel():
//
void Board::alldel()
{
  shapeset.unlink_shape( LIST_TOP, 0, shapeset.count_shape() );
}
