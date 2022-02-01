/******************************************************************************

  list.h: 汎用線型リストクラス＋それを利用したスタッククラス

*/
#ifndef __list_h
#define __list_h

#include <stdlib.h>

//
// リストの操作を行うときに利用する、原点識別子。
//
typedef enum{
  LIST_TOP, LIST_END,
} ListOrigin;

//
// リストの接続状態を表現する識別子。
//
typedef enum{
  LIST_REAL, LIST_VIRTUAL,
} LinkState;

//
// クラス定義
//
template <class T> class List{
  T** slot;                 // 管理オブジェクトポインタの配列。
  int num_slot;             // 上記配列の要素数。
  LinkState virtual_link_f; // 仮想リンクフラグ。

public:
       List      ();
       List      ( List& x );
       ~List     ();
  void append    ( T* x );
  void insert    ( ListOrigin org, int offt, T* x );
  T*   find      ( int (*compar)( T* x ) );
  T*   get       ( int x );
  void sort      ( int (*compar)(T** d1, T** d2));
  void unlink    ( ListOrigin org, int offt, int count );
  int  count     ();
  T*&  operator[]( int x );
  void operator+=( List& x );
  void operator= ( List& x );
  void set_virtual_link( LinkState ls );
};

//
// List(): コンストラクタ(1)
//         通常の生成を行う。
//
template <class T> List<T>::List()
{
  slot = NULL;
  num_slot = 0;
  virtual_link_f = LIST_VIRTUAL;
}

//
// List(): コンストラクタ(2)
//         与えられたオブジェクトの複製を新規に作成する。
//         このとき、仮想リンクフラグ(virtual_link_f)をLIST_VIRTUAL
//         にすることに注意。
//
template <class T> List<T>::List( List& x )
{
  slot = NULL;
  num_slot = 0;
  virtual_link_f = LIST_VIRTUAL;
  for( int i = 0 ; i < x.count() ; i++ )
    append( x[i] );
}

//
// ~List(): デストラクタ
//          仮想リンクのとき、管理オブジェクトは削除されない。
//
template <class T> List<T>::~List()
{
  if( virtual_link_f == LIST_REAL ){
    for( int i = 0 ; i < num_slot ; i++ )
      delete slot[i];
  }
  free( slot );
}

//
// append(): リスト最後尾にオブジェクトを追加する。
//
template <class T> void List<T>::append( T* x )
{
  T** tmp_slot;
  if( (tmp_slot = (T**)malloc( sizeof(T*)*(num_slot+1) )) == NULL )
    abort();
  memcpy( tmp_slot, slot, sizeof(T*)*num_slot );
  tmp_slot[ num_slot ] = x;
  free( slot );
  slot = tmp_slot;
  num_slot++;
}

//
// insert(): 指定された位置にオブジェクトを格納する。
//           指定した位置に管理オブジェクトが存在している場合、もとの管理
//           オブジェクトは、新しく挿入されるオブジェクトの直後に移動する。
//
template <class T> void List<T>::insert( ListOrigin org, int offt, T* x )
{
  T** tmp_slot;
  if( (tmp_slot = (T**)malloc( sizeof(T*)*(++num_slot) )) == NULL )
    abort();
  if( num_slot == 1 ){
    slot = tmp_slot;
    slot[ 0 ] = x;
  }else{
    int pos;
    switch( org ){
    case LIST_TOP: pos = offt; break;
    case LIST_END: pos = num_slot-1-offt; break;
    }
    if( pos < 0 || pos >= num_slot )
      abort();
    memcpy( tmp_slot, slot, sizeof(T*)*(num_slot-1) );
    free( slot );
    slot = tmp_slot;
    for( int i = num_slot-1 ; i > pos ; i-- )
      slot[ i ] = slot[ i-1 ];
    slot[ pos ] = x;
  }
}
    
//
// operator[]: オブジェクトを取出す。
//
template <class T> T*& List<T>::operator[]( int x )
{
  if( x < 0 || x >= num_slot )
    abort();
  return slot[x];
}

