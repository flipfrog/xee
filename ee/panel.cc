/*****************************************************************************

 panel.c:

*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <jctype.h>

#include <bios.h>

#include <board.h>
#include <shape.h>
#include <misc.h>
#include <panel.h>

#include <bitmap.h>

#include <itemset.h>
#include <scrbar.h>
#include <button.h>
#include <tile.h>
#include <check.h>
#include <text.h>
#include <message.h>
#include <dialog.h>
#include <menuitem.h>
#include <menu.h>
#include <menubar.h>
#include <icon.h>

#include <bstore.h>
#include <bitmaps.h>

static DialogClass* file_dialog;
static DialogClass* setup_dialog;
static DialogClass* tex_dialog;
static DialogClass* xfig_dialog;

static struct{
  int em;
  int fm;
  int st;
  char* data;
} edit_table[] = {
{ EM_INSERT, False, ST_LINE, line_bits },
{ EM_INSERT, False, ST_SPLINE, spline_bits },
{ EM_INSERT, False, ST_RECT, rect_bits },
{ EM_INSERT, False, ST_OVAL, oval1_bits },
{ EM_INSERT, True,  ST_OVAL, oval2_bits },
{ EM_INSERT, False, ST_ARC, arc_bits },
{ EM_INSERT, False, ST_STRING, alpha1_bits },
{ EM_INSERT, False, ST_GROUP, group_bits },
{ EM_REVERSX, False, -1, revx_bits },
{ EM_REVERSY, False, -1, revy_bits },
{ EM_MOVE, False, -1, move_bits },
{ EM_DELETE, False, -1, del_bits },
{ EM_COPY, False, -1, copy_bits },
{ EM_RESIZE, False, -1, resize_bits },
{ EM_UPDATE, False, -1, update_bits } };

static struct{
  int ls;
  char* data;
} line_style_table[] = {
{ LS_SOLID, solid_bits },
{ LS_DASH, dash_bits },
{ LS_DOT, dot_bits } };

static struct{
  int lm;
  char* data;
} line_mode_table[] = {
{ LM_SOLID, solid_bits },
{ LM_FARROW, farw_bits },
{ LM_RARROW, rarw_bits },
{ LM_BARROW, barw_bits },
{ LM_OMASSOC, om_bits },
{ LM_MMASSOC, mm_bits } };

static struct{
  int rs;
  char* data;
} round_style_table[] = {
{ RS_NONE, alpha1_bits },
{ RS_RECT, alpha2_bits },
{ RS_CAPSULE, alpha3_bits } };

static struct{
  FontSize size;
  char* label;
} font_size_table[] = {
{ FONT_tiny,         " tiny(5pt)          " },
{ FONT_scriptsize,   " scriptsize(7pt)    " },
{ FONT_footnotesize, " footnotesize(*8pt) " },
{ FONT_small,        " small(*9pt)        " },
{ FONT_normalsize,   " normalsize(*10pt)  " },
{ FONT_large,        " large(*12pt)       " },
{ FONT_Large,        " Large(*14pt)       " },
{ FONT_LARGE,        " LARGE(17pt)        " },
{ FONT_huge,         " huge(*20pt)        " },
{ FONT_Huge,         " Huge(25pt)         " } };

/*****************************************************************************

 draw_panels():

*/
void draw_panels( BoardClass* this )
{
  Boolean cm_save = BmGetClipMode();

  /* コントロールパネルを描画する。*/

  BmSetClipMode( BmOFF );
  BmSetFunction( GXset );
  BmDrawLine( this->x-1, this->y-1, this->x+this->w, this->y-1 );
  BmDrawLine( this->x-1, this->y-1, this->x-1, this->y+this->h );
  BmDrawLine(this->x,this->y+this->h,this->x+this->w,this->y+this->h);
  this->itemset->draw( this->itemset, 0, 0 );
  draw_mode( this );
  draw_filename( this );
  BmSetClipMode( cm_save );
}

