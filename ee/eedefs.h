//
// eedefs.h:
//
#ifndef __eedefs_h
#define __eedefs_h
#define MAJOR_VERSION 2
#define MINOR_VERSION 2

#define NUMBER(x) (sizeof(x)/sizeof(x[0]))
#define ROUND(x) ((int)((x)+0.5))

//#define XFS(x) ((double)(x)*80.0/180)
#define XFS(x) ((double)(x))

//#define eeIPD (BmCheckHighreso()?0.00553:0.00833)
//#define eeDPI (BmCheckHighreso()?1.0/0.00553:1.0/0.00833)

typedef enum {   // 編集処理のステータス。
  EDIT_CONTINUE, // 編集処理を継続する。
  EDIT_COMPLETE, // 編集処理完了。
  EDIT_CANCEL,   // 編集処理中断。
} EditResult;

typedef enum{      // 図形のタイプ。
  ST_SPLINE,       // スプライン曲線
  ST_LINE,         // ポリライン
  ST_RECT,         // 矩形
  ST_OVAL,         // 楕円(真円を含む)
  ST_STRING,       // 文字列
  ST_ARC,          // 円弧
  ST_GROUP,        // 図形グループ
} ShapeType;

typedef enum{      // 編集モード
  EM_INSERT,       // 挿入
  EM_MOVE,         // 移動
  EM_DELETE,       // 削除
  EM_COPY,         // 複写
  EM_RESIZE,       // 拡縮
  EM_REVERSX,      // 左右反転
  EM_REVERSY,      // 上下反転
  EM_UPDATE,       // 更新
  EM_UNGROUP,      // 非グループ化
  EM_ROTATE,       // 右回転
} EditMode;

typedef enum{      // 線の終端モード
  LM_FARROW,       // 順方向矢印
  LM_RARROW,       // 逆方向矢印
  LM_BARROW,       // 双方矢印
  LM_OMASSOC,      // 一対多（ＯＭＴ）
  LM_MMASSOC,      // 多対多（ＯＭＴ）
  LM_SOLID,        // 通常の終端処理
} LineMode;

typedef enum{      // 編集中のカーソルモード
  CM_CROSS,        // クロスカーソル
  CM_BEAM,         // ビームカーソル
  CM_FULL,         // 画面全体のクロス
  CM_NONE          // カーソルの表示をしない。
} CursorMode;

typedef enum{      // 線分のモード
  LS_SOLID,        // 通常線分
  LS_DOT,          // 点線
  LS_DASH,         // 破線
} LineStyle;

typedef enum{     // 文字列のラウンドモード
  RS_NONE,        // ラウンドしない
  RS_RECT,        // 矩形で囲む
  RS_CAPSULE,     // カプセル型で囲む
} RoundStyle;

typedef enum {    // フォントのサイズ(LaTeXで利用できるサイズに制限している)
  FONT_tiny,
  FONT_scriptsize,
  FONT_footnotesize,
  FONT_small,
  FONT_normalsize,
  FONT_large,
  FONT_Large,
  FONT_LARGE,
  FONT_huge,
  FONT_Huge,
} FontSize;

typedef enum {
  RESIZE_LU, RESIZE_RU, RESIZE_LD, RESIZE_RD
  } ResizeDir;

#endif /* __eedefs_h */
