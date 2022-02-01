//
// misc.cc:
//
#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <local_types.h>
#include <eedefs.h>
#include <list.h>

#include <shape.h>
#include <shapeset.h>
#include <board.h>

#include <xfontpair.h>
#include <widget.h>
#include <message.h>
#include <button.h>
#include <panel.h>
#include <frame.h>
#include <icon.h>

#include <misc.h>

#include <xcontext.h>
extern Xcontext* xcontext;

#include <bitmaps/shadow_dont_know.xbm>
#include <bitmaps/tabby.xbm>

#define BW 8
#define PAI 3.1415926
#define PAI_2 (PAI*2)
#define RADIAN(x) (((double)(x)/360.0)*PAI_2)

static Boolean yes_flag;
static int select_no;

static struct _font_pitch_table{
  FontSize size;
  int point;
  double height, ank, kanji;
  char* label;
} font_pitch_table[] = {
{FONT_tiny,         5,  1.5/25.4, 1.33421/25.4, 1.70833/25.4, "tiny"},
{FONT_scriptsize,   7,  2.5/25.4, 1.56579/25.4, 2.37500/25.4, "scriptsize"},
{FONT_footnotesize, 8,  3.0/25.4, 1.50000/25.4, 2.70833/25.4, "footnotesize"},
{FONT_small,        9,  3.0/25.4, 1.67105/25.4, 3.04167/25.4, "small"},
{FONT_normalsize,   10, 3.0/25.4, 1.85526/25.4, 3.33750/25.4, "normalsize"},
{FONT_large,        12, 4.0/25.4, 2.17105/25.4, 4.04167/25.4, "large"},
{FONT_Large,        14, 4.5/25.4, 2.92105/25.4, 4.83330/25.4, "Large"},
{FONT_LARGE,        17, 5.5/25.4, 3.68421/25.4, 5.81250/25.4, "LARGE"},
{FONT_huge,         20, 7.0/25.4, 4.34211/25.4, 7.00000/25.4, "huge"},
{FONT_Huge,         25, 8.0/25.4, 4.87142/25.4, 8.21057/25.4, "Huge"}
};

//
// font_size_string():
//
char* font_size_string( FontSize f_size )
{
  for( int i = 0 ; i < Number(font_pitch_table) ; i++ ){
    if( font_pitch_table[i].size == f_size )
      return font_pitch_table[i].label;
  }
  return "normalsize";
}

//
// confirm_callback():
//
static void confirm_callback( Widget* button )
{
  Dialog* dialog = (Dialog*)(button->client_data);
  yes_flag = True;
  dialog->done_dialog_loop();
}

//
// a_callback():
//
static void a_callback( Widget* button )
{
  Dialog* dialog = (Dialog*)(button->client_data);
  yes_flag = True;
  select_no = 1;
  dialog->done_dialog_loop();
}

//
// b_callback():
//
static void b_callback( Widget* button )
{
  Dialog* dialog = (Dialog*)(button->client_data);
  yes_flag = True;
  select_no = 2;
  dialog->done_dialog_loop();
}

//
// cancel_callback():
//
static void cancel_callback( Widget* button )
{
  Dialog* dialog = (Dialog*)(button->client_data);
  yes_flag = False;
  select_no = 0;
  dialog->done_dialog_loop();
}

//
// yesno_box():
//
Boolean yesno_box( Frame* parent, char* str, char* confirm, char* cancel )
{
  Dialog* dialog = new Dialog( parent, "yesno-dialog" );
  PanelWidget* panel = new PanelWidget( dialog, "panel" );
  dialog->set_panel( panel );
  panel->set_3d_edge();

  IconWidget* icon = new IconWidget( panel, "shadow_dont_know" );
  icon->set_bitmap( shadow_dont_know_bits,
		   shadow_dont_know_width, shadow_dont_know_height );
  icon->move( 10, 10 );
  panel->append_widget( icon );

  MessageWidget* msg = new MessageWidget( panel, "message" );
  msg->move( icon->x()+icon->w()+8, icon->y() );
  msg->set_column( 32 );
  msg->set_label( str );
  panel->append_widget( msg );

  ButtonWidget* button1 = new ButtonWidget( panel, "ok-button" );
  button1->set_label( confirm );
  button1->move( msg->x(), msg->y()+msg->h()+16 );
  button1->client_data = dialog;
  button1->callback = confirm_callback;
  panel->append_widget( button1 );

  ButtonWidget* button2 = new ButtonWidget( panel, "cancel-button" );
  button2->set_label( cancel );
  button2->move( button1->x()+button1->w()+10, button1->y() );
  button2->client_data = dialog;
  button2->callback = cancel_callback;
  panel->append_widget( button2 );
  panel->resize( panel->w()+10, panel->h()+10 );

  yes_flag = True;
  xcontext->do_dialog_loop( dialog, 100, 100 );
  delete dialog;
  return yes_flag;
}  

