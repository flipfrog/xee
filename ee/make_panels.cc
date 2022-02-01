//
// make_panels.cc:
//
#include <stdio.h>
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
#include <icon.h>
#include <message.h>

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

#include <panel_bitmaps.h>

//
// cancel_func():
//
static void cancel_func( Widget* w )
{
  Dialog* dialog = (Dialog*)(w->client_data);
  dialog->done_dialog_loop();
}

//
// fig_button_func():
//
static void fig_button_func( Widget* w )
{
  Dialog* dialog = (Dialog*)(w->client_data);
  Board* board = (Board*)(dialog->client_data);
  TextWidget* text = (TextWidget*)(dialog->search_by_name_widget("file-name"));
  char* s = text->get_string();
  if( strchr( s, '.' ) == NULL )
    strcat( s, ".fig" );
  text->set_string( s );
  if( board->xfig( s ) )
    dialog->done_dialog_loop();
}

//
// tex_button_func():
//
static void tex_button_func( Widget* w )
{
  Dialog* dialog = (Dialog*)(w->client_data);
  Board* board = (Board*)(dialog->client_data);
  TextWidget* text = (TextWidget*)(dialog->search_by_name_widget("file-name"));
  char* s = text->get_string();
  if( strchr( s, '.' ) == NULL )
    strcat( s, ".tex" );
  text->set_string( s );
  if( board->tex( s ) )
    dialog->done_dialog_loop();
}

//
// load_button_func():
//
static void load_button_func( Widget* w )
{
  Dialog* dialog = (Dialog*)(w->client_data);
  Board* board = (Board*)(dialog->client_data);
  TextWidget* text =
    (TextWidget*)(dialog->search_by_name_widget("file-name"));
  char* s = text->get_string();
  if( strchr( s, '.' ) == NULL )
    strcat( s, ".epc" );
  text->set_string( s );
  if( board->load( s ) )
    dialog->done_dialog_loop();
}

//
// save_button_func():
//
static void save_button_func( Widget* w )
{
  Dialog* dialog = (Dialog*)(w->client_data);
  Board* board = (Board*)(dialog->client_data);
  CheckWidget* check =
    (CheckWidget*)(dialog->search_by_name_widget("msdos-compatibility"));
  board->msdos_compatibility = check->get_value();
  TextWidget* text = (TextWidget*)(dialog->search_by_name_widget("file-name"));
  char* s = text->get_string();
  if( strchr( s, '.' ) == NULL )
    strcat( s, ".epc" );
  text->set_string( s );
  if( board->save( s ) )
    dialog->done_dialog_loop();
}

//
// quit_func():
//
static void quit_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  if( board->update_f && board->shapeset.count_shape() > 0 ){
    char* msg = "図形の更新後セーブしていませんが終了してもいいですか？";
    if( !yesno_box( board->frame, msg, "終了する", "編集に復帰する") )
      return;
  }
  exit(0);
}

//
// fig_func():
//
static void fig_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  xcontext->do_dialog_loop( board->fig_dialog, 100, 100 );
}

//
// tex_func():
//
static void tex_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  xcontext->do_dialog_loop( board->tex_dialog, 100, 100 );
}

//
// loadsave_func():
//
static void loadsave_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  CheckWidget* check =
    (CheckWidget*)(board->loadsave_dialog->
		   search_by_name_widget("msdos-compatibility"));
  check->set_value( board->msdos_compatibility );
  xcontext->do_dialog_loop( board->loadsave_dialog, 100, 100 );
}

//
// setup_func():
//
static void setup_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  Dialog* dialog = board->setup_dialog;
  TextWidget* text;
  char buf[256];
  text = (TextWidget*)(dialog->search_by_name_widget( "grid_width" ));
  sprintf( buf, "%d", board->grid_width );
  text->set_string( buf );
  text = (TextWidget*)(dialog->search_by_name_widget( "border_width" ));
  sprintf( buf, "%d", board->border_width );
  text->set_string( buf );
  text = (TextWidget*)(dialog->search_by_name_widget( "arrow_size" ));
  sprintf( buf, "%d", board->arrow_size );
  text->set_string( buf );
  text = (TextWidget*)(dialog->search_by_name_widget( "ball_size" ));
  sprintf( buf, "%d", board->ball_size );
  text->set_string( buf );
  CheckWidget* check;
  check = (CheckWidget*)(dialog->search_by_name_widget( "disp_small_str" ));
  check->set_value( board->disp_small_str );
  check = (CheckWidget*)(dialog->search_by_name_widget( "grip_disp" ));
  check->set_value( board->grip_disp );
  xcontext->do_dialog_loop( dialog, 100, 100 );
}