//
// find(): 条件に適合するオブジェクトを見付ける。
//         比較関数は、与えられたオブジェクトが適合するとき、真を返す。
//
template <class T> T* List<T>::find( int (*compare)( T* x ) )
{
  for( int i = 0 ; i < num_slot ; i++ )
    if( compare( slot[i] ) == 0 )
      return slot[i];
  return NULL;
}

//
// get(): オブジェクトのアドレスを取出す。
//
template <class T> T* List<T>::get( int x )
{
  if( x < 0 || x >= num_slot )
    abort();
  return slot[x];
}

//
// sort(): オブジェクトを与えられた評価関数を使って整列させる。
//         比較関数は、与えられた２つのオブジェクトを比較し、その論理的な
//         差異を整数値に変換して返す。同一のときは、０を返す。
//
template <class T> void List<T>::sort( int (*compare)( T** d1, T** d2 ) )
{
  if( num_slot <= 0 )
    return;
  qsort( slot, num_slot, sizeof(T*),
	(int (*)(const void*, const void*))compare );
}

//
// unlink(): 任意の位置のオブジェクトを削除する。
//           仮想リンクのときは、管理オブジェクトを削除しないで、配列領域だけ
//           を削除する。
//
template <class T> void List<T>::unlink( ListOrigin org, int offt, int count )
{
  if( count == 0 )
    return;
  if( count < 0 )
    abort();
  int pos = (org==LIST_TOP)?offt:num_slot-1+offt;
  if( num_slot <= 0 || pos+count-1 >= num_slot || pos < 0 )
    abort();
  if( num_slot-count == 0 ){
    if( virtual_link_f == LIST_REAL )
      delete slot[pos];
    free( slot );
    slot = NULL;
    num_slot = 0;
  }else{
    int i;
    T** tmp_slot;
    if( virtual_link_f == LIST_REAL ){
      for( i = 0 ; i < count ; i++ )
	delete slot[ pos+i ];
    }
    for( i = pos+count ; i < num_slot ; i++ )
      slot[ i-count ] = slot[ i ];
    num_slot -= count;
    if( (tmp_slot = (T**)malloc( sizeof(T*)*num_slot )) == NULL )
      abort();
    memcpy( tmp_slot, slot, sizeof(T*)*num_slot );
    free( slot );
    slot = tmp_slot;
  }
}

//
// count(): 格納しているオブジェクトの数を返す。
//
template <class T> int List<T>::count()
{
  return num_slot;
}

//
// set_virtual_link(): 仮想リンクの設定を行う。
//
template <class T> void List<T>::set_virtual_link( LinkState sw )
{
  virtual_link_f = sw;
}

//
// operator+=(): オブジェクトのマージをする。
//
template <class T> void List<T>::operator+=( List& x )
{
  for( int i = 0 ; i < x.count() ; i++ )
    append( x[i] );
}

//
// operator=(): オブジェクトの複製を作る。
//              既に管理していたオブジェクトは、先に削除される。
//              また、List<T>のＴは、同じ型でなければならない。
//
template <class T> void List<T>::operator=( List& x )
{
  if( count() > 0 )
    unlink( LIST_TOP, 0, count() );
  for( int i = 0 ; i < x.count() ; i++ )
    append( x[i] );
}

//
// stakc.h: スタッククラス：リストクラスの導出クラス
//          データ取出し後にそれを利用するので、デフォールトで仮想リンクに
//          設定される。
//
template<class T> class Stack: public List<T> {
public:
  Stack(){ set_virtual_link(LIST_VIRTUAL); }
  void push( T* data ){ append( data ); }
  T* pop(){ T* p = get( count()-1 ); unlink( LIST_END, 0, 1 ); return p; }
  void swap_top()
    {
      T *save_first = pop(), *save_secound = pop();
      push( save_first );
      push( save_secound );
    }
};

#endif __list_h