/*****************************************************************************

 scr_callback(): スクロールバーの通知関数

*/
static void scr_callback( ScrollbarClass* scr, void* data, void* itemset )
{
  BoardClass* board = data;
  int cm_save = BmGetClipMode();
  BmSetClipMode( BmOFF );
  BmSetFunction( GXset );
  if( strcmp( scr->name, "水平" ) == 0 ){
    int dx = (int)(scr->get_attr(scr,SCR_POSITION))-board->x_offset;
    board->x_offset += dx;
    if( abs(dx) >= board->w ){
      wait_cursor( True );
      BmClearArea( board->x, board->y, board->w, board->h );
      BmSetClipArea( board->x, board->y, board->w, board->h );
    }else if( dx > 0 ){
      wait_cursor( True );
      BmCopyArea(board->x+dx,board->y,board->w-dx,board->h,board->x,board->y);
      BmClearArea( board->x+board->w-dx, board->y, dx, board->h );
      BmSetClipArea( board->x+board->w-dx, board->y, dx, board->h );
    }else if( dx < 0 ){
      wait_cursor( True );
      BmCopyArea(board->x,board->y,board->w+dx,board->h,board->x-dx,board->y);
      BmClearArea( board->x, board->y, -dx, board->h );
      BmSetClipArea( board->x, board->y, -dx, board->h );
    }
    if( abs(dx) > 0 ){
      BmSetClipMode( BmON );
      board->draw( board );
      wait_cursor( False );
    }
  }else{
    int dy = (int)(scr->get_attr(scr,SCR_POSITION))-board->y_offset;
    board->y_offset += dy;
    if( abs(dy) >= board->h ){
      wait_cursor( True );
      BmClearArea( board->x, board->y, board->w, board->h );
      BmSetClipArea( board->x, board->y, board->w, board->h );
    }else if( dy > 0 ){
      wait_cursor( True );
      BmCopyArea(board->x,board->y+dy,board->w,board->h-dy,board->x,board->y);
      BmClearArea( board->x, board->y+board->h-dy, board->w, dy );
      BmSetClipArea( board->x, board->y+board->h-dy, board->w, dy );
    }else if( dy < 0 ){
      wait_cursor( True );
      BmCopyArea(board->x,board->y,board->w,board->h+dy,board->x,board->y-dy);
      BmClearArea( board->x, board->y, board->w, -dy );
      BmSetClipArea( board->x, board->y, board->w, -dy );
    }
    if( abs(dy) > 0 ){
      BmSetClipMode( BmON );
      board->draw( board );
      wait_cursor( False );
    }
  }
  BmSetClipArea( board->x, board->y, board->w, board->h );
  BmSetClipMode( cm_save );
}

/*****************************************************************************

 quit_callback():

*/
static void quit_callback( MenuItemClass* menuitem )
{
  BoardClass* board = menuitem->client_data;
  if( board->update_flag ){
    if( !yesno( "図形の変更後セーブしていませんが終了しますか？",
	       " 終了 ", "取り消し" ) )
      return;
  }
  BmFinish();
  BmSetClipMode( BmOFF );
  BmClearGraphic();
  exit(0);
}

/*****************************************************************************

 dialog_cancel():

*/
static void dialog_cancel( ButtonClass* button, void* data, void* itemset )
{
  DialogClass* dialog = data;
  dialog->do_loop = False;
}

/*****************************************************************************

 file_callback():

*/
static void file_callback( MenuItemClass* menuitem )
{
  BoardClass* board = menuitem->client_data;
  TextClass* text = file_dialog->find_item( file_dialog, "ファイル名：" );
  int xx = (board->w - file_dialog->w)/2 + board->x;
  int yy = (board->h - file_dialog->h)/2 + board->y;
  text->set_attr( text, TEXT_STRING, board->last_epc );
  file_dialog->interactive( file_dialog, xx, yy );
  wait_cursor( True );
  BmClearArea( board->x, board->y, board->w, board->h );
  BmSetClipMode( BmON );
  board->draw( board );
  BmSetClipMode( BmOFF );
  wait_cursor( False );
}

/*****************************************************************************

 setup_callback():

*/
static void setup_callback( MenuItemClass* menuitem )
{
  BoardClass* board = menuitem->client_data;
  int xx = (board->w - setup_dialog->w)/2 + board->x;
  int yy = (board->h - setup_dialog->h)/2 + board->y;
  setup_dialog->interactive( setup_dialog, xx, yy );
  wait_cursor( True );
  BmClearArea( board->x, board->y, board->w, board->h );
  BmSetClipMode( BmON );
  board->draw( board );
  BmSetClipMode( BmOFF );
  wait_cursor( False );
}

/*****************************************************************************

 LoadSave_callback():

*/
static void LoadSave_callback(ButtonClass* button, void* data, void* itemset)
{
  DialogClass* dialog = data;
  TextClass* text = dialog->find_item( dialog, "ファイル名：" );
  BoardClass* board = dialog->client_data;
  Boolean ret;
  char buf[ 128 ];

  strncpy( buf, (char*)(text->get_attr( text, TEXT_STRING )), sizeof(buf) );

  if( strchr( buf, '.' ) == NULL )
    strcat( buf, ".epc" );
  if( strcmp( button->name, "セーブ" ) == 0 )
    ret = board->save( board, buf );
  else
    ret = board->load( board, buf );
  if( ret ){
    dialog->do_loop = False;
    strcpy( board->last_epc, text->get_attr( text, TEXT_STRING ) );
    draw_filename( board );
  }
}

