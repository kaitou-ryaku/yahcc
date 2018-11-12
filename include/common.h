#ifndef __C_COMPILER_COMMON__
#define __C_COMPILER_COMMON__

#include <stdbool.h>

typedef struct {
  int id;
  int total_size;
  int used_size;
  int bnf_id;
  int token_id; // bnf_idがstructかtypedefの場合のみ使用
  int alias_id; // bnf_idがtypedefの場合のみ使用
  int block;
  int byte;  // バイト数
} TYPE;

typedef struct {
  int   id;
  int   total_size;
  int   used_size;
  int   token_id;
  int   kind; // 関数引数、変数、プロトタイプ等を表すSYMBOL_TABLE_*の値
  int   type;    // int, char, typedef_keywordのtoken_id
  int   storage; // static, externのtoken_id
  int   qualify; // const, volatileのtoken_id
  int   block; // ローカル変数なら属する{}, 関数定義の引数なら関数の{}, 関数プロトタイプの引数なら0
  int   addr;
  int*  array;      // int a; -> 無効  int *a; -> 0   int **a; -> 00  int a[2][3]; -> 23
  int   array_size; // int a; -> 0     int *a; -> 1   int **a; -> 2   int a[2][3]; -> 2
  int   total_array_size;
  int   size;    // char:1 int:4
  int   function_id;  // 引数の場合、親関数のidを入れる
  int   argument_id;  // 引数の場合、何番目の引数か入れる
  int   total_argument; // 関数の場合、引数の個数を入れる
} SYMBOL;

typedef struct {
  int   id;
  int   total_size;
  int   used_size;
  int   here;
  int   up;
} BLOCK;

#endif
