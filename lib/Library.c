#include <assert.h>
#include <ctype.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef R05_POSIX
#include <sys/wait.h>
#endif  /* R05_POSIX */

#include "refal05rts.h"


/**
   1. <Mu s.Func e.Arg> == <s.Func e.Arg>
*/
R05_DEFINE_ENTRY_FUNCTION(Mu, "Mu") {
  struct r05_node *mu = state->arg_begin->next;
  struct r05_node *callable = mu->next;
  if (callable->tag != R05_DATATAG_FUNCTION) {
    r05_recognition_impossible(state);
  }

  r05_reuse_aterm(
    state->arg_begin, state->arg_end, aterm, state
  );
  r05_splice_to_freelist(mu, mu, state);
  r05_enqueue_one_aterm(state, aterm);
}


#define ARITHM_PREFIX \
  struct r05_node *func_name, *sX, *sY; \
  func_name = state->arg_begin->next; \
  \
  sX = func_name->next; \
  if (sX->tag != R05_DATATAG_NUMBER) { \
    r05_recognition_impossible(state); \
  } \
  \
  sY = sX->next; \
  if (sY->tag != R05_DATATAG_NUMBER) { \
    r05_recognition_impossible(state); \
  } \
  \
  if (sY->next != state->arg_end) { \
    r05_recognition_impossible(state); \
  }

#define ARITHM_OP(op, check) \
  ARITHM_PREFIX \
  \
  check \
  \
  sX->info.number = sX->info.number op sY->info.number; \
  \
  r05_splice_to_freelist(state->arg_begin, func_name, state); \
  r05_splice_to_freelist(sY, state->arg_end, state); \
  r05_aterm_category_complete(aterm); \
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1); \
  if (old_counter == 1) \
    r05_enqueue_one_aterm(state, aterm->parent);

#define NO_CHECK
#define CHECK_ZERODIV \
  if (sY->info.number == 0) { \
    r05_builtin_error("divide by zero", state); \
  }


