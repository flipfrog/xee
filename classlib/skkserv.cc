//
// skkserv.cc: ＳＫＫサーバとの接続を制御する。
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <memory.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <local_types.h>
#include <list.h>
#include <skkserv.h>

#define PORT_NUMBER 1178
#define SERVICENAME "skkserv"

static int check_last_size( char* s );

//
// SkkServ(): コンストラクタ
//
SkkServ::SkkServ( char* hn )
{
  struct sockaddr_in hostaddr;
  struct hostent *entry;
  struct servent *serv;
  struct protoent *proto;
	
  if( (serv = getservbyname( SERVICENAME, "tcp" )) == NULL ){
    serv = new struct servent[1];
    serv->s_port = PORT_NUMBER;
  }
  memset( (char*)&hostaddr, 0, sizeof(struct sockaddr_in) );
  if( (proto = getprotobyname( "tcp" )) == NULL )
    fatal();

  if ((sfd = socket( AF_INET, SOCK_STREAM, proto->p_proto )) < 0)
    fatal();

  if( hn != NULL ){
    strcpy( hostname, hn );
  }else{
    if( (hn = (char*)getenv( "SKKSERVER" )) == NULL )
      fatal();
    strcpy( hostname, hn );
  }

  if( isdigit( hostname[0] ) ){
    int a1, a2, a3, a4;
    if (sscanf( hostname,"%d.%d.%d.%d", &a1, &a2, &a3, &a4 ) != 4)
      fatal();
    unsigned int ip_address = htonl((a1<<12)|(a2<<8)|(a3<<4)|a4);
    memcpy( &(hostaddr.sin_addr), &ip_address, sizeof(ip_address) );
  }else{
    if( (entry = gethostbyname(hostname)) == NULL)
      fatal();
    memcpy( &(hostaddr.sin_addr), entry->h_addr, entry->h_length );
  }
  hostaddr.sin_family = AF_INET;
  hostaddr.sin_port = serv->s_port;
  if(connect(sfd,(struct sockaddr*)&hostaddr,sizeof(struct sockaddr_in)) < 0)
    fatal();
  rfp = fdopen( sfd, "r" );
  wfp = fdopen( sfd, "w" );

  char dict_file[1024];
  sprintf( dict_file, "%s/.skk-jisyo", getenv("HOME") );
  FILE* dfp = fopen( dict_file, "r" );
  if( dfp != NULL ){
    char buf[ 1024*10 ];
    while( fgets( buf, sizeof(buf), dfp ) != NULL ){
      if( strncmp( buf, ";;", 2 ) != 0 ){
//	char* foo = new char[ strlen(buf)+1 ];
//	strcpy( foo, buf );
//	local_dict.append( foo );
	local_dict.append( strdup(buf) );
      }
    }
    fclose( dfp );
  }
}
  
//
// ~SkkServ(): デストラクタ
//
SkkServ::~SkkServ()
{
  if( sfd >= 0 ){
    fprintf( wfp, "0\n" );
    fflush( wfp );
  }
}

//
// convert_word(): 与えられた文字列を元に変換処理を行う。
//
void SkkServ::convert_word( char* in, List<char>* out_slot )
{
  // まずローカル辞書から検索する。
  for( int i = 0 ; i < local_dict.count() ; i++ ){
    char word[1024];
    char* w = local_dict.get(i);
    char* sp = strchr( w, '/' );
    memcpy( word, w, sp-w );
    word[ sp-w-1 ] = 0;
    if( strcmp( word, in ) == 0 ){
      char buf[1024];
      strcpy( buf, sp );
      // '/'で囲まれた文字列を分解して、out_slot に追加してゆく。
      for( int p = 1 ; buf[ p ] != 0 && buf[ p ] != '\n' ; ){
	if( buf[p] == 0x5b ) // '[' です (^^ヾ
	  break;
	for( int m = p ; buf[ m ] != '/' ; m++ )
	  ;
	buf[ m ] = 0;
//	char* foo = new char[ strlen(&buf[p])+1 ];
//	strcpy( foo, &buf[p] );
//	out_slot->append( foo );
	out_slot->append( strdup(&buf[p]) );
	p += strlen( &buf[ p ] )+1;
      }
      // 見付けた行をリストの先頭に移動させる。
      // これは、ワード完補機能に利用するため。
      if( i > 0 ){
	char* p = local_dict.get(i);
	local_dict.unlink( LIST_TOP, i, 0 );
	local_dict.insert( LIST_TOP, 0, p );
      }
      break; // return; とすると、ローカル辞書の内容しか反映されない。
    }
  }

  // 次に、サーバと通信を行う。
  fprintf( wfp, "1%s \n", in );
  fflush( wfp );
  char r;
  read( sfd, &r , 1 );
  if( r == '1' ){ // 変換成功。
    char buf[1024*10];
    fgets( buf, sizeof(buf), rfp );
    // '/'で囲まれた文字列を分解して、out_slot に追加してゆく。
    for( int p = 1 ; buf[ p ] != 0 && buf[ p ] != '\n' ; ){
      for( int m = p ; buf[ m ] != '/' ; m++ )
	;
      buf[ m ] = 0;
//      char* foo = new char[ strlen( &buf[p] )+1 ];
//      strcpy( foo, &buf[p] );
//      out_slot->append( foo );
      out_slot->append( strdup(&buf[p]) );
      p += strlen( &buf[ p ] )+1;
    }
  }else while( read( sfd, &r, 1 ) > 0 && r != '\n') // 変換失敗。
    ;
}

//
// 与えられた文字列を完補する文字を出力する。
//
Boolean SkkServ::complesion_word( char* in, char* out )
{
  for( int i = 0 ; i < local_dict.count() ; i++ ){
    char* p = local_dict.get(i);
    if( strncmp( p, in, strlen(in) ) == 0 ){
      char* ep = strchr( p, ' ' );
      memcpy( out, p, ep-p );
      out[ ep-p ] = 0;
      if( check_last_size(out) == 1 )
	out[ ep-p-1 ] = 0;
      return 1;
    }
  }
  return 0;
}

//
// check_last_size(): 文字列を調べて最後の文字のバイト数を返す。
//
static int check_last_size( char* s )
{
  Boolean c = 1;
  for( int i = 0 ; s[i] != 0 ; ){
    if( (unsigned char)s[i] & 0x80 )
      c = 2;
    else
      c = 1;
    i += c;
  }
  return c;
}
