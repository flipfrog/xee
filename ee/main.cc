//
// ee [-batch <tex|??>] <file>...
//
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <local_types.h>
#include <list.h>

#include <geometry.h>

#include <eedefs.h>
#include <xfontpair.h>
#include <widget.h>
#include <scrollbar.h>
#include <canvas.h>
#include <panel.h>
#include <frame.h>
#include <skkserv.h>
#include <skkfep.h>
#include <text.h>

#include <shape.h>
#include <shapeset.h>
#include <board.h>
#include <xcontext.h>

#include <misc.h>

Xcontext* xcontext;

extern int major_version;
extern int minor_version;
extern char* mdate;

/*****************************************************************************

 main():

*/
main( int ac, char** av )
{
  struct stat stbuf;
#if 0
  if( strcmp( av[1], "-help" ) == 0 ){
    fprintf( stderr, "*** %s V%d.%d (%s) ***\n",
	    av[0], major_version, minor_version, mdate );
    fprintf( stderr, "    ee [-batch <tex>|<fig>] [file] ...\n" );
    fprintf( stderr, "    -batch tex ... *.epc -> *.tex 変換\n" );
    fprintf( stderr, "    -batch fig ... *.epc -> *.fig 変換\n" );
    exit(0);
  }

  if( ac >= 4 && stricmp( av[1], "-batch" ) == 0 ){
    Boolean tex_batch = False;
    Boolean fig_batch = False;
    board->batch_mode = True;
    if( stricmp( av[2], "tex" ) == 0 )
      tex_batch = True;
    if( stricmp( av[2], "fig" ) == 0 )
      fig_batch = True;
    for( i = 3 ; i < ac ; i++ ){
      char buf[128];
      if( stat( av[i], &stbuf ) == -1 ){
	fprintf( stderr, "'%s' がオープンできませんでした。\n", av[i] );
	continue;
      }
      strcpy( buf, av[i] );
      if( strchr( buf, '.' ) != 0 )
	*strchr( buf, '.' ) = 0;
      strcat( buf, ".epc" );
      board->load( board, buf );
      if( tex_batch ){
	strcpy( buf, av[i] );
	if( strchr( buf, '.' ) != 0 )
	  *strchr( buf, '.' ) = 0;
	strcat( buf, ".tex" );
	fprintf( stderr, "TeX 変換中 '%s' -> '%s' ...", av[i], buf );
	if( board->tex( board, buf ) )
	  fprintf( stderr, " 完了\n" );
      }else if( fig_batch ){
	strcpy( buf, av[i] );
	if( strchr( buf, '.' ) != 0 )
	  *strchr( buf, '.' ) = 0;
	strcat( buf, ".fig" );
	fprintf( stderr, "Fig 変換中 '%s' -> '%s' ...", av[i], buf );
	if( board->xfig( board, buf ) )
	  fprintf( stderr, " 完了\n" );
      }else{
	strcpy( buf, av[i] );
	if( strchr( buf, '.' ) != 0 )
	  *strchr( buf, '.' ) = 0;
	strcat( buf, ".epc" );
	fprintf( stderr, "V1.X -> V2.0 convert '%s' ...", buf );
	board->save( board, buf );
	fprintf( stderr, " complete.\n" );
      }
      board->alldel( board );
    }
    exit(0);
  }
#endif // 0

  char* dsp_name = "unix:0";
  if( getenv("DISPLAY") != NULL )
    dsp_name = getenv("DISPLAY");
  xcontext = new Xcontext(dsp_name);
  xcontext->connect_skkserv( getenv("SKKSERVER" ) );
  Board* board = new Board( xcontext );

  for( int i = 1 ; i < ac ; i++ ){
    char buf[ 128 ];
    strcpy( buf, av[i] );
    if( strchr( buf, '.' ) == NULL )
      strcat( buf, ".epc" );
    if( stat( buf, &stbuf ) != -1 )
      board->load( buf );
    strncpy( board->last_epc, buf, sizeof(board->last_epc) );
    board->last_epc[ sizeof(board->last_epc)-1 ] = 0;
  }
  TextWidget* text =
    (TextWidget*)(board->loadsave_dialog->search_by_name_widget("file-name"));
  text->set_string( board->last_epc );

  xcontext->append_frame( board->frame );
  xcontext->do_main_loop();
}