//
// selectABC_box():
//
int selectABC_box( Frame* parent, char* str, char* a, char* b, char* cancel )
{
  Dialog* dialog = new Dialog( parent, "selectABC-dialog" );
  PanelWidget* panel = new PanelWidget( dialog, "panel" );
  dialog->set_panel( panel );
  panel->set_3d_edge();

  IconWidget* icon = new IconWidget( panel, "shadow_dont_know" );
  icon->set_bitmap( shadow_dont_know_bits,
		   shadow_dont_know_width, shadow_dont_know_height );
  icon->move( 10, 10 );
  panel->append_widget( icon );

  MessageWidget* msg = new MessageWidget( panel, "message" );
  msg->move( icon->x()+icon->w()+8, icon->y() );
  msg->set_column( 40 );
  msg->set_label( str );
  panel->append_widget( msg );

  ButtonWidget* button1 = new ButtonWidget( panel, "a-button" );
  button1->set_label( a );
  button1->move( msg->x(), msg->y()+msg->h()+16 );
  button1->client_data = dialog;
  button1->callback = a_callback;
  panel->append_widget( button1 );

  ButtonWidget* button2 = new ButtonWidget( panel, "b-button" );
  button2->set_label( b );
  button2->move( button1->x()+button1->w()+10, button1->y() );
  button2->client_data = dialog;
  button2->callback = b_callback;
  panel->append_widget( button2 );

  ButtonWidget* button3 = new ButtonWidget( panel, "cancel-button" );
  button3->set_label( cancel );
  button3->move( button2->x()+button2->w()+10, button2->y() );
  button3->client_data = dialog;
  button3->callback = cancel_callback;
  panel->append_widget( button3 );
  panel->resize( panel->w()+10, panel->h()+10 );

  xcontext->do_dialog_loop( dialog, 100, 100 );
  delete dialog;
  return select_no;
}  

//
// message_box():
//
void message_box( Frame* parent, char* str, char* confirm )
{
  Dialog* dialog = new Dialog( parent, "message-dialog" );
  PanelWidget* panel = new PanelWidget( dialog, "panel" );
  dialog->set_panel( panel );
  panel->set_3d_edge();

  IconWidget* icon = new IconWidget( panel, "icon" );
  icon->set_bitmap( tabby_bits, tabby_width, tabby_height );
  icon->move( 10, 10 );
  panel->append_widget( icon );

  MessageWidget* msg = new MessageWidget( panel, "message" );
  msg->move( icon->x()+icon->w()+8, icon->y() );
  msg->set_column( 30 );
  msg->set_label( str );
  panel->append_widget( msg );

  ButtonWidget* button = new ButtonWidget( panel, "confirm-button" );
  button->set_label( confirm );
  button->move( (msg->w()-button->w())/2+msg->x(), msg->y()+msg->h()+16 );
  button->client_data = dialog;
  button->callback = confirm_callback;
  panel->append_widget( button );

  panel->resize( panel->w()+10, panel->h()+10 );

  xcontext->do_dialog_loop( dialog, 100, 100 );
  delete dialog;
}  

//
// tex_ball():
//
void
tex_ball( FILE* fp, double h, double ox, double oy, double angle, double r )
{
  double x, y;
  x = r/2 * cos(angle+PAI) + ox;
  y = r/2 * sin(angle+PAI) + oy;
  fprintf( fp, "\\put(%g,%g){\\ellipse*{%g}{%g}}\n", x, h-y, r, r );
}

