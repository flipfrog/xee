//
// skkserv.h: ＳＫＫサーバとの接続を制御する。
//
#ifndef __skkserv_h
#define __skkserv_h

class SkkServ {
  int sfd;
  FILE *rfp, *wfp;
  char hostname[256];
  List<char> local_dict;
public:
  SkkServ( char* hostname );
  ~SkkServ();
  void convert_word( char* in, List<char>* out_slot );
  Boolean complesion_word( char* in, char* out );
};

#endif // __skkserv_h