/*****************************************************************************

 texconv_callback():

*/
static void texconv_callback(ButtonClass* button, void* data, void* itemset)
{
  DialogClass* dialog = data;
  TextClass* text = dialog->find_item( dialog, "ファイル名：" );
  BoardClass* board = dialog->client_data;
  Boolean ret;

  ret = board->tex( board, (char*)(text->get_attr( text, TEXT_STRING )) );
  if( ret )
    dialog->do_loop = False;
}

/*****************************************************************************

 xfigconv_callback():

*/
static void xfigconv_callback(ButtonClass* button, void* data, void* itemset)
{
  DialogClass* dialog = data;
  TextClass* text = dialog->find_item( dialog, "ファイル名：" );
  BoardClass* board = dialog->client_data;
  Boolean ret;

  ret = board->xfig( board, (char*)(text->get_attr( text, TEXT_STRING )) );
  if( ret )
    dialog->do_loop = False;
}

/*****************************************************************************

 setup_confirm_callback():

*/
static void
setup_confirm_callback( ButtonClass* button, void* data, void* itemset )
{
  DialogClass* dialog = data;
  BoardClass* board = dialog->client_data;
  TextClass* text;
  CheckClass* check;
  char buf[128];

  check = dialog->find_item( dialog, "小さな文字列の表示" );
  board->disp_small_str = (Boolean)(check->get_attr(check,CHECK_ON));

  check = dialog->find_item( dialog, "グリップポイント表示" );
  board->grip_disp = (Boolean)(check->get_attr(check,CHECK_ON));

  check = dialog->find_item( dialog, "カーソル位置のクオンタイズ" );
  board->cursor_polish = (Boolean)(check->get_attr(check,CHECK_ON));

  check = dialog->find_item( dialog, "グリッドの表示" );
  board->grid_disp = (Boolean)(check->get_attr(check,CHECK_ON));

  text = dialog->find_item( dialog, "        グリッドの幅(pixel)        ：" );
  strcpy( buf, (char*)(text->get_attr( text, TEXT_STRING )) );
  if( strlen(buf) > 0 && atoi( buf ) > 0 )
    board->grid_width = atoi(buf);

  text = dialog->find_item( dialog, "        文字を囲む線の境界幅(pixel)：" );
  strcpy( buf, (char*)(text->get_attr( text, TEXT_STRING )) );
  if( strlen(buf) > 0 && atoi( buf ) > 0 )
    board->border_width = atoi(buf);

  text = dialog->find_item( dialog, "オブジェクトを探査する幅(半径pixel)：" );
  strcpy( buf, (char*)(text->get_attr( text, TEXT_STRING )) );
  if( strlen(buf) > 0 && atoi( buf ) > 0 )
    board->grip_radious = atoi(buf);

  text = dialog->find_item( dialog, "                矢印の大きさ(pixel)：" );
  strcpy( buf, (char*)(text->get_attr( text, TEXT_STRING )) );
  if( strlen(buf) > 0 && atoi( buf ) > 0 )
    board->arrow_size = atoi(buf);

  text = dialog->find_item( dialog, "          ボ−ルの大きさ(半径pixel)：" );
  strcpy( buf, (char*)(text->get_attr( text, TEXT_STRING )) );
  if( strlen(buf) > 0 && atoi( buf ) > 0 )
    board->ball_size = atoi(buf);

  dialog->do_loop = False;
}

/*****************************************************************************

 tex_callback():

*/
static void tex_callback( MenuItemClass* menuitem )
{
  char buf[64];
  BoardClass* board = menuitem->client_data;
  BackingStoreClass* bstore = NULL;
  TextClass* text = tex_dialog->find_item( tex_dialog, "ファイル名：" );
  int xx = (board->w - tex_dialog->w)/2 + board->x;
  int yy = (board->h - tex_dialog->h)/2 + board->y;
  bstore = BackingStore__create( xx, yy, tex_dialog->w, tex_dialog->h );
  if( strlen( board->last_epc ) > 0 ){
    strcpy( buf, board->last_epc );
    if( strchr( buf, '.' ) != NULL )
      *strchr( buf, '.' ) = 0;
    strcat( buf, ".tex" );
    text->set_attr( text, TEXT_STRING, buf );
  }
  tex_dialog->interactive( tex_dialog, xx, yy );
  if( bstore != NULL ){
    BackingStore__delete( bstore );
  }else{
    wait_cursor( True );
    BmClearArea( board->x, board->y, board->w, board->h );
    BmSetClipMode( BmON );
    board->draw( board );
    BmSetClipMode( BmOFF );
    wait_cursor( False );
  }
}