//
// setup_ok_func():
//
static void setup_ok_func( Widget* button )
{
  Boolean redraw_f = False;
  Board* board = (Board*)(button->client_data);
  Dialog* dialog = board->setup_dialog;
  TextWidget* text;
  int last_value = board->grid_width;
  text = (TextWidget*)(dialog->search_by_name_widget( "grid_width" ));
  board->grid_width = atoi(text->get_string());
  if( last_value != board->grid_width )
    redraw_f = True;
  text = (TextWidget*)(dialog->search_by_name_widget( "border_width" ));
  board->border_width = atoi(text->get_string());
  text = (TextWidget*)(dialog->search_by_name_widget( "arrow_size" ));
  board->arrow_size = atoi(text->get_string());
  text = (TextWidget*)(dialog->search_by_name_widget( "ball_size" ));
  board->ball_size = atoi(text->get_string());
  CheckWidget* check;
  check = (CheckWidget*)(dialog->search_by_name_widget( "disp_small_str" ));
  Boolean last_f = board->disp_small_str;
  board->disp_small_str = check->get_value();
  if( last_f != board->disp_small_str )
    redraw_f = True;
  check = (CheckWidget*)(dialog->search_by_name_widget( "grip_disp" ));
  last_f = board->grip_disp;
  board->grip_disp = check->get_value();
  if( last_f != board->grip_disp )
    redraw_f = True;
  dialog->done_dialog_loop();
  if( redraw_f )
    XClearArea( board->xcontext->get_dpy(),
	       board->canvas->get_canvas_window(), 0, 0, 0, 0, True );
}

//
// redraw_func():
//
static void redraw_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  XClearArea( board->xcontext->get_dpy(), board->canvas->get_canvas_window(),
	     0, 0, 0, 0, True );
}

//
// delete_func():
//
static void delete_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  if( board->shapeset.count_shape() == 0 ){
    message_box( board->frame, "図形は存在しません…", "確認" );
    return;
  }
  char* msg = "全ての図形を削除します。よろしいですか？";
  if( yesno_box( board->frame, msg, "削除する", "キャンセル" ) ){
    board->alldel();
    XClearArea( board->xcontext->get_dpy(),
	       board->canvas->get_canvas_window(),
	       0, 0, 0, 0, True );
  }
}

//
// undelete_func():
//
static void undelete_func( Menuitem* item )
{
  Board* board = (Board*)(item->client_data);
  Display* dpy = board->xcontext->get_dpy();
  GC gc = board->gc;
  if( board->deleted_shape.count_shape() == 0 ){
    message_box( board->frame, "削除した図形は存在しません…", "よかです" );
    return;
  }
  XSetPlaneMask( dpy, gc, board->shape_mask );
  XSetFunction( dpy, gc, GXset );
  Shape* shape =
    board->deleted_shape.get_shape( board->deleted_shape.count_shape()-1 );
  board->shapeset.append_shape( shape );
  board->deleted_shape.unlink_shape( LIST_END, 0, 1 );
  XSetPlaneMask( dpy, gc, ~0 );
  XSetFunction( dpy, gc, GXcopy );
}

//
// rs_notice(): 文字囲みモードの設定
//
static void rs_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
  static RoundStyle round_style_list[] = {
    RS_NONE, RS_RECT, RS_CAPSULE
    };
  board->round_style = round_style_list[ tile->get_value() ];
}

//
// magnet_notice(): カーソル磁石モードの設定
//
static void magnet_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
  if( tile->get_value() == 0 )
    board->cursor_quontize = True;
  else
    board->cursor_quontize = False;
}

//
// cursor_notice(): カーソル形状モードの設定
//
static void cursor_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
  if( tile->get_value() == 0 )
    board->cursor_mode = CM_CROSS;
  else
    board->cursor_mode = CM_FULL;
}

//
// fill_notice(): 塗り潰しモードの設定
//
static void fill_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
  board->fill_mode = tile->get_value();
}

