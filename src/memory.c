#include "../min-bnf-parser/include/min-bnf-parser.h"
#include "../include/common.h"
#include "../include/memory.h"
#include "../include/symbol.h"
#include "../include/type.h"
#include "../include/pt_common.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

static const int ADDRESS_BYTE=8;
static void register_typedef_size(const int type_index, TYPE* type);
static void register_symbol_size(const int symbol_index, const LEX_TOKEN* token, const BNF* bnf, const PARSE_TREE* pt, const TYPE* type, SYMBOL* symbol);

extern int sizeof_symbol_array(const int byte, const int* array, const int array_size) {/*{{{*/
  int ret;

  // 型そのもの
  if (array_size == 0) {
    ret=byte;

  // 配列かポインタ
  } else if (array_size > 0){
    ret=1;
    for (int i=0; i<array_size; i++) {
      const int index=array_size-i-1;
      // ポインタの場合
      if (array[index] == 0) {
        ret=ret*ADDRESS_BYTE;
        break;

      // 配列の場合
      } else if (array[index] > 0){
        ret=ret*array[index];
        if (index == 0) {
          ret=ret*byte;
          break;
        }

      } else {
        assert(0);
      }
    }

  } else {
    assert(0);
  }

  return ret;
}/*}}}*/
extern void register_type_and_symbol_size(/*{{{*/
  const BLOCK* block
  , const LEX_TOKEN* token
  , const BNF* bnf
  , const PARSE_TREE* pt
  , TYPE* type
  , SYMBOL* symbol
) {
  // ソースを上から下までたどり、各変数とtypedefの型を決定
  for (int token_index=0; token_index<token[0].used_size; token_index++) {
    const int type_index = search_type_table_by_declare_token(token_index, bnf, type);
    const int symbol_index = search_symbol_table_by_declare_token(token_index, symbol);
    assert((type_index < 0) || (symbol_index < 0));
    if ((type_index < 0) && (symbol_index < 0)) continue;

    if (type_index >= 0)  {
      if (type[type_index].byte >= 0) continue;
      const char* bnf_name = bnf[type[type_index].bnf_id].name;

      // TODO typedef int hoge;のみ対応。typedef int* hoge;や配列には未対応
      if (strcmp("typedef", bnf_name)==0) {
        register_typedef_size(type_index, type);

      } else if (strcmp("struct", bnf_name)==0) {

      } else {
        assert(0);
      }

    } else if (symbol_index >= 0) {
      if (symbol[symbol_index].byte >= 0) continue;
      register_symbol_size(symbol_index, token, bnf, pt, type, symbol);

    } else {
      assert(0);
    }
  }
}/*}}}*/
static void register_typedef_size(const int type_index, TYPE* type) {/*{{{*/
  assert(type_index >= 0);
  const int alias_id = type[type_index].alias_id;

  int origin_type_index=0;
  while (origin_type_index < type[0].used_size) {
    if (alias_id == origin_type_index) {
      type[type_index].byte = type[origin_type_index].byte;
      break;
    }
    origin_type_index++;
  }
  assert(origin_type_index < type[0].used_size);
}/*}}}*/
static void register_symbol_size(const int symbol_index, const LEX_TOKEN* token, const BNF* bnf, const PARSE_TREE* pt, const TYPE* type, SYMBOL* symbol) {/*{{{*/
  assert(symbol[symbol_index].type >= 0);

  if (0==strcmp("typedef_name", bnf[pt[symbol[symbol_index].type].bnf_id].name)) {
    const int symbol_token_id = pt[symbol[symbol_index].type].token_begin_index;

    int origin_type_index=0;
    while (origin_type_index < type[0].used_size) {
      const int origin_token_id = type[origin_type_index].token_id;
      if (is_same_token_str(symbol_token_id, origin_token_id, token)) {
        const int byte = type[origin_type_index].byte;
        symbol[symbol_index].original_byte = byte;
        symbol[symbol_index].byte = sizeof_symbol_array(byte, symbol[symbol_index].array, symbol[symbol_index].array_size);
        break;
      }
      origin_type_index++;
    }
    assert(origin_type_index < type[0].used_size);

  } else {
    int origin_type_index=0;
    while (origin_type_index < type[0].used_size) {
      if (pt[symbol[symbol_index].type].bnf_id == type[origin_type_index].bnf_id) {
        const int byte = type[origin_type_index].byte;
        symbol[symbol_index].original_byte = byte;
        symbol[symbol_index].byte = sizeof_symbol_array(byte, symbol[symbol_index].array, symbol[symbol_index].array_size);
        break;
      }
      origin_type_index++;
    }
    assert(origin_type_index < type[0].used_size);
  }
}/*}}}*/