/*****************************************************************************

 xfig_callback():

*/
static void xfig_callback( MenuItemClass* menuitem )
{
  char buf[ 64 ];
  BoardClass* board = menuitem->client_data;
  BackingStoreClass* bstore = NULL;
  TextClass* text = xfig_dialog->find_item( xfig_dialog, "ファイル名：" );
  int xx = (board->w - tex_dialog->w)/2 + board->x;
  int yy = (board->h - tex_dialog->h)/2 + board->y;
  bstore = BackingStore__create( xx, yy, xfig_dialog->w, xfig_dialog->h );
  if( strlen( board->last_epc ) > 0 ){
    strcpy( buf, board->last_epc );
    if( strchr( buf, '.' ) != NULL )
      *strchr( buf, '.' ) = 0;
    strcat( buf, ".fig" );
    text->set_attr( text, TEXT_STRING, buf );
  }
  xfig_dialog->interactive( xfig_dialog, xx, yy );
  if( bstore != NULL ){
    BackingStore__delete( bstore );
  }else{
    wait_cursor( True );
    BmClearArea( board->x, board->y, board->w, board->h );
    BmSetClipMode( BmON );
    board->draw( board );
    BmSetClipMode( BmOFF );
    wait_cursor( False );
  }
}

/*****************************************************************************

 wipe_callback():

*/
static void wipe_callback( MenuItemClass* menuitem )
{
  Boolean cm_save = BmGetClipMode();
  BoardClass* this = menuitem->client_data;
  wait_cursor( True );
  BmClearArea( this->x, this->y, this->w, this->h );
  BmSetClipMode( BmON );
  this->draw( this );
  BmSetClipMode( cm_save );
  wait_cursor( False );
}

/*****************************************************************************

 font_callback():

*/
static void font_callback( MenuItemClass* menuitem )
{
  int i;
  for( i = 0 ; i < NUMBER(font_size_table) ; i++ ){
    if( strcmp( font_size_table[i].label, menuitem->label ) == NULL ){
      BoardClass* board = menuitem->client_data;
      board->font_size = font_size_table[i].size;
      draw_mode( board );
      return;
    }
  }
}

/*****************************************************************************

 layout_callback():

*/
static void layout_callback( MenuItemClass* menuitem )
{
#define BW (BmGetFontWidth()/2)
  int i, lx, ly;
  double scale_x, scale_y;
  int layout_w, layout_h;
  Boolean cm_save = BmGetClipMode();
  BoardClass* this = menuitem->client_data;
  BackingStoreClass* bstore = NULL;

  wait_cursor( True );
  layout_h = this->h;
  scale_y = (double)(layout_h-BW*2)/(double)(this->paper_h);
  scale_x = scale_y;
  layout_w = ROUND( (double)(this->paper_w)*scale_x )+BW*2;
  lx = this->x+(this->w-layout_w)/2, ly = this->y;
  if( !BmCheckHighreso() )
    bstore = BackingStore__create( lx, ly, layout_w, layout_h );
  BmClearArea( lx, ly, layout_w, layout_h );
  BmDrawRectangle( lx, ly, layout_w, layout_h );
  BmDrawRectangle( lx+BW, ly+BW, layout_w-BW*2, layout_h-BW*2 );
  for( i = 0 ; i < this->shape_slot->count(this->shape_slot) ; i++ ){
    ShapeClass* shape = this->shape_slot->abs_get( this->shape_slot, TOP, i );
    shape->t_draw( shape, scale_x, scale_y, lx+BW, ly+BW, this );
  }
  wait_cursor( False );
  while(1){
    Event event;
    BmGetNextEvent( &event );
    if( event.what == Btn0Release || event.what == Btn1Release )
      break;
  }
  if( bstore != NULL ){
    BackingStore__delete( bstore );
  }else{
    wait_cursor( True );
    BmClearArea( this->x, this->y, this->w, this->h );
    BmSetClipMode( BmON );
    this->draw( this );
    wait_cursor( False );
  }
  BmSetClipMode( cm_save );
}

