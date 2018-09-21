// Automatically generated file. Don't edit!
#include "refalrts.h"


extern enum r05_fnresult r05c_Success(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Fails(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_True(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_False(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Add(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Sub(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Mul(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Div(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Mod(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_WriteLine(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_FWriteLine(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_ReadLine(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_FReadLine(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_FOpen(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_FClose(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Arg(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_ExistFile(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_GetEnv(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Exit(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_System(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_IntFromStr(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_StrFromInt(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Chr(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Ord(struct r05_node *arg_begin, struct r05_node *arg_end);
extern enum r05_fnresult r05c_Compare(struct r05_node *arg_begin, struct r05_node *arg_end);

#line 2 "..\\lib/Library.ref"
#include <stdio.h>
#include <string.h>
#include <vector>
#include <ctype.h>
#include <stdlib.h>

//FROM refalrts

//------------------------------------------------------------------------------

// Операции распределителя памяти

namespace refalrts{

namespace allocator {

bool alloc_node(struct r05_node *&node);

struct r05_node *free_ptr();

} // namespace allocator

} // namespace refalrts
#line 56 "..\\lib/Library.cpp"
enum r05_fnresult r05c_Success(struct r05_node *, struct r05_node *) {
  return (enum r05_fnresult)(
    R05_RECOGNITION_IMPOSSIBLE | (__LINE__ << 8)
  );
}

enum r05_fnresult r05c_Fails(struct r05_node *, struct r05_node *) {
  return (enum r05_fnresult)(
    R05_RECOGNITION_IMPOSSIBLE | (__LINE__ << 8)
  );
}

enum r05_fnresult r05c_True(struct r05_node *, struct r05_node *) {
  return (enum r05_fnresult)(
    R05_RECOGNITION_IMPOSSIBLE | (__LINE__ << 8)
  );
}

enum r05_fnresult r05c_False(struct r05_node *, struct r05_node *) {
  return (enum r05_fnresult)(
    R05_RECOGNITION_IMPOSSIBLE | (__LINE__ << 8)
  );
}

#line 40 "..\\lib/Library.ref"

#define ARITHM_OP(op, check) \
  struct r05_node *func_name = arg_begin->next; \
  \
  struct r05_node *sX = func_name->next; \
  if (sX->tag != R05_DATATAG_NUMBER) { \
    return R05_RECOGNITION_IMPOSSIBLE; \
  } \
  \
  struct r05_node *sY = sX->next; \
  if (sY->tag != R05_DATATAG_NUMBER) { \
    return R05_RECOGNITION_IMPOSSIBLE; \
  } \
  \
  if (sY->next != arg_end) { \
    return R05_RECOGNITION_IMPOSSIBLE; \
  } \
  \
  check \
  \
  sX->info.number = sX->info.number op sY->info.number; \
  \
  refalrts::splice_to_freelist(arg_begin, func_name); \
  refalrts::splice_to_freelist(sY, arg_end); \
  \
  return R05_SUCCESS;

#define NO_CHECK
#define CHECK_ZERODIV \
  if (sY->info.number == 0) { \
    return R05_RECOGNITION_IMPOSSIBLE; \
  }

#line 115 "..\\lib/Library.cpp"
enum r05_fnresult r05c_Add(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 78 "..\\lib/Library.ref"
  ARITHM_OP(+, NO_CHECK)
#line 119 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Sub(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 84 "..\\lib/Library.ref"
  ARITHM_OP(-, NO_CHECK)
#line 126 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Mul(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 90 "..\\lib/Library.ref"
  ARITHM_OP(*, NO_CHECK)
#line 133 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Div(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 96 "..\\lib/Library.ref"
  ARITHM_OP(/, CHECK_ZERODIV);
#line 140 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Mod(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 102 "..\\lib/Library.ref"
  ARITHM_OP(%, CHECK_ZERODIV);
#line 147 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

#line 111 "..\\lib/Library.ref"
enum r05_fnresult write_to_stream(
  FILE *out, struct r05_node *str_begin, struct r05_node *str_end
) {
  if (ferror(out)) {
    return R05_RECOGNITION_IMPOSSIBLE;
  }

  int printf_res;

  for (
    struct r05_node *p = str_begin;
    ! r05_empty_seq(p, str_end);
    r05_move_left(&p, &str_end)
  ) {
    switch(p->tag) {
      case R05_DATATAG_CHAR: {
        printf_res = fprintf(out, "%c", p->info.char_);
        if (printf_res < 0) {
          return R05_RECOGNITION_IMPOSSIBLE;
        } else {
          break;
        }
      }

      case R05_DATATAG_NUMBER: {
        printf_res = fprintf(out, "%lu ", p->info.number);
        if (printf_res < 0) {
          return R05_RECOGNITION_IMPOSSIBLE;
        } else {
          break;
        }
      }

      case R05_DATATAG_FUNCTION: {
        if (p->info.function.name[0] != '\0') {
          printf_res = fprintf(out, "%s ", p->info.function.name);
        } else {
          printf_res = fprintf(out, "&%p ", p->info.function.ptr);
        }

        if (printf_res < 0) {
          return R05_RECOGNITION_IMPOSSIBLE;
        } else {
          break;
        }
      }

      case R05_DATATAG_OPEN_BRACKET: {
        printf_res = fprintf(out, "(");
        if (printf_res < 0) {
          return R05_RECOGNITION_IMPOSSIBLE;
        } else {
          break;
        }
      }

      case R05_DATATAG_CLOSE_BRACKET: {
        printf_res = fprintf(out, ")");
        if (printf_res < 0) {
          return R05_RECOGNITION_IMPOSSIBLE;
        } else {
          break;
        }
      }

      case R05_DATATAG_FILE: {
        printf_res = fprintf(out, "*%p", p->info.file);
        if (printf_res < 0) {
          return R05_RECOGNITION_IMPOSSIBLE;
        } else {
          break;
        }
      }

      default:
        r05_switch_default_violation(p->tag);
    }
  }

  printf_res = fprintf(out, "\n");
  if (printf_res < 0) {
    return R05_RECOGNITION_IMPOSSIBLE;
  } else {
    return R05_SUCCESS;
  }
}
#line 238 "..\\lib/Library.cpp"
enum r05_fnresult r05c_WriteLine(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 202 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *eLine_b_1;
    struct r05_node *eLine_e_1;
    eLine_b_1 = bb_0;
    eLine_e_1 = be_0;

    enum r05_fnresult fnres = write_to_stream(stdout, eLine_b_1, eLine_e_1);

    if (fnres != R05_SUCCESS)
      return fnres;

    refalrts::reset_allocator();
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return fnres;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 262 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_FWriteLine(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 228 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sFileHandle_1;
    struct r05_node *eLine_b_1;
    struct r05_node *eLine_e_1;
    // s.FileHandle e.Line
    if (! refalrts::svar_left(sFileHandle_1, bb_0, be_0))
      break;
    if (sFileHandle_1->tag != R05_DATATAG_FILE)
      break;
    eLine_b_1 = bb_0;
    eLine_e_1 = be_0;

    enum r05_fnresult write_result =
      write_to_stream(
        static_cast<FILE*>(sFileHandle_1->info.file), eLine_b_1, eLine_e_1
      );

    if (write_result != R05_SUCCESS)
      return write_result;

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;
    refalrts::splice_stvar(res, sFileHandle_1);
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 300 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

#line 264 "..\\lib/Library.ref"
enum r05_fnresult read_from_stream(
  FILE *input, struct r05_node *&begin, struct r05_node *&end
) {
  struct r05_node *before_begin = refalrts::allocator::free_ptr()->prev;
  struct r05_node *cur_char_node = 0;

  int cur_char;

  for ( ; ; ) {
    cur_char = getc(input);
    if (EOF == cur_char) {
      if (! refalrts::alloc_number(cur_char_node, 0UL)) {
        return R05_NO_MEMORY;
      }
      break;
    } else if ('\n' == cur_char) {
      break;
    } else {
      /*
        Пользуемся тем фактом, что в данной реализации размещёные в свободной
        памяти узлы располагаются в последовательных адресах, которые будут
        начинаться с before_begin->next.
      */
      if (! refalrts::alloc_char(cur_char_node, static_cast<char>(cur_char))) {
        return R05_NO_MEMORY;
      }
    }
  }

  if (cur_char_node != 0) {
    begin = before_begin->next;
    end = cur_char_node;
  } else {
    begin = 0;
    end = 0;
  }

  return R05_SUCCESS;
}
#line 344 "..\\lib/Library.cpp"
enum r05_fnresult r05c_ReadLine(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 307 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    if (! r05_empty_seq(bb_0, be_0))
      break;

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;

    struct r05_node *str_begin, *str_end;

    enum r05_fnresult fn_result = read_from_stream(stdin, str_begin, str_end);

    if (fn_result != R05_SUCCESS)
      return fn_result;

    refalrts::splice_evar(res, str_begin, str_end);
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 371 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_FReadLine(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 336 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sFileHandle_1;
    // s.FileHandle
    if (! refalrts::svar_left(sFileHandle_1, bb_0, be_0))
      break;
    if (sFileHandle_1->tag != R05_DATATAG_FILE)
      break;
    if (! r05_empty_seq(bb_0, be_0))
      break;

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;

    struct r05_node *str_begin, *str_end;

    enum r05_fnresult fn_result =
      read_from_stream(
        static_cast<FILE*>(sFileHandle_1->info.file), str_begin, str_end
      );

    if (fn_result != R05_SUCCESS)
      return fn_result;

    res = refalrts::splice_evar(res, str_begin, str_end);
    refalrts::splice_stvar(res, sFileHandle_1);
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 411 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

#line 375 "..\\lib/Library.ref"
namespace {

enum r05_fnresult string_from_seq(
  std::vector<char>& string, struct r05_node *begin, struct r05_node *end
) {
  std::vector<char> result;

  enum { cBufLen = 100 };
  char buffer[cBufLen + 1] = { 0 };

  for ( ; ; ) {
    unsigned read = refalrts::read_chars(buffer, cBufLen, begin, end);

    if (read == 0) {
      break;
    }

    // В текущей версии Open Watcom (и форка Open Watcom V2) есть ошибка
    // в функции vector::insert, которая возникает в случае,
    // если во время вставки вектор увеличивает свою ёмкость (capacity).
    // Для обхода этой ошибки предварительно резервируем место.
    result.reserve(result.size() + read + 1);

    result.insert(result.end(), buffer, buffer + read);
  }

  /*
    Здесь r05_empty_seq(begin, end) || (begin->tag != cDataChar).
  */

  if (r05_empty_seq(begin, end)) {
    result.push_back('\0');
    string.swap(result);
    return R05_SUCCESS;
  } else {
    // здесь begin->tag != cDataChar
    return R05_RECOGNITION_IMPOSSIBLE;
  }
}

} // unnamed namespace
#line 457 "..\\lib/Library.cpp"
enum r05_fnresult r05c_FOpen(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 421 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *eFileName_b_1;
    struct r05_node *eFileName_e_1;

    const char *mode;

    if (refalrts::char_left('r', bb_0, be_0)) {
      mode = "r";
    } else if (refalrts::char_left('w', bb_0, be_0)) {
      mode = "w";
    } else {
      break;
    }

    eFileName_b_1 = bb_0;
    eFileName_e_1 = be_0;

    std::vector<char> filename;

    enum r05_fnresult fname_read =
      string_from_seq(filename, eFileName_b_1, eFileName_e_1);

    if (R05_SUCCESS != fname_read)
      return fname_read;

    if (filename.empty())
      return R05_RECOGNITION_IMPOSSIBLE;

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;

    struct r05_node *file_ptr = 0;
    if (! refalrts::allocator::alloc_node(file_ptr))
      return R05_NO_MEMORY;

    file_ptr->tag = R05_DATATAG_FILE;

    if (FILE *f = fopen(&filename[0], mode)) {
      file_ptr->info.file = f;
    } else {
      return R05_RECOGNITION_IMPOSSIBLE;
    }

    refalrts::splice_elem(res, file_ptr);
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 513 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_FClose(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 479 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sFileHandle_1;
    // s.FileHandle
    if (! refalrts::svar_left(sFileHandle_1, bb_0, be_0))
      break;
    if (sFileHandle_1->tag != R05_DATATAG_FILE)
      break;
    if (! r05_empty_seq(bb_0, be_0))
      break;

    int fclose_res = fclose(static_cast<FILE*>(sFileHandle_1->info.file));

    if (EOF == fclose_res) {
      return R05_RECOGNITION_IMPOSSIBLE;
    } else {
      /* Ничего не делаем */;
    }

    refalrts::reset_allocator();
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 547 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

#line 512 "..\\lib/Library.ref"
/*
  Глобальные переменные, хранящие параметры вызова
  (устанавливаются в refalrts.cpp).
*/

extern char **g_argv;
extern int g_argc;
#line 559 "..\\lib/Library.cpp"
enum r05_fnresult r05c_Arg(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 524 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sParamNumber_1;
    // s.ParamNumber
    if (! refalrts::svar_left(sParamNumber_1, bb_0, be_0))
      break;
    if (sParamNumber_1->tag != R05_DATATAG_NUMBER)
      break;
    if (! r05_empty_seq(bb_0, be_0))
      break;

    refalrts::reset_allocator();

    int arg_number = sParamNumber_1->info.number;

    struct r05_node *res_begin = 0;
    struct r05_node *res_end = 0;

    if (arg_number < g_argc) {
      if (! refalrts::alloc_string(res_begin, res_end, g_argv[arg_number]))
        return R05_NO_MEMORY;

      refalrts::splice_evar(arg_begin, res_begin, res_end);
    }

    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 595 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_ExistFile(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 562 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *eFileName_b_1;
    struct r05_node *eFileName_e_1;
    // e.FileName
    eFileName_b_1 = bb_0;
    eFileName_e_1 = be_0;

    std::vector<char> fname;

    enum r05_fnresult fname_res =
      string_from_seq(fname, eFileName_b_1, eFileName_e_1);

    if (fname_res != R05_SUCCESS)
      return fname_res;

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;

    struct r05_node *ans = 0;
    if (FILE *f = fopen(&fname[0], "r")) {
      // Файл существует
      fclose(f);

      if (! refalrts::alloc_name(ans, r05c_True, "True")) {
        return R05_NO_MEMORY;
      }
    } else {
      // Файл по-видимому не существует
      if (! refalrts::alloc_name(ans, r05c_False, "False")) {
        return R05_NO_MEMORY;
      }
    }

    refalrts::splice_elem(res, ans);
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 644 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_GetEnv(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 610 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);
    struct r05_node *eEnvName_b_1;
    struct r05_node *eEnvName_e_1;
    // e.EnvName
    eEnvName_b_1 = bb_0;
    eEnvName_e_1 = be_0;

    std::vector<char> envname;

    enum r05_fnresult envname_res =
      string_from_seq(envname, eEnvName_b_1, eEnvName_e_1);

    if (envname_res != R05_SUCCESS)
      return envname_res;

    refalrts::reset_allocator();

    const char *envres = getenv(& envname[0]);

    if (envres != 0) {
      struct r05_node *env_begin;
      struct r05_node *env_end;

      if (! refalrts::alloc_string(env_begin, env_end, envres))
        return R05_NO_MEMORY;

      refalrts::splice_evar(arg_begin, env_begin, env_end);

      //struct r05_node *char_pos;
      //
      //for (const char *env = envres; *env != '\0'; ++ env) {
      //  if (! refalrts::alloc_char(char_pos, *env))
      //    return R05_NO_MEMORY;
      //
      //  refalrts::splice_elem(res, char_pos);
      //}
    }

    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 696 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Exit(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 661 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sCode_1;
    // s.Code
    if (! refalrts::svar_left(sCode_1, bb_0, be_0))
      break;
    if (sCode_1->tag != R05_DATATAG_NUMBER)
      break;
    if (! r05_empty_seq(bb_0, be_0))
      break;

    refalrts::set_return_code(sCode_1->info.number);

    refalrts::reset_allocator();
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_EXIT;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 724 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_System(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 688 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *eCommand_b_1;
    struct r05_node *eCommand_e_1;
    // e.Command
    eCommand_b_1 = bb_0;
    eCommand_e_1 = be_0;

    std::vector<char> command;

    enum r05_fnresult read_res =
      string_from_seq(command, eCommand_b_1, eCommand_e_1);

    if (read_res != R05_SUCCESS)
      return read_res;

    fflush(stdout);
    fflush(stderr);

    system(&command[0]);

    refalrts::reset_allocator();
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 760 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_IntFromStr(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 725 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *eNumber_b_1;
    struct r05_node *eNumber_e_1;
    // e.NoNumber
    eNumber_b_1 = bb_0;
    eNumber_e_1 = be_0;

    bool start_is_digit =
      ! r05_empty_seq(eNumber_b_1, eNumber_e_1)
      && (R05_DATATAG_CHAR == eNumber_b_1->tag)
      && isdigit(eNumber_b_1->info.char_);

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;

    if (! start_is_digit) {
      struct r05_node *fail_pos = 0;
      if (! refalrts::alloc_name(fail_pos, r05c_Fails, "Fails"))
        return R05_NO_MEMORY;

      res = refalrts::splice_evar(res, eNumber_b_1, eNumber_e_1);
      refalrts::splice_elem(res, fail_pos);
    } else {
      r05_number acc = 0;

      for ( ; ; ) {
        if (r05_empty_seq(eNumber_b_1, eNumber_e_1)) {
          break;
        } else if (eNumber_b_1->tag != R05_DATATAG_CHAR) {
          break;
        } else if (! isdigit(eNumber_b_1->info.char_)) {
          break;
        } else {
          (acc *= 10) += eNumber_b_1->info.char_ - '0';
        }

        r05_move_left(&eNumber_b_1, &eNumber_e_1);
      }

      struct r05_node *success_pos = 0;
      struct r05_node *number_pos = 0;

      if (! refalrts::alloc_name(success_pos, r05c_Success, "Success"))
        return R05_NO_MEMORY;

      if (! refalrts::alloc_number(number_pos, acc))
        return R05_NO_MEMORY;

      res = refalrts::splice_evar(res, eNumber_b_1, eNumber_e_1);
      res = refalrts::splice_elem(res, number_pos);
      refalrts::splice_elem(res, success_pos);
    }

    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 828 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_StrFromInt(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 792 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sNumber_1;
    // s.Number
    if (! refalrts::svar_left(sNumber_1, bb_0, be_0))
      break;
    if (sNumber_1->tag != R05_DATATAG_NUMBER)
      break;
    if (! r05_empty_seq(bb_0, be_0))
      break;

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;

    struct r05_node *char_pos = 0;
    if (r05_number num = sNumber_1->info.number) {
      // Длина десятичного числа = 0,3 * длина двоичного числа,
      // т.к. lg(2) = 0,3. Хрен с ним, что много. Главное, что не мало.
      enum { cMaxNumberLen = 8 * sizeof(r05_number) * 3 / 10 + 2 };

      char buffer[cMaxNumberLen + 1] = { 0 };
      char *lim_digit = buffer + cMaxNumberLen;
      char *cur_digit = lim_digit;

      while (num != 0) {
        -- cur_digit;
        *cur_digit = static_cast<char>((num % 10) + '0');
        num /= 10;
      }

      struct r05_node *num_begin;
      struct r05_node *num_end;
      bool allocated = refalrts::alloc_chars(
        num_begin, num_end, cur_digit, unsigned(lim_digit - cur_digit)
      );

      if (! allocated)
        return R05_NO_MEMORY;

      refalrts::splice_evar(res, num_begin, num_end);
    } else {
      if (! refalrts::alloc_char(char_pos, '0'))
        return R05_NO_MEMORY;

      refalrts::splice_elem(res, char_pos);
    }

    refalrts::splice_to_freelist(arg_begin, arg_end);

    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 890 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Chr(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 853 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sCode_1;
    // s.Code
    if (! refalrts::svar_left(sCode_1, bb_0, be_0))
      break;
    if (sCode_1->tag != R05_DATATAG_NUMBER)
      break;
    if (! r05_empty_seq(bb_0, be_0))
      break;

    char result = static_cast<char>(sCode_1->info.number);

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;
    struct r05_node *n0 = 0;
    if (! refalrts::alloc_char(n0, result))
      return R05_NO_MEMORY;
    refalrts::splice_elem(res, n0);
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 923 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Ord(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 885 "..\\lib/Library.ref"
  do {
    struct r05_node *bb_0 = 0;
    struct r05_node *be_0 = 0;
    r05_prepare_argument(&bb_0, &be_0, arg_begin, arg_end);

    struct r05_node *sChar_1;
    // s.Char
    if (! refalrts::svar_left(sChar_1, bb_0, be_0))
      break;
    if (sChar_1->tag != R05_DATATAG_CHAR)
      break;
    if (! r05_empty_seq(bb_0, be_0))
      break;

    r05_number result = static_cast<unsigned char>(sChar_1->info.char_);

    refalrts::reset_allocator();
    struct r05_node *res = arg_begin;
    struct r05_node *n0 = 0;
    if (! refalrts::alloc_number(n0, result))
      return R05_NO_MEMORY;
    refalrts::splice_elem(res, n0);
    refalrts::splice_to_freelist(arg_begin, arg_end);
    return R05_SUCCESS;
  } while (0);

  return R05_RECOGNITION_IMPOSSIBLE;
#line 956 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}

enum r05_fnresult r05c_Compare(struct r05_node *arg_begin, struct r05_node *arg_end) {
#line 917 "..\\lib/Library.ref"
  struct r05_node *func_name = arg_begin->next;

  struct r05_node *sX = func_name->next;
  if (sX->tag != R05_DATATAG_NUMBER) {
    return R05_RECOGNITION_IMPOSSIBLE;
  }

  struct r05_node *sY = sX->next;
  if (sY->tag != R05_DATATAG_NUMBER) {
    return R05_RECOGNITION_IMPOSSIBLE;
  }

  if (sY->next != arg_end) {
    return R05_RECOGNITION_IMPOSSIBLE;
  }

  sX->info.char_ =
    sX->info.number < sY->info.number ? '-' :
    sX->info.number > sY->info.number ? '+' :
    '0';
  sX->tag = R05_DATATAG_CHAR;

  refalrts::splice_to_freelist(arg_begin, func_name);
  refalrts::splice_to_freelist(sY, arg_end);

  return R05_SUCCESS;
#line 988 "..\\lib/Library.cpp"
  //return R05_RECOGNITION_IMPOSSIBLE;
}


//End of file