//
// line_width_notice(): 線幅の設定
//
static void line_width_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
#define __line_width line_width
#undef line_width
  board->line_width = tile->get_value()+1;
#define line_width __line_width
}

//
// ls_notice(): ラインモードの設定
//
static void ls_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
  static LineStyle line_style_list[] = {
    LS_SOLID, LS_DASH, LS_DOT
    };
  board->line_style = line_style_list[ tile->get_value() ];
}

//
// lm_notice(): 終端処理モードの設定
//
static void lm_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
  static LineMode line_mode_list[] = {
    LM_SOLID, LM_FARROW, LM_RARROW, LM_BARROW, LM_OMASSOC, LM_MMASSOC
    };
  board->line_mode = line_mode_list[ tile->get_value() ];
}


//
// insert_type_notice(): 挿入図形と編集モードの設定
//
static void insert_type_notice( Widget* tile )
{
  Board* board = (Board*)(tile->client_data);
  static ShapeType shape_type_list[] = {
    ST_ARC, ST_LINE, ST_OVAL, ST_RECT, ST_SPLINE, ST_STRING, ST_GROUP,
  };
  static EditMode edit_mode_list[] = {
    EM_UNGROUP, EM_MOVE, EM_DELETE, EM_COPY, EM_RESIZE,
    EM_REVERSX, EM_REVERSY, EM_ROTATE, EM_UPDATE,
  };
  if( tile->get_value() < NUMBER(shape_type_list) ){
    board->edit_mode = EM_INSERT;
    board->insert_type = shape_type_list[ tile->get_value() ];
  }else{
    board->edit_mode =
      edit_mode_list[ tile->get_value()-NUMBER(shape_type_list) ];
  }
}