/*****************************************************************************

 alldel_callback():

*/
static void alldel_callback( MenuItemClass* menuitem )
{
  Boolean cm_save = BmGetClipMode();
  BoardClass* this = menuitem->client_data;
  if( yesno("全ての図形を削除します。よろしいですか？", "削除", "取り消し") ){
    wait_cursor( True );
    this->alldel( this );
    BmClearArea( this->x, this->y, this->w, this->h );
    BmSetClipMode( BmON );
    this->draw( this );
    BmSetClipMode( cm_save );
    wait_cursor( False );
  }
}

/*****************************************************************************

 edit_callback():

*/
static edit_callback( TileClass* tile, void* client_data, void* itemset )
{
  BoardClass* board = tile->client_data;
  int sel = (int)(tile->get_attr( tile, TILE_SELECTED ));

  /* 編集モードの設定 */
  if( edit_table[sel].em >= 0 )
    board->edit_mode = (EditMode)(edit_table[sel].em);
  /* 挿入図形の設定 */
  if( edit_table[sel].st >= 0 )
    board->insert_eprim = (ShapeType)(edit_table[sel].st);
  /* 塗り潰しモードの設定 */
  board->fill = edit_table[sel].fm;

  draw_mode( board );
}

/*****************************************************************************

 line_style_callback():

*/
static void line_style_callback( TileClass* tile, void* cdata, void* itemset )
{
  BoardClass* board = cdata;
  int sel = (int)(tile->get_attr( tile, TILE_SELECTED ));
  board->ls = (LineStyle)(line_style_table[sel].ls);
}

/*****************************************************************************

 line_mode_callback():

*/
static void line_mode_callback( TileClass* tile, void* cdata, void* itemset )
{
  BoardClass* board = cdata;
  int sel = (int)(tile->get_attr( tile, TILE_SELECTED ));
  board->lm = (LineMode)(line_mode_table[sel].lm);
}

/*****************************************************************************

 round_style_callback():

*/
static void round_style_callback(TileClass* tile, void* cdata, void* itemset)
{
  BoardClass* board = cdata;
  int sel = (int)(tile->get_attr( tile, TILE_SELECTED ));
  board->rs = (RoundStyle)(round_style_table[sel].rs);
}