/**
   2. <Add s.NUMBER s.NUMBER> == 1? s.NUMBER
*/
R05_DEFINE_ENTRY_FUNCTION(Add, "Add") {
  r05_number res;
  ARITHM_PREFIX

  res = sX->info.number + sY->info.number;

  if (res >= sX->info.number) {
    state->arg_begin->tag = R05_DATATAG_NUMBER;
    state->arg_begin->info.number = res;

    r05_splice_to_freelist(func_name, state->arg_end, state);
  } else {
    state->arg_begin->tag = R05_DATATAG_NUMBER;
    state->arg_begin->info.number = 1;

    func_name->tag = R05_DATATAG_NUMBER;
    func_name->info.number = res;

    r05_splice_to_freelist(sX, state->arg_end, state);
  }
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  4. <Br e.Key '=' e.Value> == empty
*/
struct r05_function r05f_Br = { r05_br, "Br" };


/**
   3. <Ars s.ArgNo> == e.Argument

      s.ArgNo ::= s.NUMBER
      e.Argument ::= s.CHAR*
*/
R05_DEFINE_ENTRY_FUNCTION(Arg, "Arg") {
  struct r05_node *callable = state->arg_begin->next;
  struct r05_node *parg_no = callable->next;
  int arg_no;

  if (
    parg_no == state->arg_end
    || R05_DATATAG_NUMBER != parg_no->tag
    || parg_no->next != state->arg_end
  ) {
    r05_recognition_impossible(state);
  }

  arg_no = (int) parg_no->info.number;

  r05_reset_allocator(state);
  r05_alloc_string(r05_arg(arg_no), state);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
   5. <Card> == s.CHAR* 0?
*/
static void read_from_stream(FILE *input, struct r05_state *state);

R05_DEFINE_ENTRY_FUNCTION(Card, "Card") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callee = state->arg_begin->next;

  if (callee->next != state->arg_end) {
    r05_recognition_impossible(state);
  }

  r05_reset_allocator(state);
  read_from_stream(stdin, state);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


static void read_from_stream(FILE *input, struct r05_state *state) {
  int cur_char;

  while (cur_char = fgetc(input), cur_char != EOF && cur_char != '\n') {
    r05_alloc_char((char) cur_char, state);
  }

  if (cur_char == EOF) {
    r05_alloc_number(0, state);
  }
}


/**
   6. <Chr e.Expr> == e.Expr’

   В e.Expr’ все числа заменены на литеры с соответствующими кодами
*/
R05_DEFINE_ENTRY_FUNCTION(Chr, "Chr") {
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *p = callee->next;

  while (p != state->arg_end) {
    if (p->tag == R05_DATATAG_NUMBER) {
      p->tag = R05_DATATAG_CHAR;
      p->info.char_ = (unsigned char) p->info.number;
    }
    p = p->next;
  }
  r05_splice_to_freelist(state->arg_begin, callee, state);
  r05_splice_to_freelist(state->arg_end, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  7. <Cp e.Key> == e.Value
*/
struct r05_function r05f_Cp = { r05_cp, "Cp" };


/**
  8. <Dg e.Key> == e.Value
*/
struct r05_function r05f_Dg = { r05_dg, "Dg" };


/**
  10. <Div s.NUMBER s.NUMBER> == s.NUMBER
*/
R05_DEFINE_ENTRY_FUNCTION(Div, "Div") {
  ARITHM_OP(/, CHECK_ZERODIV);
}


/**
  12. <Explode s.FUNCTION> == s.CHAR+
*/
R05_DEFINE_ENTRY_FUNCTION(Explode, "Explode") {
  struct r05_node *callable = state->arg_begin->next;
  struct r05_node *ident = callable->next;

  if (
     ident == state->arg_end
     || R05_DATATAG_FUNCTION != ident->tag
     || ident->next != state->arg_end
  ) {
    r05_recognition_impossible(state);
  }

  r05_reset_allocator(state);
  r05_alloc_string(ident->info.function->name, state);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  13. <First s.Len e.Items> == (e.Prefix) e.Suffix

  e.Items : e.Prefix e.Suffix
  |e.Prefix| == s.Len || { |e.Prefix| < s.Len && |e.Suffix| == 0 }
*/
R05_DEFINE_ENTRY_FUNCTION(First, "First") {
  struct r05_node *sLen, *eItems_b, *eItems_e;
  r05_number counter;
  struct r05_node *ePrefix_b, *ePrefix_e, *eSuffix_b, *eSuffix_e;
  struct r05_node *left_bracket, *right_bracket, *pos;

  r05_prepare_argument(&eItems_b, &eItems_e, state);

  if (
    ! r05_svar_left(&sLen, &eItems_b, &eItems_e, state)
    || sLen->tag != R05_DATATAG_NUMBER
  ) {
    r05_recognition_impossible(state);
  }

  counter = sLen->info.number;

  ePrefix_b = 0;
  ePrefix_e = 0;
  eSuffix_b = eItems_b;
  eSuffix_e = eItems_e;
  while (
    counter > 0
    && r05_open_evar_advance(&ePrefix_b, &ePrefix_e, &eSuffix_b, &eSuffix_e, state)
  ) {
    -- counter;
  }

  r05_reset_allocator(state);
  r05_alloc_open_bracket(&left_bracket, state);
  r05_alloc_close_bracket(&right_bracket, state);
  pos = r05_insert_pos(state);

  r05_link_brackets(left_bracket, right_bracket);
  r05_splice_evar(right_bracket, ePrefix_b, ePrefix_e);
  r05_splice_evar(pos, eSuffix_b, eSuffix_e);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  14. <Get s.FileNo> == s.Char* 0?
      s.FileNo ::= s.NUMBER
*/
FILE *open_numbered(unsigned int no, const char mode, struct r05_state *state);

R05_DEFINE_ENTRY_FUNCTION(Get, "Get") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callable = state->arg_begin->next;
  struct r05_node *pfile_no = callable->next;
  FILE *stream;

  if (
    pfile_no == state->arg_end
    || R05_DATATAG_NUMBER != pfile_no->tag
    || pfile_no->next != state->arg_end
  ) {
    r05_recognition_impossible(state);
  }

  stream = open_numbered((unsigned int) pfile_no->info.number, 'r', state);

  r05_reset_allocator(state);
  read_from_stream(stream, state);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


enum { FILE_LIMIT = 40 };

static FILE *s_streams[FILE_LIMIT] = { NULL };

enum { UINT_DIGITS = (sizeof(unsigned int) * 8 + 2) / 3 };

FILE *open_numbered(
  unsigned int file_no, const char mode,
  struct r05_state *state
) {
  char mode_str[2] = "*";

  /*
    Ложное предупреждение BCC 5.5:
    компилятор не допускает инициализацию структур и массивов переменными.
  */
  mode_str[0] = mode;
  file_no %= FILE_LIMIT;
  if (file_no == 0 && mode == 'r') {
    return stdin;
  } else if (file_no == 0 && mode == 'w') {
    return stderr;
  }

  if (s_streams[file_no] == NULL) {
    static const char filename_format[] = "REFAL%u.DAT";
    char filename[sizeof(filename_format) + UINT_DIGITS];

    sprintf(filename, filename_format, file_no);
    s_streams[file_no] = fopen(filename, mode_str);

    if (s_streams[file_no] == NULL) {
      static const char error_format[] = "Can't open REFAL%u.DAT as \"%c\"";
      char error[sizeof(error_format) + UINT_DIGITS];

      sprintf(error, error_format, file_no, mode);
      r05_builtin_error_errno(error, state);
    }
  }

  return s_streams[file_no];
}


/**
  17. <Lenw e.Expr> == s.Len e.Expr
      s.Len ::= s.NUMBER, s.Len == |e.Expr|
*/
R05_DEFINE_ENTRY_FUNCTION(Lenw, "Lenw") {
  struct r05_node *sLen, *eItems_b, *eItems_e, *tTerm;
  r05_number counter = 0;

  r05_prepare_argument(&eItems_b, &eItems_e, state);
  sLen = state->arg_begin->next;

  while (r05_tvar_left(&tTerm, &eItems_b, &eItems_e, state)) {
    ++ counter;
  }

  sLen->tag = R05_DATATAG_NUMBER;
  sLen->info.number = counter;

  r05_splice_to_freelist(state->arg_begin, state->arg_begin, state);
  r05_splice_to_freelist(state->arg_end, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
   18. <Lower e.Expr> == e.Expr’

   В e.Expr’ все литеры приведены к нижнему регистру
*/
R05_DEFINE_ENTRY_FUNCTION(Lower, "Lower") {
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *p = callee->next;

  while (p != state->arg_end) {
    if (p->tag == R05_DATATAG_CHAR) {
      p->info.char_ = (char) tolower(p->info.char_);
    }
    p = p->next;
  }
  r05_splice_to_freelist(state->arg_begin, callee, state);
  r05_splice_to_freelist(state->arg_end, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}

/**
  19. <Mod s.NUMBER s.NUMBER> == s.NUMBER
*/
R05_DEFINE_ENTRY_FUNCTION(Mod, "Mod") {
  ARITHM_OP(%, CHECK_ZERODIV);
}


/**
  20. <Mul s.NUMBER s.NUMBER> == s.NUMBER
*/
R05_DEFINE_ENTRY_FUNCTION(Mul, "Mul") {
  ARITHM_OP(*, NO_CHECK)
}


/**
  21. <Numb s.Digit* e.Skipped> == s.NUMBER
      s.Digit ::= '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'

      Если аргумент не начинается с последовательности цифр,
      функция возвращает 0.
*/
R05_DEFINE_ENTRY_FUNCTION(Numb, "Numb") {
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *p;
  r05_number result = 0;

  for (
    p = callee->next;
    p != state->arg_end && R05_DATATAG_CHAR == p->tag && isdigit(p->info.char_);
    p = p->next
  ) {
    result = 10 * result + (p->info.char_ - '0');
  }

  state->arg_begin->tag = R05_DATATAG_NUMBER;
  state->arg_begin->info.number = result;
  r05_splice_to_freelist(callee, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  22. <Open s.Mode s.FileNo e.FileName> == []
      s.Mode ::=
          'r' | 'w' | 'a'
        |  r  |  w  |  a
        |  rb |  wb |  ab
*/
R05_DEFINE_ENTRY_ENUM(r, "r")
R05_DEFINE_ENTRY_ENUM(w, "w")
R05_DEFINE_ENTRY_ENUM(a, "a")
R05_DEFINE_ENTRY_ENUM(rb, "rb")
R05_DEFINE_ENTRY_ENUM(wb, "wb")
R05_DEFINE_ENTRY_ENUM(ab, "ab")

static void ensure_close_stream(unsigned int file_no, struct r05_state *state);

R05_DEFINE_ENTRY_FUNCTION(Open, "Open") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *eFileName_b, *eFileName_e, *sMode, *sFileNo;
  unsigned int file_no;
  char mode_str[2] = { '.', '\0' };
  const char *mode = mode_str;
  static const char filename_format[] = "REFAL%u.DAT";
  enum {
    DEFAULT_LEN = sizeof(filename_format) + UINT_DIGITS,
    FILENAME_LEN = FILENAME_MAX > DEFAULT_LEN ? FILENAME_MAX : DEFAULT_LEN
  };
  char filename[FILENAME_LEN + 1] = { '\0' };
  size_t filename_len;

  r05_prepare_argument(&eFileName_b, &eFileName_e, state);

  if (
    ! r05_svar_left(&sMode, &eFileName_b, &eFileName_e, state)
    || (R05_DATATAG_CHAR != sMode->tag && R05_DATATAG_FUNCTION != sMode->tag)
    || ! r05_svar_left(&sFileNo, &eFileName_b, &eFileName_e, state)
    || R05_DATATAG_NUMBER != sFileNo->tag
  ) {
    r05_recognition_impossible(state);
  }

  filename_len =
    r05_read_chars(filename, FILENAME_LEN, &eFileName_b, &eFileName_e);

  file_no = sFileNo->info.number % FILE_LIMIT;

  if (filename_len != 0) {
    filename[filename_len] = '\0';
  } else {
    sprintf(filename, filename_format, file_no);
  }

  if (R05_DATATAG_CHAR == sMode->tag) {
    char mode = sMode->info.char_;
    if (mode != 'r' && mode != 'w' && mode != 'a') {
      r05_builtin_error("Bad file mode, expected 'r', 'w' or 'a'", state);
    }
    mode_str[0] = mode;
  } else {
    mode = sMode->info.function->name;
  }

  if (! r05_empty_seq(eFileName_b, eFileName_e)) {
    static const char error_format[] =
      "Very long file name, maximum available is %u";
    char error[sizeof(error_format) + UINT_DIGITS];

    sprintf(error, error_format, (unsigned int) FILENAME_MAX);
    r05_builtin_error(error, state);
  }

  ensure_close_stream(file_no, state);

  s_streams[file_no] = fopen(filename, mode);
  if (s_streams[file_no] == NULL) {
    char mode_buffer[100] = { '\0' };
    static const char error_format[] = "Can't open %s for \"%s\"";
    char error[sizeof(error_format) + FILENAME_MAX + sizeof(mode_buffer)];

    strncpy(mode_buffer, mode, sizeof(mode_buffer) - 1);
    sprintf(error, error_format, filename, mode_buffer);
    r05_builtin_error_errno(error, state);
  }

  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}

static void ensure_close_stream(unsigned int file_no, struct r05_state *state) {
  if (s_streams[file_no] != NULL && fclose(s_streams[file_no]) == EOF) {
    r05_builtin_error_errno("Can't close stream", state);
  }

  s_streams[file_no] = NULL;
}


/**
  23. <Ord e.Expr> == e.Expr’

  В e.Expr’ все литеры заменены на их коды ASCII
*/
R05_DEFINE_ENTRY_FUNCTION(Ord, "Ord") {
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *p = callee->next;

  while (p != state->arg_end) {
    if (p->tag == R05_DATATAG_CHAR) {
      p->tag = R05_DATATAG_NUMBER;
      p->info.number = (unsigned char) p->info.char_;
    }
    p = p->next;
  }

  r05_splice_to_freelist(state->arg_begin, callee, state);
  r05_splice_to_freelist(state->arg_end, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


enum output_func_type {
  PRINT, PROUT, PUT, PUTOUT, WRITE
};

static void output_func(
  enum output_func_type type, struct r05_aterm *aterm, struct r05_state *state
) {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *p, *before_expr;
  FILE *output;

  if (type == PRINT || type == PROUT) {
    before_expr = callee;
    output = stdout;
  } else if (type == PUT || type == PUTOUT || type == WRITE) {
    struct r05_node *pfile_no = callee->next;

    if (R05_DATATAG_NUMBER != pfile_no->tag) {
      r05_recognition_impossible(state);
    }

    before_expr = pfile_no;
    output = open_numbered((unsigned int) pfile_no->info.number, 'w', state);
  } else {
    r05_switch_default_violation(type);
  }

#define CHECK_PRINTF(printf_call) \
  ((printf_call) >= 0 ? (void) 0 \
  : r05_builtin_error_errno("Error in call " #printf_call, state))

  for (p = before_expr->next; p != state->arg_end; p = p->next) {
    switch (p->tag) {
      case R05_DATATAG_CHAR:
        CHECK_PRINTF(fprintf(output, "%c", p->info.char_));
        break;

      case R05_DATATAG_FUNCTION:
        CHECK_PRINTF(fprintf(output, "%s ", p->info.function->name));
        break;

      case R05_DATATAG_NUMBER:
        CHECK_PRINTF(fprintf(output, "%lu ", (long unsigned int) p->info.number));
        break;

      case R05_DATATAG_OPEN_BRACKET:
        CHECK_PRINTF(fprintf(output, "("));
        break;

      case R05_DATATAG_CLOSE_BRACKET:
        CHECK_PRINTF(fprintf(output, ")"));
        break;

      default:
        r05_switch_default_violation(p->tag);
    }
  }

  if (type != WRITE) {
    CHECK_PRINTF(fprintf(output, "\n"));
  }

#undef CHECK_PRINTF

  if (type == PRINT || type == PUT) {
    r05_splice_to_freelist(state->arg_begin, before_expr, state);
    r05_splice_to_freelist(state->arg_end, state->arg_end, state);
  } else if (type == PROUT || type == PUTOUT || type == WRITE) {
    r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  } else {
    r05_switch_default_violation(type);
  }
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  24. <Print e.Expr> == []
*/
R05_DEFINE_ENTRY_FUNCTION(Print, "Print") {
  output_func(PRINT, aterm, state);
}


/**
  25. <Prout e.Expr> == []
*/
R05_DEFINE_ENTRY_FUNCTION(Prout, "Prout") {
  output_func(PROUT, aterm, state);
}


/**
  26. <Put s.FileNo e.Expr> == []
*/
R05_DEFINE_ENTRY_FUNCTION(Put, "Put") {
  output_func(PUT, aterm, state);
}


/**
  27. <Putout s.FileNo e.Expr> == []
*/
R05_DEFINE_ENTRY_FUNCTION(Putout, "Putout") {
  output_func(PUTOUT, aterm, state);
}


/**
  28. <Rp e.Key '=' e.Value> == empty
*/
struct r05_function r05f_Rp = { r05_rp, "Rp" };


/**
  30. <Sub s.NUMBER s.NUMBER> == '-' s.NUMBER
*/
R05_DEFINE_ENTRY_FUNCTION(Sub, "Sub") {
  ARITHM_PREFIX

  if (sX->info.number >= sY->info.number) {
    state->arg_begin->tag = R05_DATATAG_NUMBER;
    state->arg_begin->info.number = sX->info.number - sY->info.number;

    r05_splice_to_freelist(func_name, state->arg_end, state);
  } else {
    state->arg_begin->tag = R05_DATATAG_CHAR;
    state->arg_begin->info.char_ = '-';

    func_name->tag = R05_DATATAG_NUMBER;
    func_name->info.number = sY->info.number - sX->info.number;

    r05_splice_to_freelist(sX, state->arg_end, state);
  }
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  31. <Symb e.Sign s.NUMBER> == e.Sign s.CHAR+
      e.Sign ::= '+' | '-' | пусто
*/
R05_DEFINE_ENTRY_FUNCTION(Symb, "Symb") {
  struct r05_node *callable = state->arg_begin->next;
  struct r05_node *pnumber = callable->next;
  r05_number number;
  char sign = '\0';

  /*
    Длина десятичного числа = 0,3 * длина двоичного числа,
    т.к. lg(2) = 0,3. Хрен с ним, что много. Главное, что не мало.
  */
  enum { cMaxNumberLen = 8 * sizeof(r05_number) * 3 / 10 + 2 };

  char buffer[cMaxNumberLen + 1] = { '\0' };
  char *cur_digit = buffer + cMaxNumberLen;

  if (R05_DATATAG_CHAR == pnumber->tag) {
    sign = pnumber->info.char_;

    if (sign != '+' && sign != '-') {
      r05_recognition_impossible(state);
    }

    pnumber = pnumber->next;
  }

  if (
    pnumber == state->arg_end
    || R05_DATATAG_NUMBER != pnumber->tag
    || pnumber->next != state->arg_end
  ) {
    r05_recognition_impossible(state);
  }

  number = pnumber->info.number;

  r05_reset_allocator(state);

  if (sign != '\0') {
    r05_alloc_char(sign, state);
  }

  if (number > 0) {
    while (number != 0) {
      -- cur_digit;
      *cur_digit = (char) ((number % 10) + '0');
      number /= 10;
    }

    r05_alloc_string(cur_digit, state);
  } else {
    r05_alloc_string("0", state);
  }

  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  32. <Time> == s.CHAR+
*/
R05_DEFINE_ENTRY_FUNCTION(Time, "Time") {
  if (!r05_is_ready_to_exec(aterm))
    return;

  char *time_str, *n;
  time_t now;

  if (state->arg_begin->next->next != state->arg_end) {
    r05_recognition_impossible(state);
  }

  time(&now);
  time_str = ctime(&now);
  n = strchr(time_str, '\n');
  assert(n != NULL);
  *n = '\0';

  r05_reset_allocator(state);
  r05_alloc_string(time_str, state);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  33. <Type e.Expr> == s.Type s.SubType e.Expr
      Type of first term of e.Expr

      s.Type s.SubType ::=
          'Lu' — uppercase latin letter
        | 'Ll' — lowercase latin letter
        | 'D0' — decimal digit
        | 'Wi' — identifier (function)
        | 'N0' — number
        | 'Pu' — isprint() && isupper()
        | 'Pl' — isprint() && ! isupper()
        | 'Ou' — other && isupper()
        | 'Ol' — other && ! isupper()
        | 'B0' — brackets
        | '*0' — empty expression
*/
R05_DEFINE_ENTRY_FUNCTION(Type, "Type") {
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *first_term = callee->next;
  char type, subtype;

  if (first_term == state->arg_end) {
    type = '*';
    subtype = '0';
  } else if (R05_DATATAG_CHAR == first_term->tag) {
    char ch = first_term->info.char_;

    if (isupper(ch)) {
      subtype = 'u';
    } else {
      subtype = 'l';
    }
    if (isalpha(ch)) {
      type = 'L';
    } else if (isdigit(ch)) {
      type = 'D';
      subtype = '0';
    } else if (isprint(ch)) {
      type = 'P';
    } else {
      type = 'O';
    }
  } else if (R05_DATATAG_FUNCTION == first_term->tag) {
    type = 'W';
    subtype = 'i';
  } else if (R05_DATATAG_NUMBER == first_term->tag) {
    type = 'N';
    subtype = '0';
  } else if (R05_DATATAG_OPEN_BRACKET == first_term->tag) {
    type = 'B';
    subtype = '0';
  } else {
    r05_switch_default_violation(first_term->tag);
#ifndef R05_NORETURN_DEFINED
    return;
#endif
  }

  state->arg_begin->tag = R05_DATATAG_CHAR;
  state->arg_begin->info.char_ = type;
  callee->tag = R05_DATATAG_CHAR;
  callee->info.char_ = subtype;

  r05_splice_to_freelist(state->arg_end, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
   34. <Upper e.Expr> == e.Expr’

   В e.Expr’ все литеры приведены к верхнему регистру
*/
R05_DEFINE_ENTRY_FUNCTION(Upper, "Upper") {
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *p = callee->next;

  while (p != state->arg_end) {
    if (p->tag == R05_DATATAG_CHAR) {
      p->info.char_ = (char) toupper(p->info.char_);
    }
    p = p->next;
  }

  r05_splice_to_freelist(state->arg_begin, callee, state);
  r05_splice_to_freelist(state->arg_end, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}

/**
   50. <Residue s.Func e.Arg> == <s.Func e.Arg>
*/
R05_DEFINE_ENTRY_FUNCTION(Residue, "Residue") {
  struct r05_node *residue = state->arg_begin->next;
  struct r05_node *callable = residue->next;
  if (callable->tag != R05_DATATAG_FUNCTION) {
    r05_recognition_impossible(state);
  }

  r05_reuse_aterm(
    state->arg_begin, state->arg_end, aterm, state
  );
  r05_splice_to_freelist(residue, residue, state);
  r05_enqueue_one_aterm_async(state, aterm);
}

/**
  51. <GetEnv e.EnvName> == e.EnvValue
      e.EnvName, e.EnvValue ::= s.CHAR*
*/
R05_DEFINE_ENTRY_FUNCTION(GetEnv, "GetEnv") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *eEnvName_b, *eEnvName_e;
  char env_name[2001];
  size_t env_name_len;
  const char *env_value;

  r05_prepare_argument(&eEnvName_b, &eEnvName_e, state);
  env_name_len =
    r05_read_chars(env_name, sizeof(env_name) - 1, &eEnvName_b, &eEnvName_e);

  if (! r05_empty_seq(eEnvName_b, eEnvName_e)) {
    if (R05_DATATAG_CHAR == eEnvName_b->tag) {
      r05_builtin_error("very long environment variable name", state);
    } else {
      r05_recognition_impossible(state);
    }
  }

  env_name[env_name_len] = '\0';
  env_value = getenv(env_name);
  if (! env_value) {
    env_value = "";
  }

  r05_reset_allocator(state);
  r05_alloc_string(env_value, state);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  52. <System e.Command> == e.RetCode
      e.RetCode ::= '-'? s.NUMBER
*/
R05_DEFINE_ENTRY_FUNCTION(System, "System") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *eCommand_b, *eCommand_e;
  char command[2001];
  size_t command_len;
  int retcode;

  r05_prepare_argument(&eCommand_b, &eCommand_e, state);
  command_len =
    r05_read_chars(command, sizeof(command) - 1, &eCommand_b, &eCommand_e);

  if (! r05_empty_seq(eCommand_b, eCommand_e)) {
    if (R05_DATATAG_CHAR == eCommand_b->tag) {
      r05_builtin_error("very long command line", state);
    } else {
      r05_recognition_impossible(state);
    }
  }

  command[command_len] = '\0';
  fflush(NULL);
  retcode = system(command);

#if defined(WIFEXITED) && defined(WEXITSTATUS)
  if (WIFEXITED(retcode)) {
    retcode = WEXITSTATUS(retcode);
  } else {
    retcode = -1;
  }
#endif  /* defined(WIFEXITED) && defined(WEXITSTATUS) */

  r05_reset_allocator(state);
  if (retcode < 0) {
    r05_alloc_char('-', state);
    retcode = -retcode;
  }
  r05_alloc_number((r05_number) retcode, state);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  53. <Exit e.RetCode>
*/
R05_DEFINE_ENTRY_FUNCTION(Exit, "Exit") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callable = state->arg_begin->next;
  struct r05_node *pretcode = callable->next;
  int retcode;
  signed sign = +1;

  if (pretcode == state->arg_end) {
    r05_recognition_impossible(state);
  }

  if (R05_DATATAG_CHAR == pretcode->tag) {
    if ('-' == pretcode->info.char_) {
      sign = -1;
      pretcode = pretcode->next;
    } else {
      r05_recognition_impossible(state);
    }
  }

  if (
    pretcode == state->arg_end
    || R05_DATATAG_NUMBER != pretcode->tag
    || pretcode->next != state->arg_end
  ) {
    r05_recognition_impossible(state);
  }

  retcode = sign * (int) pretcode->info.number;
  r05_exit(retcode, state);
}


/**
  54. <Close s.FileNo> == []
*/
R05_DEFINE_ENTRY_FUNCTION(Close, "Close") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callable = state->arg_begin->next;
  struct r05_node *pfile_no = callable->next;
  unsigned int file_no;

  if (
    pfile_no == state->arg_end
    || R05_DATATAG_NUMBER != pfile_no->tag
    || pfile_no->next != state->arg_end
  ) {
    r05_recognition_impossible(state);
  }

  file_no = (unsigned int) pfile_no->info.number % FILE_LIMIT;
  ensure_close_stream(file_no, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  55. <ExistFile e.FileName>
        == True
        == False
      e.FileName ::= s.CHAR+
*/
R05_DEFINE_ENTRY_ENUM(True, "True")
R05_DEFINE_ENTRY_ENUM(False, "False")

R05_DEFINE_ENTRY_FUNCTION(ExistFile, "ExistFile") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *eFileName_b, *eFileName_e;
  char filename[FILENAME_MAX + 1];
  size_t filename_len;
  FILE *file;

  r05_prepare_argument(&eFileName_b, &eFileName_e, state);
  filename_len =
    r05_read_chars(filename, sizeof(filename) - 1, &eFileName_b, &eFileName_e);

  if (! r05_empty_seq(eFileName_b, eFileName_e)) {
    if (R05_DATATAG_CHAR == eFileName_b->tag) {
      r05_builtin_error("very long filename", state);
    } else {
      r05_recognition_impossible(state);
    }
  }

  filename[filename_len] = '\0';
  file = fopen(filename, "r");

  state->arg_begin->tag = R05_DATATAG_FUNCTION;
  if (file != NULL) {
    state->arg_begin->info.function = &r05f_True;
    if (fclose(file) == EOF) {
      r05_builtin_error_errno("fclose error", state);
    }
  } else {
    state->arg_begin->info.function = &r05f_False;
  }

  r05_splice_to_freelist(callee, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  57. <RemoveFile e.FileName>
        == True ()
        == False (e.Message)
      e.Message ::= s.CHAR*
*/
R05_DEFINE_ENTRY_FUNCTION(RemoveFile, "RemoveFile") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *eFileName_b, *eFileName_e, *left_bracket, *right_bracket;
  char filename[FILENAME_MAX + 1];
  size_t filename_len;
  int res;
  struct r05_function *sign;
  const char *message;

  r05_prepare_argument(
    &eFileName_b, &eFileName_e, state);
  filename_len =
    r05_read_chars(filename, sizeof(filename) - 1, &eFileName_b, &eFileName_e);

  if (! r05_empty_seq(eFileName_b, eFileName_e)) {
    if (R05_DATATAG_CHAR == eFileName_b->tag) {
      r05_builtin_error("very long filename", state);
    } else {
      r05_recognition_impossible(state);
    }
  }

  filename[filename_len] = '\0';
  errno = 0;
  res = remove(filename);

  if (res == 0) {
    sign = &r05f_True;
    message = "";
  } else {
    sign = &r05f_False;
    message = strerror(errno);
  }

  r05_reset_allocator(state);
  r05_alloc_function(sign, state);
  r05_alloc_open_bracket(&left_bracket, state);
  r05_alloc_string(message, state);
  r05_alloc_close_bracket(&right_bracket, state);
  r05_link_brackets(left_bracket, right_bracket);
  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  12. <Explode_Ext s.FUNCTION> == s.CHAR+
*/
struct r05_function r05f_Explodeu_Ext = { r05c_Explode, "Explode_Ext" };


/**
  61. <Compare s.X s.Y>
        == '-' | '0' | '+'
      s.X, s.Y ::= s.NUMBER

  Функция возвращает знак разности между s.X и s.Y
*/
R05_DEFINE_ENTRY_FUNCTION(Compare, "Compare") {
  struct r05_node *func_name, *sX, *sY;

  func_name = state->arg_begin->next;
  sX = func_name->next;
  if (sX->tag != R05_DATATAG_NUMBER) {
    r05_recognition_impossible(state);
  }

  sY = sX->next;
  if (sY->tag != R05_DATATAG_NUMBER) {
    r05_recognition_impossible(state);
  }

  if (sY->next != state->arg_end) {
    r05_recognition_impossible(state);
  }

  if (sX->info.number < sY->info.number) {
    sX->info.char_ = '-';
  } else if (sX->info.number > sY->info.number) {
    sX->info.char_ = '+';
  } else {
    sX->info.char_ = '0';
  }
  sX->tag = R05_DATATAG_CHAR;

  r05_splice_to_freelist(state->arg_begin, func_name, state);
  r05_splice_to_freelist(sY, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


static r05_number random_digit_in_range(r05_number max);
static r05_number random_digit(void);


/**
  64. <Random s.Len> == e.RandomDigits
      e.RandomDigits ::= s.NUMBER+
      |e.RandomDigits| == ((s.Len != 0) ? s.Len : 1)
*/
R05_DEFINE_ENTRY_FUNCTION(Random, "Random") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callable = state->arg_begin->next;
  struct r05_node *pcount = callable->next;
  r05_number count;

  if (R05_DATATAG_NUMBER != pcount->tag || pcount->next != state->arg_end) {
    r05_recognition_impossible(state);
  }

  count = pcount->info.number;
  count = count > 0 ? count - 1 : 1;
  count = random_digit_in_range(count) + 1;

  r05_reset_allocator(state);
  while (count > 0) {
    r05_alloc_number(random_digit(), state);
    --count;
  }

  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


/**
  65. <RandomDigit s.Max> == s.RandomDigit
      s.RandomDigit, s.Max ::= s.NUMBER
      s.RandomDigit <= s.Max
*/
R05_DEFINE_ENTRY_FUNCTION(RandomDigit, "RandomDigit") {
  if (!r05_is_ready_to_exec(aterm))
    return;
  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *pmax = callee->next;
  const r05_number MAX = ~0;
  r05_number max, res;

  if (R05_DATATAG_NUMBER != pmax->tag || pmax->next != state->arg_end) {
    r05_recognition_impossible(state);
  }

  max = pmax->info.number;
  if (max != MAX) {
    res = random_digit_in_range(max + 1);
  } else {
    res = random_digit();
  }

  state->arg_begin->tag = R05_DATATAG_NUMBER;
  state->arg_begin->info.number = res;
  r05_splice_to_freelist(callee, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
}


static r05_number random_digit_in_range(r05_number limit) {
  const r05_number MAX = ~0;
  r05_number max_valid;
  r05_number random;

  if (limit == 0) {
    return 0;
  }

  max_valid = MAX - MAX % limit;

  do {
    random = random_digit();
  } while (random >= max_valid);

  return random % limit;
}

/*
  Метод Фибоначчи с запаздываниями.
  См. D. E. Knuth, The Art of Computer Programming,
  Volume 2, chapter 3.2.2, program A
*/
static r05_number random_digit(void) {
  enum { cMinDelay = 24, cMaxDelay = 55 };

  static int init = 0;
  static size_t k = cMaxDelay - 1;
  static size_t j = cMinDelay - 1;
  static r05_number y[cMaxDelay];

  r05_number result;

  if (! init) {
    r05_number seed = (r05_number) time(NULL);
    size_t i;

    for (i = 0; i < cMaxDelay; ++i) {
      seed = seed * 1103515245 + 12345;
      y[i] = seed;
    }

    init = 1;
  }

  result = y[k] = y[k] + y[j];
  k = (k + cMaxDelay - 1) % cMaxDelay;
  j = (j + cMaxDelay - 1) % cMaxDelay;

  return result;
}


/**
  66. <Write e.Expr> == []
*/
R05_DEFINE_ENTRY_FUNCTION(Write, "Write") {
  output_func(WRITE, aterm, state);
}


/**
  67. <ListOfBuiltin> == (s.No s.Name s.Type)*

      s.No ::= s.NUMBER
      s.Name ::= s.FUNCTION
      s.Type ::= special | regular
*/
R05_DEFINE_ENTRY_ENUM(special, "special")
R05_DEFINE_ENTRY_ENUM(regular, "regular")


R05_DEFINE_ENTRY_FUNCTION(ListOfBuiltin, "ListOfBuiltin") {
/*
  Чтобы добавить встроенную функцию Рефала-5, нужно раскомментировать
  строчку в функции ListOfBuiltin и реализовать функцию.

  Для удобства чтения и поиска функция должна находиться в месте,
  определяемом её порядковым номером. Например, Lenw (номер 17) должна
  находиться между Get (14) и Mod (19).

  В комментарии перед функцией должен быть кратко описан её тип в стиле
  принятой здесь нотации.

  Функции с номерами 42 ("Imp$$"), 43 ("Stop$$") и 44 ("") определить
  невозможно, поскольку они имеют специфическое имя, и не нужно, поскольку
  первые две — деталь внутренней реализации Рефала-5 (версии PZ),
  а третья — артефакт.
*/

  struct r05_node *callee = state->arg_begin->next;
  struct r05_node *left_bracket, *right_bracket;

  if (callee->next != state->arg_end) {
    r05_recognition_impossible(state);
  }

#define ALLOC_BUILTIN(id, function, type) \
  r05_alloc_open_bracket(&left_bracket, state); \
  r05_alloc_number(id, state); \
  r05_alloc_function(&r05f_ ## function, state); \
  r05_alloc_function(&r05f_ ## type, state); \
  r05_alloc_close_bracket(&right_bracket, state); \
  r05_link_brackets(left_bracket, right_bracket);

  r05_reset_allocator(state);

  ALLOC_BUILTIN(1, Mu, special)
  ALLOC_BUILTIN(2, Add, regular)
  ALLOC_BUILTIN(3, Arg, regular)
  ALLOC_BUILTIN(4, Br, regular)
  ALLOC_BUILTIN(5, Card, regular)
  ALLOC_BUILTIN(6, Chr, regular)
  ALLOC_BUILTIN(7, Cp, regular)
  ALLOC_BUILTIN(8, Dg, regular)
  /* ALLOC_BUILTIN(9, Dgall, regular) */
  ALLOC_BUILTIN(10, Div, regular)
  /* ALLOC_BUILTIN(11, Divmod, regular) */
  ALLOC_BUILTIN(12, Explode, regular)
  ALLOC_BUILTIN(13, First, regular)
  ALLOC_BUILTIN(14, Get, regular)
  /* ALLOC_BUILTIN(15, Implode, regular) */
  /* ALLOC_BUILTIN(16, Last, regular) */
  ALLOC_BUILTIN(17, Lenw, regular)
  ALLOC_BUILTIN(18, Lower, regular)
  ALLOC_BUILTIN(19, Mod, regular)
  ALLOC_BUILTIN(20, Mul, regular)
  ALLOC_BUILTIN(21, Numb, regular)
  ALLOC_BUILTIN(22, Open, regular)
  ALLOC_BUILTIN(23, Ord, regular)
  ALLOC_BUILTIN(24, Print, regular)
  ALLOC_BUILTIN(25, Prout, regular)
  ALLOC_BUILTIN(26, Put, regular)
  ALLOC_BUILTIN(27, Putout, regular)
  ALLOC_BUILTIN(28, Rp, regular)
  /* ALLOC_BUILTIN(29, Step, regular) */
  ALLOC_BUILTIN(30, Sub, regular)
  ALLOC_BUILTIN(31, Symb, regular)
  ALLOC_BUILTIN(32, Time, regular)
  ALLOC_BUILTIN(33, Type, regular)
  ALLOC_BUILTIN(34, Upper, regular)
  /* ALLOC_BUILTIN(35, Sysfun, regular) */
  /* ALLOC_BUILTIN(42, Impd_d_, regular) */
  /* ALLOC_BUILTIN(43, Stopd_d_, regular) */
  /* 44, "", regular */
  /* ALLOC_BUILTIN(45, Freeze, regular) */
  /* ALLOC_BUILTIN(46, Freezer, regular) */
  /* ALLOC_BUILTIN(47, Dn, regular) */
  /* ALLOC_BUILTIN(48, Up, special) */
  /* ALLOC_BUILTIN(49, Ev_met, special) */
  ALLOC_BUILTIN(50, Residue, special)
  ALLOC_BUILTIN(51, GetEnv, regular)
  ALLOC_BUILTIN(52, System, regular)
  ALLOC_BUILTIN(53, Exit, regular)
  ALLOC_BUILTIN(54, Close, regular)
  ALLOC_BUILTIN(55, ExistFile, regular)
  /* ALLOC_BUILTIN(56, GetCurrentDirectory, regular) */
  ALLOC_BUILTIN(57, RemoveFile, regular)
  /* ALLOC_BUILTIN(58, Implode_Ext, regular) */
  ALLOC_BUILTIN(59, Explodeu_Ext, regular)
  /* ALLOC_BUILTIN(60, TimeElapsed, regular) */
  ALLOC_BUILTIN(61, Compare, regular)
  /* ALLOC_BUILTIN(62, DeSysfun, regular) */
  /* ALLOC_BUILTIN(63, XMLParse, regular) */
  ALLOC_BUILTIN(64, Random, regular)
  ALLOC_BUILTIN(65, RandomDigit, regular)
  ALLOC_BUILTIN(66, Write, regular)
  ALLOC_BUILTIN(67, ListOfBuiltin, regular)
  /* ALLOC_BUILTIN(68, SizeOf, regular) */
  /* ALLOC_BUILTIN(69, GetPID, regular) */
  /* ALLOC_BUILTIN(70, int4fab_1, regular) */
  /* ALLOC_BUILTIN(71, GetPPID, regular) */

#undef ALLOC_BUILTIN

  r05_splice_from_freelist(state->arg_begin, state);
  r05_splice_to_freelist(state->arg_begin, state->arg_end, state);
  int old_counter = atomic_fetch_sub(&(aterm->parent->child_aterms), 1);
  if (old_counter == 1)
    r05_enqueue_one_aterm(state, aterm->parent);
  r05_aterm_category_complete(aterm);
};