//
// make_panel():
//
void Board::make_panel()
{
  frame = new Frame( xcontext, "frame" );
  XStoreName( xcontext->get_dpy(), frame->window, "xee 70% (^^;" );
  XSetIconName( xcontext->get_dpy(), frame->window, "xee" );

  PanelWidget* panel = new PanelWidget( frame, "panel" );
  frame->set_panel( panel );

  // メインフレームのパネルを作成する。

  Menuitem* item;
  MenubarWidget* menubar = new MenubarWidget( panel, "menubar" );
  int menu_no = menubar->append_menu( "ファイル" );
  menubar->append_menuitem(menu_no,(item=new Menuitem( "ロード/セーブ…" )));
  item->client_data = this;
  item->callback = loadsave_func;
  menubar->append_menuitem( menu_no, (item = new Menuitem( "Fig変換…" )) );
  item->client_data = this;
  item->callback = fig_func;
  menubar->append_menuitem( menu_no, (item = new Menuitem( "TeX変換…" )) );
  item->client_data = this;
  item->callback = tex_func;
  menubar->append_menuitem( menu_no, (item = new Menuitem( "終了" )) );
  item->client_data = this;
  item->callback = quit_func;
  menu_no = menubar->append_menu( "編集" );
  menubar->
    append_menuitem( menu_no, (item = new Menuitem("編集モードの設定…" )) );
  item->client_data = this;
  item->callback = setup_func;
  menubar->
    append_menuitem( menu_no, (item = new Menuitem("再描画" )) );
  item->client_data = this;
  item->callback = redraw_func;
  menubar->
    append_menuitem( menu_no, (item = new Menuitem("全図形削除" )) );
  item->client_data = this;
  item->callback = delete_func;
  menubar->
    append_menuitem( menu_no, (item = new Menuitem("削除した図形の復活" )) );
  item->client_data = this;
  item->callback = undelete_func;

  panel->append_widget( menubar );

  TileWidget* tile = new TileWidget( panel, "挿入図形:" );
  tile->append_bitmap( arc_bits, 24, 24 );
  tile->append_bitmap( line_bits, 24, 24 );
  tile->append_bitmap( oval_bits, 24, 24 );
  tile->append_bitmap( rectangle_bits, 24, 24 );
  tile->append_bitmap( spline_bits, 24, 24 );
  tile->append_bitmap( string_bits, 24, 24 );
  tile->append_bitmap( group_bits, 24, 24 );

  tile->append_bitmap( ungroup_bits, 24, 24 );
  tile->append_bitmap( move_bits, 24, 24 );
  tile->append_bitmap( delete_bits, 24, 24 );
  tile->append_bitmap( copy_bits, 24, 24 );
  tile->append_bitmap( resize_bits, 24, 24 );
  tile->append_bitmap( reversx_bits, 24, 24 );
  tile->append_bitmap( reversy_bits, 24, 24 );
  tile->append_bitmap( rotate_bits, 24, 24 );
  tile->append_bitmap( update_bits, 24, 24 );

  tile->move( 10, menubar->y()+menubar->h()+5 );
  tile->client_data = this;
  tile->callback = insert_type_notice;
  panel->append_widget( tile );

  TileWidget* tile2 = new TileWidget( panel, "終端処理:" );
  tile2->append_bitmap( lmsolid_bits, 24, 12 );
  tile2->append_bitmap( lmfarrow_bits, 24, 12 );
  tile2->append_bitmap( lmrarrow_bits, 24, 12 );
  tile2->append_bitmap( lmbarrow_bits, 24, 12 );
  tile2->append_bitmap( lmomassoc_bits, 24, 12 );
  tile2->append_bitmap( lmmmassoc_bits, 24, 12 );

  tile2->move( 10, tile->y()+tile->h()+5 );
  tile2->client_data = this;
  tile2->callback = lm_notice;
  panel->append_widget( tile2 );

  TileWidget* tile3 = new TileWidget( panel, "ラインスタイル:" );
  tile3->append_bitmap( lssolid_bits, 24, 12 );
  tile3->append_bitmap( lsdash_bits, 24, 12 );
  tile3->append_bitmap( lsdot_bits, 24, 12 );

  tile3->move( tile2->x()+tile2->w()+10, tile->y()+tile->h()+5 );
  tile3->client_data = this;
  tile3->callback = ls_notice;
  panel->append_widget( tile3 );
  
  TileWidget* tile7 = new TileWidget( panel, "線幅" );
  tile7->append_bitmap( width_one_bits, 12, 12 );
  tile7->append_bitmap( width_two_bits, 12, 12 );
  tile7->append_bitmap( width_three_bits, 12, 12 );

  tile7->move( tile3->x()+tile3->w()+10, tile->y()+tile->h()+5 );
  tile7->client_data = this;
  tile7->callback = line_width_notice;
  panel->append_widget( tile7 );

  TileWidget* tile4 = new TileWidget( panel, "文字囲みモード:" );
  tile4->append_bitmap( rsnone_bits, 12, 12 );
  tile4->append_bitmap( rsrect_bits, 12, 12 );
  tile4->append_bitmap( rscapsule_bits, 12, 12 );

  tile4->move( tile7->x()+tile7->w()+10, tile->y()+tile->h()+5 );
  tile4->client_data = this;
  tile4->callback = rs_notice;
  panel->append_widget( tile4 );

  TileWidget* tile5 = new TileWidget( panel, "カーソル磁石" );
  tile5->append_bitmap( magnet_on_bits, 12, 12 );
  tile5->append_bitmap( magnet_off_bits, 12, 12 );

  tile5->move( tile4->x()+tile4->w()+10, tile->y()+tile->h()+5 );
  tile5->client_data = this;
  tile5->callback = magnet_notice;
  panel->append_widget( tile5 );

  TileWidget* tile6 = new TileWidget( panel, "カーソル形状" );
  tile6->append_bitmap( cross_cursor_bits, 12, 12 );
  tile6->append_bitmap( full_cursor_bits, 12, 12 );

  tile6->move( tile5->x()+tile5->w()+10, tile->y()+tile->h()+5 );
  tile6->client_data = this;
  tile6->callback = cursor_notice;
  panel->append_widget( tile6 );

  TileWidget* tile8 = new TileWidget( panel, "塗り潰しモード" );
  tile8->append_bitmap( fill_off_bits, 12, 12 );
  tile8->append_bitmap( fill_on_bits, 12, 12 );

  tile8->move( tile6->x()+tile6->w()+10, tile->y()+tile->h()+5 );
  tile8->client_data = this;
  tile8->callback = fill_notice;
  panel->append_widget( tile8 );

  canvas = new CanvasWidget( panel, "canvas" );
  canvas->event_func = board_event_func;
  canvas->client_data = this;
  canvas->add_input_mask( PointerMotionMask|LeaveWindowMask );
  canvas->set_canvas_size( paper_w, paper_h );
  canvas->move_resize( 0, tile4->y()+tile4->h()+10,
		      canvas->get_complete_width(), 300 );
  panel->append_widget( canvas );

  menubar->resize( panel->w()-menubar->x(), menubar->h() );

  // Ｆｉｇ変換ダイアログを作成する。

  fig_dialog = new Dialog( frame, "fig-dialog" );
  panel = new PanelWidget( fig_dialog, "panel" );
  fig_dialog->set_panel( panel );
  fig_dialog->client_data = this;

  IconWidget* icon = new IconWidget( panel, "fig" );
  icon->set_pixmap( fig_xpm );
  icon->move( 10, 10 );
  panel->append_widget( icon );

  TextWidget* text =
    new TextWidget( xcontext->get_skkserv(), panel, "file-name" );
  text->set_label( "ファイル名:" );
  text->set_input_len( 30 );
  text->client_data = fig_dialog;
  text->callback = fig_button_func;
  text->move( icon->x()+icon->w()+10, icon->y()+(icon->h()-text->h())/2 );
  panel->append_widget( text );

  ButtonWidget* cancel_button = new ButtonWidget( panel, "cancel-button" );
  cancel_button->set_label( "キャンセル" );
  cancel_button->client_data = fig_dialog;
  cancel_button->callback = cancel_func;
  cancel_button->move( panel->w()-cancel_button->w(), text->y()+text->h()+10 );
  panel->append_widget( cancel_button );

  ButtonWidget* ok_button = new ButtonWidget( panel, "fig-button" );
  ok_button->set_label( "Ｆｉｇ変換" );
  ok_button->client_data = fig_dialog;
  ok_button->callback = fig_button_func;
  ok_button->move( cancel_button->x()-ok_button->w()-10, cancel_button->y() );
  panel->append_widget( ok_button );

  panel->resize( panel->w()+10, panel->h()+10 );
  panel->set_3d_edge();

  // ＴｅＸ変換ダイアログを作成する。

  tex_dialog = new Dialog( frame, "tex-dialog" );
  panel = new PanelWidget( tex_dialog, "panel" );
  tex_dialog->set_panel( panel );
  tex_dialog->client_data = this;

  icon = new IconWidget( panel, "tex_lion" );
  icon->set_bitmap( tex_lion_bits, tex_lion_width, tex_lion_height );
  icon->move( 10, 10 );
  panel->append_widget( icon );

  text = new TextWidget( xcontext->get_skkserv(), panel, "file-name" );
  text->set_label( "ファイル名:" );
  text->move( icon->x()+icon->w()+5, icon->y()+(icon->h()-text->h())/2 );
  text->set_input_len( 30 );
  text->client_data = tex_dialog;
  text->callback = tex_button_func;
  panel->append_widget( text );
  cancel_button = new ButtonWidget( panel, "cancel-button" );
  cancel_button->set_label( "キャンセル" );
  cancel_button->client_data = tex_dialog;
  cancel_button->callback = cancel_func;
  cancel_button->move( panel->w()-cancel_button->w(), text->y()+text->h()+10 );
  panel->append_widget( cancel_button );


  ok_button = new ButtonWidget( panel, "tex-button" );
  ok_button->set_label( "ＴｅＸ変換" );
  ok_button->client_data = tex_dialog;
  ok_button->callback = tex_button_func;
  ok_button->move( cancel_button->x()-10-ok_button->w(), cancel_button->y() );
  panel->append_widget( ok_button );

  panel->resize( panel->w()+10, panel->h()+10 );
  panel->set_3d_edge();

  // データロードセーブダイアログを作成する。

  loadsave_dialog = new Dialog( frame, "load-save-dialog" );
  panel = new PanelWidget( loadsave_dialog, "panel" );
  loadsave_dialog->set_panel( panel );
  loadsave_dialog->client_data = this;

  icon = new IconWidget( panel, "file_server" );
  icon->set_pixmap( file_server_xpm );
  icon->move( 10, 10 );
  panel->append_widget( icon );

  text = new TextWidget( xcontext->get_skkserv(), panel, "file-name" );
  text->set_label( "ファイル名:" );
  text->set_input_len( 30 );
  text->client_data = loadsave_dialog;
  text->callback = load_button_func;
  text->move( icon->x()+icon->w()+10, icon->y() );
  panel->append_widget( text );

  CheckWidget* check = new CheckWidget( panel, "msdos-compatibility" );
  check->set_label( "ＭＳ−ＤＯＳ版互換モード" );
  check->move( text->x(), text->y()+text->h()+5 );
  panel->append_widget( check );

  cancel_button = new ButtonWidget( panel, "cancel-button" );
  cancel_button->set_label( "キャンセル" );
  cancel_button->client_data = loadsave_dialog;
  cancel_button->callback = cancel_func;
  cancel_button->move(panel->w()-cancel_button->w(), check->y()+check->h()+7);
  panel->append_widget( cancel_button );

  ButtonWidget* save_button = new ButtonWidget( panel, "save-button" );
  save_button->set_label( "  セーブ  " );
  save_button->client_data = loadsave_dialog;
  save_button->callback = save_button_func;
  save_button->move(cancel_button->x()-save_button->w()-10,cancel_button->y());
  panel->append_widget( save_button );

  ButtonWidget* load_button = new ButtonWidget( panel, "load-button" );
  load_button->set_label( "  ロード  " );
  load_button->client_data = loadsave_dialog;
  load_button->callback = load_button_func;
  load_button->move( save_button->x()-load_button->w()-30, save_button->y() );
  panel->append_widget( load_button );

  panel->resize( panel->w()+10, panel->h()+10 );
  panel->set_3d_edge();

  // 編集モード設定ダイアログを作成する。

  setup_dialog = new Dialog( frame, "setup-dialog" );
  panel = new PanelWidget( setup_dialog, "panel" );
  setup_dialog->set_panel( panel );
  setup_dialog->client_data = this;

  int last_x = 10, last_y = 10;

  text = new TextWidget( xcontext->get_skkserv(), panel, "grid_width" );
  text->set_label( "グリッドの幅        ：" );
  text->move( 10, last_y );
  text->client_data = this;
  text->callback = setup_ok_func;
  panel->append_widget( text );
  last_y = text->y() + text->h() + 10;

  text = new TextWidget( xcontext->get_skkserv(), panel, "border_width" );
  text->set_label( "文字を囲む線の境界幅：" );
  text->move( 10, last_y );
  text->client_data = this;
  text->callback = setup_ok_func;
  panel->append_widget( text );
  last_y = text->y() + text->h() + 10;

  text = new TextWidget( xcontext->get_skkserv(), panel, "arrow_size" );
  text->set_label( "矢印の大きさ        ：" );
  text->move( 10, last_y );
  text->client_data = this;
  text->callback = setup_ok_func;
  panel->append_widget( text );
  last_y = text->y() + text->h() + 10;

  text = new TextWidget( xcontext->get_skkserv(), panel, "ball_size" );
  text->set_label( "ボ−ルの大きさ      ：" );
  text->move( 10, last_y );
  text->client_data = this;
  text->callback = setup_ok_func;
  panel->append_widget( text );
  last_y = text->y() + text->h() + 10;

  check = new CheckWidget( panel, "disp_small_str" );
  check->set_label( "小さな文字列の表示" );
  check->move( last_x, last_y );
  panel->append_widget( check );
  last_x = check->x() + check->w() + 10;

  check = new CheckWidget( panel, "grip_disp" );
  check->set_label( "グリップポイント表示" );
  check->move( last_x, last_y );
  panel->append_widget( check );
  last_y = check->y() + check->h() + 10;

  cancel_button = new ButtonWidget( panel, "cancel_button" );
  cancel_button->set_label( "キャンセル" );
  cancel_button->client_data = setup_dialog;
  cancel_button->callback = cancel_func;
  cancel_button->move( panel->w()-cancel_button->w()-10, last_y );
  panel->append_widget( cancel_button );

  ok_button = new ButtonWidget( panel, "ok_button" );
  ok_button->set_label( "この条件を適用する" );
  ok_button->client_data = this;
  ok_button->callback = setup_ok_func;
  ok_button->move( cancel_button->x()-ok_button->w()-10, cancel_button->y() );
  panel->append_widget( ok_button );

  panel->resize( panel->w()+10, panel->h()+10 );
  panel->set_3d_edge();
}  