/*****************************************************************************

 make_panels():

*/
void make_panels( BoardClass* board )
{
  int i, last_y, last_x;
  char buf[64];
  ScrollbarClass* scr;
  ButtonClass* button;
  TileClass* tile;
  CheckClass* check;
  TextClass* text;
  MessageClass* message;
  BitmapClass* bitmap;
  MenubarClass* menubar;
  MenuClass* menu;
  MenuItemClass* menuitem;
  IconClass* icon;

  BmSetForeground( FG_COLOR );
  BmSetBackground( BG_COLOR );

  board->itemset = Itemset__create();

  board->x = 1;
  board->y = 0;

  board->w = BmGetScreenWidth() - BmGetFontWidth()*2 - 1;

  menubar = Menubar__create();
  menu = Menu__create("ファイル");

  menuitem = MenuItem__create(" ロード／セーブ ");
  menuitem->notice_func = file_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menuitem = MenuItem__create(" ＴｅＸ変換     ");
  menuitem->notice_func = tex_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menuitem = MenuItem__create(" Ｆｉｇ変換     ");
  menuitem->notice_func = xfig_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menuitem = MenuItem__create(" 終了           ");
  menuitem->notice_func = quit_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menubar->set_attr( menubar, MENUBAR_MENU, menu );

  menu = Menu__create("編集");

  menuitem = MenuItem__create(" 編集環境の設定     ");
  menuitem->notice_func = setup_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menuitem = MenuItem__create(" 全オブジェクト削除 ");
  menuitem->notice_func = alldel_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menubar->set_attr( menubar, MENUBAR_MENU, menu );

  menu = Menu__create("表示");

  menuitem = MenuItem__create(" 再描画         ");
  menuitem->notice_func = wipe_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menuitem = MenuItem__create(" レイアウト表示 ");
  menuitem->notice_func = layout_callback;
  menuitem->client_data = board;
  menu->append_item( menu, menuitem );

  menubar->set_attr( menubar, MENUBAR_MENU, menu );

  menu = Menu__create("フォント");

  for( i = 0 ; i < NUMBER(font_size_table) ; i++ ){
    menuitem = MenuItem__create( font_size_table[i].label );
    menuitem->notice_func = font_callback;
    menuitem->client_data = board;
    menu->append_item( menu, menuitem );
  }
  board->itemset->append_item( board->itemset, menubar );
  last_x = BmGetFontWidth();
  last_y = menubar->y + menubar->h + BmGetFontWidth()/2;

  menubar->set_attr( menubar, MENUBAR_MENU, menu );

  tile = Tile__create("編集");
  tile->callback = edit_callback;
  tile->client_data = board;
  tile->x = last_x, tile->y = last_y;
  for( i = 0 ; i < NUMBER(edit_table) ; i++ ){
    BitmapClass* bitmap = Bitmap__create( edit_table[i].data, 16, 16 );
    if( BmCheckHighreso() )
      bitmap->expand( bitmap, 2, 2 );
    tile->set_attr( tile, TILE_BITMAP, bitmap );
  }
  board->itemset->append_item( board->itemset, tile );
  last_x = tile->x + tile->w + BmGetFontWidth();

  tile = Tile__create("矢印");
  tile->callback = line_mode_callback;
  tile->client_data = board;
  tile->x = last_x, tile->y = last_y;
  for( i = 0 ; i < NUMBER(line_mode_table) ; i++ ){
    BitmapClass* bitmap = Bitmap__create( line_mode_table[i].data, 16, 16 );
    if( BmCheckHighreso() )
      bitmap->expand( bitmap, 2, 2 );
    tile->set_attr( tile, TILE_BITMAP, bitmap );
  }
  board->itemset->append_item( board->itemset, tile );
  last_x = tile->x + tile->w + BmGetFontWidth();

  tile = Tile__create("線種");
  tile->callback = line_style_callback;
  tile->client_data = board;
  tile->x = last_x, tile->y = last_y;
  for( i = 0 ; i < NUMBER(line_style_table) ; i++ ){
    BitmapClass* bitmap = Bitmap__create( line_style_table[i].data, 16, 16 );
    if( BmCheckHighreso() )
      bitmap->expand( bitmap, 2, 2 );
    tile->set_attr( tile, TILE_BITMAP, bitmap );
  }
  board->itemset->append_item( board->itemset, tile );
  last_x = tile->x + tile->w + BmGetFontWidth();

  tile = Tile__create("文字囲み");
  tile->callback = round_style_callback;
  tile->client_data = board;
  tile->x = last_x, tile->y = last_y;
  for( i = 0 ; i < NUMBER(round_style_table) ; i++ ){
    BitmapClass* bitmap = Bitmap__create( round_style_table[i].data, 16, 16 );
    if( BmCheckHighreso() )
      bitmap->expand( bitmap, 2, 2 );
    tile->set_attr( tile, TILE_BITMAP, bitmap );
  }
  board->itemset->append_item( board->itemset, tile );

  board->y = tile->y+tile->h+BmGetFontWidth()/2;
  board->h = BmGetScreenHeight() - board->y - BmGetFontHeight()*2;

  scr = Scrollbar__create( "水平", SCR_HOLIZONTAL );
  scr->x = board->x+board->w/2, scr->y = board->y+board->h;
  scr->w = board->w/2;
  scr->client_data = board;
  scr->callback = scr_callback;
  scr->set_attr( scr, SCR_OBJECT_LEN, (void*)(board->paper_w - board->w) );
  scr->set_attr( scr, SCR_PAGE_STEP, (void*)(board->w) );
  scr->set_attr( scr, SCR_MOVE_STEP, (void*)(BmGetFontWidth()*10) );
  board->itemset->append_item( board->itemset, scr );

  if( board->w >= board->paper_w || BmCheckHighreso() )
    scr->status = 0;

  scr = Scrollbar__create( "垂直", SCR_VERTICAL );
  scr->x = board->x+board->w, scr->y = board->y;
  scr->h = board->h;
  scr->client_data = board;
  scr->callback = scr_callback;
  scr->set_attr( scr, SCR_OBJECT_LEN, (void*)(board->paper_h - board->h) );
  scr->set_attr( scr, SCR_PAGE_STEP, (void*)((board->h/4)*3) );
  scr->set_attr( scr, SCR_MOVE_STEP, (void*)(BmGetFontWidth()*10) );
  board->itemset->append_item( board->itemset, scr );

  /* ファイル管理ダイアログ */

  file_dialog = Dialog__create("LOAD-SAVE");
  file_dialog->client_data = board;
  last_y = 0;

  if( BmCheckHighreso() )
    bitmap = Bitmap__create( file48_bits, file48_width, file48_height );
  else
    bitmap = Bitmap__create( file32_bits, file32_width, file32_height );
  icon = Icon__create( "file_icon" );
  icon->set_attr( icon, ICON_BITMAP, bitmap );
  file_dialog->append_item( file_dialog, icon );
  last_y = icon->y+icon->h + BmGetFontWidth();

  message = Message__create("file-msg");
  message->x = icon->x+icon->w+BmGetFontWidth();
  message->set_attr( message, ITEM_LABEL, "ファイルの管理" );
  file_dialog->append_item( file_dialog, message );

  text = Text__create("ファイル名：");
  text->x = icon->x+icon->w+BmGetFontWidth(), text->y = last_y;
  text->set_attr( text, TEXT_INPUT_LEN, (void*)(sizeof(board->last_epc)-2) );
  file_dialog->append_item( file_dialog, text );
  last_y = text->y + text->h + BmGetFontHeight();
  last_x = text->x+text->w;

  button = Button__create("取り消し");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'C' );
  button->x = last_x - button->w - BmGetFontWidth(), button->y = last_y;
  button->callback = dialog_cancel;
  button->client_data = file_dialog;
  file_dialog->append_item( file_dialog, button );
  last_x = button->x;

  button = Button__create("セーブ");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'S' );
  button->x = last_x - button->w - BmGetFontWidth()*2, button->y = last_y;
  button->callback = LoadSave_callback;
  button->client_data = file_dialog;
  file_dialog->append_item( file_dialog, button );
  last_x = button->x;

  button = Button__create("ロード");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'L' );
  button->x = last_x - button->w - BmGetFontWidth(), button->y = last_y;
  button->callback = LoadSave_callback;
  button->client_data = file_dialog;
  file_dialog->append_item( file_dialog, button );

  /* ＴｅＸ変換ダイアログ */

  tex_dialog = Dialog__create("TEX");
  tex_dialog->client_data = board;
  last_y = 0;

  if( BmCheckHighreso() )
    bitmap = Bitmap__create( tex48_bits, tex48_width, tex48_height );
  else
    bitmap = Bitmap__create( tex32_bits, tex32_width, tex32_height );
  icon = Icon__create( "tex_icon" );
  icon->set_attr( icon, ICON_BITMAP, bitmap );
  tex_dialog->append_item( tex_dialog, icon );
  last_y = icon->y + icon->h + BmGetFontWidth();

  message = Message__create("tex-msg");
  message->x = icon->y + icon->w + BmGetFontWidth();
  message->set_attr( message, ITEM_LABEL, "ＴｅＸ変換処理" );
  message->y = 0;
  tex_dialog->append_item( tex_dialog, message );

  text = Text__create("ファイル名：");
  text->x = icon->x+icon->w+BmGetFontWidth(), text->y = last_y;
  text->set_attr( text, TEXT_INPUT_LEN, (void*)30 );
  tex_dialog->append_item( tex_dialog, text );
  last_y = text->y + text->h + BmGetFontHeight();

  button = Button__create("取り消し");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'C' );
  button->x = text->w - button->w - BmGetFontWidth(), button->y = last_y;
  button->callback = dialog_cancel;
  button->client_data = tex_dialog;
  file_dialog->append_item( tex_dialog, button );
  last_x = button->x;

  button = Button__create("変換");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'X' );
  button->x = last_x - button->w - BmGetFontWidth(), button->y = last_y;
  button->callback = texconv_callback;
  button->client_data = tex_dialog;
  tex_dialog->append_item( tex_dialog, button );

  /* ｘｆｉｇ変換ダイアログ */

  xfig_dialog = Dialog__create("xfig");
  xfig_dialog->client_data = board;
  last_y = 0;

  message = Message__create("xfig-msg");
  message->set_attr( message, ITEM_LABEL, "◇ ｘｆｉｇ変換処理" );
  message->y = last_y;
  xfig_dialog->append_item( xfig_dialog, message );
  last_y = message->y + message->h + BmGetFontWidth();

  text = Text__create("ファイル名：");
  text->x = 0, text->y = last_y;
  text->set_attr( text, TEXT_INPUT_LEN, (void*)30 );
  xfig_dialog->append_item( xfig_dialog, text );
  last_y = text->y + text->h + BmGetFontHeight();

  button = Button__create("取り消し");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'C' );
  button->x = text->w - button->w - BmGetFontWidth(), button->y = last_y;
  button->callback = dialog_cancel;
  button->client_data = xfig_dialog;
  file_dialog->append_item( xfig_dialog, button );
  last_x = button->x;

  button = Button__create("変換");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'X' );
  button->x = last_x - button->w - BmGetFontWidth(), button->y = last_y;
  button->callback = xfigconv_callback;
  button->client_data = xfig_dialog;
  xfig_dialog->append_item( xfig_dialog, button );

  /* その他の設定 */

  setup_dialog = Dialog__create("Setup");
  setup_dialog->client_data = board;
  last_y = 0;

  message = Message__create("setup-msg");
  message->set_attr( message, ITEM_LABEL, "◇ 編集モードの設定" );
  message->y = last_y;
  setup_dialog->append_item( setup_dialog, message );
  last_y = message->y + message->h + BmGetFontWidth();

  text = Text__create("        グリッドの幅(pixel)        ：");
  text->x = 0, text->y = last_y;
  sprintf( buf, "%d", board->grid_width );
  text->set_attr( text, TEXT_STRING, (void*)buf );
  text->set_attr( text, TEXT_INPUT_LEN, (void*)5 );
  setup_dialog->append_item( setup_dialog, text );
  last_y = text->y + text->h + BmGetFontWidth();

  text = Text__create("        文字を囲む線の境界幅(pixel)：");
  text->x = 0, text->y = last_y;
  sprintf( buf, "%d", board->border_width );
  text->set_attr( text, TEXT_STRING, (void*)buf );
  text->set_attr( text, TEXT_INPUT_LEN, (void*)5 );
  setup_dialog->append_item( setup_dialog, text );
  last_y = text->y + text->h + BmGetFontWidth();

  text = Text__create("オブジェクトを探査する幅(半径pixel)：");
  text->x = 0, text->y = last_y;
  sprintf( buf, "%d", board->grip_radious );
  text->set_attr( text, TEXT_STRING, (void*)buf );
  text->set_attr( text, TEXT_INPUT_LEN, (void*)5 );
  setup_dialog->append_item( setup_dialog, text );
  last_y = text->y + text->h + BmGetFontWidth();

  text = Text__create("                矢印の大きさ(pixel)：");
  text->x = 0, text->y = last_y;
  sprintf( buf, "%d", board->arrow_size );
  text->set_attr( text, TEXT_STRING, (void*)buf );
  text->set_attr( text, TEXT_INPUT_LEN, (void*)5 );
  setup_dialog->append_item( setup_dialog, text );
  last_y = text->y + text->h + BmGetFontWidth();

  text = Text__create("          ボ−ルの大きさ(半径pixel)：");
  text->x = 0, text->y = last_y;
  sprintf( buf, "%d", board->ball_size );
  text->set_attr( text, TEXT_STRING, (void*)buf );
  text->set_attr( text, TEXT_INPUT_LEN, (void*)5 );
  setup_dialog->append_item( setup_dialog, text );
  last_y = text->y + text->h + BmGetFontWidth();

  check = Check__create("グリップポイント表示");
  check->x = 0, check->y = last_y;
  check->set_attr( check, CHECK_ON, (void*)(board->grip_disp) );
  setup_dialog->append_item( setup_dialog, check );
  last_y = check->y + check->h + BmGetFontWidth();

  check = Check__create("カーソル位置のクオンタイズ");
  check->x = 0, check->y = last_y;
  check->set_attr( check, CHECK_ON, (void*)(board->cursor_polish) );
  setup_dialog->append_item( setup_dialog, check );
  last_y = check->y + check->h + BmGetFontWidth();

  check = Check__create("グリッドの表示");
  check->x = 0, check->y = last_y;
  check->set_attr( check, CHECK_ON, (void*)(board->grid_disp) );
  setup_dialog->append_item( setup_dialog, check );
  last_y += check->h + BmGetFontWidth();

  check = Check__create("小さな文字列の表示");
  check->x = 0, check->y = last_y;
  check->set_attr( check, CHECK_OFF, (void*)(board->grid_disp) );
  setup_dialog->append_item( setup_dialog, check );
  last_y += check->h + BmGetFontHeight();

  button = Button__create("取り消し");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'C' );
  button->x = text->w - button->w -BmGetFontWidth(), button->y = last_y;
  button->client_data = setup_dialog;
  button->callback = dialog_cancel;
  setup_dialog->append_item( setup_dialog, button );
  last_x = button->x;

  button = Button__create("適用");
  button->set_attr( button, ITEM_SHORTCUT_KEY, (void*)'X' );
  button->client_data = setup_dialog;
  button->x = last_x - button->w - BmGetFontWidth(), button->y = last_y;
  button->callback = setup_confirm_callback;
  setup_dialog->append_item( setup_dialog, button );
}