//
// tex_arrow():
//
void
tex_arrow( FILE* fp, double h, double ox, double oy, double angle,
	  double r, double t )
{
  double x = r * cos(angle+RADIAN(160)) + ox;
  double y = r * sin(angle+RADIAN(160)) + oy;
  fprintf( fp, "\\path(%g,%g)(%g,%g)", x, h-y, ox, h-oy );
  x = r * cos(angle+RADIAN(-160)) + ox;
  y = r * sin(angle+RADIAN(-160)) + oy;
  fprintf( fp, "(%f,%f)\n", x, h-y );
}

//
// draw_arrow():
//
void draw_arrow( Display* dpy, Window window, GC gc,
		double ox, double oy, double angle, double r  )
{
  int x = ROUND( r * cos(angle+RADIAN(160))+ox);
  int y = ROUND( r * sin(angle+RADIAN(160))+oy);
  XDrawLine( dpy, window, gc, x, y, ROUND(ox), ROUND(oy) );
  x = ROUND( r * cos(angle+RADIAN(-160))+ox);
  y = ROUND( r * sin(angle+RADIAN(-160))+oy);
  XDrawLine( dpy, window, gc, x, y, ROUND(ox), ROUND(oy) );
}

//
// draw_ball():
//
void draw_ball( Display* dpy, Window window, GC gc,
	       double ox, double oy, double angle, double r )
{
  int x = ROUND(r * cos(angle+PAI)-r+ox);
  int y = ROUND(r * sin(angle+PAI)-r+oy);
  XFillArc( dpy, window, gc, x, y, ROUND(r*2), ROUND(r*2), 0, 360*64 );
}

//
// get_font_point():
//
int get_font_point( FontSize f_size )
{
  for( int i = 0 ; i < NUMBER(font_pitch_table) ; i++ ){
    if( font_pitch_table[i].size == f_size )
      return font_pitch_table[i].point;
  }
  return -1;
}

//
// lm_string():
//
char* lm_string( LineMode lm )
{
  switch( lm ){
  case LM_SOLID:
    return "solid";
    break;
  case LM_FARROW:
    return "farrow";
    break;
  case LM_RARROW:
    return "rarrow";
    break;
  case LM_BARROW:
    return "barrow";
    break;
  case LM_MMASSOC:
    return "mmassoc";
    break;
  case LM_OMASSOC:
    return "omassoc";
    break;
  }
  return "solid";
}

//
// ls_string():
//
char* ls_string( LineStyle ls )
{
  switch( ls ){
  case LS_SOLID:
    return "solid";
    break;
  case LS_DASH:
    return "dash";
    break;
  case LS_DOT:
    return "dot";
    break;
  }
  return "solid";
}

//
// rs_string():
//
char* rs_string( RoundStyle rs )
{
  switch( rs ){
  case RS_NONE:
    return "none";
    break;
  case RS_RECT:
    return "rect";
    break;
  case RS_CAPSULE:
    return "capsule";
    break;
  }
  return "none";
}

//
// bool_string():
//
char * bool_string( Boolean b )
{
  return b?"true":"false";
}

//
// DrawX(): 画面に×マークを描画する。
//
void DrawX( Display* dpy, Window win, GC gc, int x, int y )
{
  XDrawLine( dpy, win, gc, x-2, y-2, x+2, y+2 );
  XDrawLine( dpy, win, gc, x+2, y-2, x-2, y+2 );
}

//
// draw_cursor():
//
void draw_cursor( Display* dpy, Window w, GC gc, int x, int y, Board* board )
{
  switch( board->cursor_mode ){
  case CM_CROSS:
    XDrawLine( dpy, w, gc, x-8, y, x+8, y );
    XDrawLine( dpy, w, gc, x, y-8, x, y+8 );
    break;
  case CM_FULL:
    XDrawLine( dpy, w, gc, 0, y, board->paper_w, y );
    XDrawLine( dpy, w, gc, x, 0, x, board->paper_h );
    break;
  case CM_BEAM:
    break;
  case CM_NONE:
    break;
  }
}
