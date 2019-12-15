#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "refal05rts.h"

#ifndef R05_SHOW_DEBUG
#define R05_SHOW_DEBUG 0
#endif  /* ifdef R05_SHOW_DEBUG */


#define EXIT_CODE_RECOGNITION_IMPOSSIBLE 201
#define EXIT_CODE_NO_MEMORY 202
#define EXIT_CODE_BUILTIN_ERROR 203


/*==============================================================================
   Операции сопоставления с образцом
==============================================================================*/

void r05_prepare_argument(
  struct r05_node **left, struct r05_node **right,
  struct r05_node *arg_begin, struct r05_node *arg_end
) {
  *left = arg_begin->next->next;
  *right = arg_end->prev;
  if ((*right)->next == *left) {
    *left = 0;
    *right = 0;
  }
}


static void move_left(struct r05_node **first, struct r05_node **last) {
  /* assert((*first == 0) == (*last == 0)); */
  if (*first == 0) assert (*last == 0);
  if (*first != 0) assert (*last != 0);

  if (*first == *last) {
    *first = 0;
    *last = 0;
  } else {
    *first = (*first)->next;
  }
}


static void move_right(struct r05_node **first, struct r05_node **last) {
  /* assert((*first == 0) == (*last == 0)); */
  if (*first == 0) assert (*last == 0);
  if (*first != 0) assert (*last != 0);

  if (*first == *last) {
    *first = 0;
    *last = 0;
  } else {
    *last = (*last)->prev;
  }
}


int r05_empty_seq(struct r05_node *first, struct r05_node *last) {
  /* assert((first == 0) == (last == 0)); */
  if (first == 0) assert (last == 0);
  if (first != 0) assert (last != 0);

  return (first == 0) && (last == 0);
}


int r05_function_left(
  struct r05_function *fn, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if ((*first)->tag != R05_DATATAG_FUNCTION) {
    return 0;
  } else if ((*first)->info.function != fn) {
    return 0;
  } else {
    move_left(first, last);
    return 1;
  }
}


int r05_function_right(
  struct r05_function *fn, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (R05_DATATAG_FUNCTION != (*last)->tag) {
    return 0;
  } else if ((*last)->info.function != fn) {
    return 0;
  } else {
    move_right(first, last);
    return 1;
  }
}


int r05_char_left(
  char ch, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (R05_DATATAG_CHAR != (*first)->tag) {
    return 0;
  } else if ((*first)->info.char_ != ch) {
    return 0;
  } else {
    move_left(first, last);
    return 1;
  }
}


int r05_char_right(
  char ch, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (R05_DATATAG_CHAR != (*last)->tag) {
    return 0;
  } else if ((*last)->info.char_ != ch) {
    return 0;
  } else {
    move_right(first, last);
    return 1;
  }
}


int r05_number_left(
  r05_number num, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (R05_DATATAG_NUMBER != (*first)->tag) {
    return 0;
  } else if ((*first)->info.number != num) {
    return 0;
  } else {
    move_left(first, last);
    return 1;
  }
}


int r05_number_right(
  r05_number num, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (R05_DATATAG_NUMBER != (*last)->tag) {
    return 0;
  } else if ((*last)->info.number != num) {
    return 0;
  } else {
    move_right(first, last);
    return 1;
  }
}


int r05_brackets_left(
  struct r05_node **res_first, struct r05_node **res_last,
  struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (R05_DATATAG_OPEN_BRACKET != (*first)->tag) {
    return 0;
  } else {
    struct r05_node *left_bracket = *first;
    struct r05_node *right_bracket = left_bracket->info.link;

    if (left_bracket->next != right_bracket) {
      *res_first = left_bracket->next;
      *res_last = right_bracket->prev;
    } else {
      *res_first = 0;
      *res_last = 0;
    }

    if (right_bracket == *last) {
      *first = 0;
      *last = 0;
    } else {
      *first = right_bracket->next;
    }

    return 1;
  }
}


int r05_brackets_right(
  struct r05_node **res_first, struct r05_node **res_last,
  struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (R05_DATATAG_CLOSE_BRACKET != (*last)->tag) {
    return 0;
  } else {
    struct r05_node *right_bracket = *last;
    struct r05_node *left_bracket = right_bracket->info.link;

    if (left_bracket->next != right_bracket) {
      *res_first = left_bracket->next;
      *res_last = right_bracket->prev;
    } else {
      *res_first = 0;
      *res_last = 0;
    }

    if (*first == left_bracket) {
      *first = 0;
      *last = 0;
    } else {
      *last = left_bracket->prev;
    }

    return 1;
  }
}


#define is_open_bracket(node) (R05_DATATAG_OPEN_BRACKET == (node)->tag)
#define is_close_bracket(node) (R05_DATATAG_CLOSE_BRACKET == (node)->tag)

int r05_svar_left(
  struct r05_node **svar, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (is_open_bracket(*first)) {
    return 0;
  } else {
    *svar = *first;
    move_left(first, last);
    return 1;
  }
}


int r05_svar_right(
  struct r05_node **svar, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (is_close_bracket(*last)) {
    return 0;
  } else {
    *svar = *last;
    move_right(first, last);
    return 1;
  }
}


int r05_tvar_left(
  struct r05_node **tvar, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (is_open_bracket(*first)) {
    struct r05_node *right_bracket = (*first)->info.link;

    *tvar = *first;
    *first = right_bracket;
    move_left(first, last);
    return 1;
  } else {
    *tvar = *first;
    move_left(first, last);
    return 1;
  }
}


int r05_tvar_right(
  struct r05_node **tvar, struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  assert((*first == 0) == (*last == 0));

  if (r05_empty_seq(*first, *last)) {
    return 0;
  } else if (is_close_bracket(*last)) {
    struct r05_node *right_bracket = *last;
    struct r05_node *left_bracket = right_bracket->info.link;

    *tvar = left_bracket;
    *last = left_bracket;
    move_right(first, last);
    return 1;
  } else {
    *tvar = *last;
    move_right(first, last);
    return 1;
  }
}


static int equal_nodes(struct r05_node *node1, struct r05_node *node2) {
  if (node1->tag != node2->tag) {
    return 0;
  } else {
    switch (node1->tag) {
      case R05_DATATAG_CHAR:
        return (node1->info.char_ == node2->info.char_);

      case R05_DATATAG_NUMBER:
        return (node1->info.number == node2->info.number);

      case R05_DATATAG_FUNCTION:
        return (node1->info.function == node2->info.function);

      /*
        Сведения о связях между скобками нужны для других целей, здесь
        же нам важны только их одновременные появления.
      */
      case R05_DATATAG_OPEN_BRACKET:
      case R05_DATATAG_CLOSE_BRACKET:
        return 1;

      /*
        Данная функция предназначена только для использования функциями рас-
        познавания образца. Поэтому других узлов мы тут не ожидаем.
      */
      default:
        r05_switch_default_violation(node1->tag);
#ifndef R05_NORETURN_DEFINED
        return 0;
#endif
    }
  }
}


static void add_match_repeated_tvar_time(
  clock_t duration, struct r05_state *state
);

static int equal_expressions(
  struct r05_node *first1, struct r05_node *last1,
  struct r05_node *first2, struct r05_node *last2,
  struct r05_state *state
) {
  clock_t start_match = clock();

  assert((first1 == 0) == (last1 == 0));
  assert((first2 == 0) == (last2 == 0));

  while (
    /* Порядок условий важен */
    ! r05_empty_seq(first1, last1) && ! r05_empty_seq(first2, last2)
      && equal_nodes(first1, first2)
  ) {
    move_left(&first1, &last1);
    move_left(&first2, &last2);
  }

  /*
    Здесь r05_empty_seq(first1, last1) || r05_empty_seq(first2, last2)
      || !equal_nodes(first1, first2)
  */

  add_match_repeated_tvar_time(clock() - start_match, state);

  /* Успешное завершение — если мы достигли конца в обоих выражениях */
  if (r05_empty_seq(first1, last1) && r05_empty_seq(first2, last2)) {
    return 1;
  } else {
    /* Любое другое завершение цикла свидетельствует о несовпадении */
    return 0;
  }
}


int r05_repeated_stvar_left(
  struct r05_node **stvar, struct r05_node *stvar_sample,
  struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  struct r05_node *left_term = 0;
  struct r05_node *copy_last = *last;

  assert((*first == 0) == (*last == 0));

  if (
    ! is_open_bracket(stvar_sample) &&
    r05_svar_left(stvar, first, last, state)
  ) {
    return equal_nodes(*stvar, stvar_sample);
  } else if (r05_tvar_left(&left_term, first, last, state)) {
    struct r05_node *left_term_e;
    struct r05_node *stvar_sample_e;

    if (r05_empty_seq(*first, *last)) {
      left_term_e = copy_last;
    } else {
      left_term_e = (*first)->prev;
    }

    if (is_open_bracket(stvar_sample)) {
      stvar_sample_e = stvar_sample->info.link;
    } else {
      stvar_sample_e = stvar_sample;
    }

    if (equal_expressions(
        left_term, left_term_e,stvar_sample, stvar_sample_e, state
      )) {
      *stvar = left_term;

      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}

int r05_repeated_stvar_right(
  struct r05_node **stvar, struct r05_node *stvar_sample,
  struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  struct r05_node *right_term = 0;
  struct r05_node *old_last = *last;

  assert((*first == 0) == (*last == 0));

  if (
    ! is_open_bracket(stvar_sample)
    && r05_svar_right(stvar, first, last, state)
  ) {
    return equal_nodes(*stvar, stvar_sample);
  } else if (r05_tvar_right(&right_term, first, last, state)) {
    struct r05_node *right_term_e = old_last;
    struct r05_node *stvar_sample_e;

    if (is_open_bracket(stvar_sample)) {
      stvar_sample_e = stvar_sample->info.link;
    } else {
      stvar_sample_e = stvar_sample;
    }

    if (equal_expressions(
        right_term, right_term_e, stvar_sample, stvar_sample_e, state
      )) {
      *stvar = right_term;

      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}


static void add_match_repeated_evar_time(
  clock_t duration, struct r05_state *state
);

int r05_repeated_evar_left(
  struct r05_node **evar_b, struct r05_node **evar_e,
  struct r05_node *evar_b_sample, struct r05_node *evar_e_sample,
  struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  clock_t start_match = clock();
  struct r05_node *current = *first;
  struct r05_node *cur_sample = evar_b_sample;
  struct r05_node *copy_last = *last;

  while (
    /* порядок условий важен */
    !r05_empty_seq(current, copy_last)
      && !r05_empty_seq(cur_sample, evar_e_sample)
      && equal_nodes(current, cur_sample)
  ) {
    move_left(&cur_sample, &evar_e_sample);
    move_left(&current, &copy_last);
  }

  add_match_repeated_evar_time(clock() - start_match, state);

  /*
    Здесь r05_empty_seq(current, copy_last)
      || r05_empty_seq(cur_sample, evar_e_sample
      || ! equal_nodes(current, cur_sample)
  */
  if (r05_empty_seq(cur_sample, evar_e_sample)) {
    /* Это нормальное завершение цикла — вся образцовая переменная проверена */

    if (r05_empty_seq(current, copy_last)) {
      *evar_b = *first;
      *evar_e = *last;

      *first = 0;
      *last = 0;
    } else if (current != *first) {
      *evar_b = *first;
      *evar_e = current->prev;

      *first = current;
    } else {
      *evar_b = 0;
      *evar_e = 0;
    }

    return 1;
  } else {
    return 0;
  }
}


int r05_repeated_evar_right(
  struct r05_node **evar_b, struct r05_node **evar_e,
  struct r05_node *evar_b_sample, struct r05_node *evar_e_sample,
  struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  clock_t start_match = clock();
  struct r05_node *current = *last;
  struct r05_node *cur_sample = evar_e_sample;
  struct r05_node *copy_first = *first;

  while (
    /* порядок перечисления условий важен */
    !r05_empty_seq(copy_first, current)
      && !r05_empty_seq(evar_b_sample, cur_sample)
      && equal_nodes(current, cur_sample)
  ) {
    move_right(&copy_first, &current);
    move_right(&evar_b_sample, &cur_sample);
  }

  add_match_repeated_evar_time(clock() - start_match, state);

  /*
    Здесь r05_empty_seq(copy_first, current)
      || r05_empty_seq(evar_b_sample, cur_sample)
      || ! equal_nodes(current, cur_sample)
  */

  if (r05_empty_seq(evar_b_sample, cur_sample)) {
    /* Это нормальное завершение цикла: вся переменная-образец просмотрена */

    if (r05_empty_seq(copy_first, current)) {
      *evar_b = *first;
      *evar_e = *last;

      *first = 0;
      *last = 0;
    } else if (current != *last) {
      *evar_b = current->next;
      *evar_e = *last;

      *last = current;
    } else {
      *evar_b = 0;
      *evar_e = 0;
    }

    return 1;
  } else {
    return 0;
  }
}


int r05_open_evar_advance(
  struct r05_node **evar_b, struct r05_node **evar_e,
  struct r05_node **first, struct r05_node **last,
  struct r05_state *state
) {
  struct r05_node *prev_first = 0;

  assert((*first == 0) == (*last == 0));

  if (r05_tvar_left(&prev_first, first, last, state)) {
    if (! *evar_b) {
      *evar_b = prev_first;
    }

    if (is_open_bracket(prev_first)) {
      *evar_e = prev_first->info.link;
    } else {
      *evar_e = prev_first;
    }

    return 1;
  } else {
    return 0;
  }
}


size_t r05_read_chars(
  char buffer[], size_t buflen,
  struct r05_node **first, struct r05_node **last
) {
  size_t read = 0;
  while (
    read != buflen && ! r05_empty_seq(*first, *last)
      && (*first)->tag == R05_DATATAG_CHAR
  ) {
    buffer[read] = (*first)->info.char_;
    ++read;
    move_left(first, last);
  }
  return read;
}


/*==============================================================================
   Распределитель памяти
==============================================================================*/

enum { CHUNK_SIZE = 251 };

struct memory_chunk {
  struct r05_node elems[CHUNK_SIZE];
  struct memory_chunk *next;
};

static void weld(struct r05_node *left, struct r05_node *right) {
  assert(left != 0 && right != 0);

  left->next = right;
  right->prev = left;
}


static int create_nodes(struct r05_state *state) {
  size_t i;
  struct memory_chunk *chunk;

#ifdef R05_MEMORY_LIMIT
  if (state->memory_use >= R05_MEMORY_LIMIT) {
    return 0;
  }
#endif  /* ifdef R05_MEMORY_LIMIT */

  chunk = malloc(sizeof(*chunk));

  if (chunk == 0) {
    return 0;
  }

  chunk->next = state->pool;
  state->pool = chunk;

  for (i = 0; i < CHUNK_SIZE - 1; ++i) {
    chunk->elems[i].next = &chunk->elems[i + 1];
    chunk->elems[i + 1].prev = &chunk->elems[i];
    chunk->elems[i].tag = R05_DATATAG_ILLEGAL;
  }
  chunk->elems[CHUNK_SIZE - 1].tag = R05_DATATAG_ILLEGAL;

  weld(state->end_free_list.prev, &chunk->elems[0]);
  weld(&chunk->elems[CHUNK_SIZE - 1], &state->end_free_list);

  state->free_ptr = &chunk->elems[0];
  state->memory_use += CHUNK_SIZE;

  return 1;
}


static void make_dump(struct r05_state *state);

static void ensure_memory(struct r05_state *state) {
  if ((state->free_ptr == &state->end_free_list) && ! create_nodes(state)) {
    fprintf(stderr, "\nNO MEMORY\n\n");
    make_dump(state);

    r05_exit(EXIT_CODE_NO_MEMORY, state);
  }
}


static void free_memory(struct r05_state *state) {
  while (state->pool != NULL) {
    struct memory_chunk *next = state->pool->next;
    free(state->pool);
    state->pool = next;
  }

#ifdef R05_SHOW_STAT
  fprintf(
    stderr,
    "Memory used %lu nodes, %lu * %lu = %lu bytes\n",
    (unsigned long int) state->memory_use,
    (unsigned long int) state->memory_use,
    (unsigned long int) sizeof(struct r05_node),
    (unsigned long int) (state->memory_use * sizeof(struct r05_node))
  );
#endif  /* R05_SHOW_STAT */
}


/*==============================================================================
   Операции построения результата
==============================================================================*/

static void start_building_result(struct r05_state *state);

void r05_reset_allocator(struct r05_state *state) {
  start_building_result(state);
  state->free_ptr = state->begin_free_list.next;
}


struct r05_node *r05_alloc_node(enum r05_datatag tag, struct r05_state *state) {
  struct r05_node *node;

  ensure_memory(state);
  node = state->free_ptr;
  state->free_ptr = state->free_ptr->next;
  node->tag = tag;
  return node;
}


struct r05_node *r05_insert_pos(struct r05_state *state) {
  ensure_memory(state);
  return state->free_ptr;
}


static struct r05_node *list_splice(
  struct r05_node *res, struct r05_node *begin, struct r05_node *end
) {
  if ((res != begin) && ! r05_empty_seq(begin, end)) {
    struct r05_node *prev_res = res->prev;
    struct r05_node *prev_begin = begin->prev;
    struct r05_node *next_end = end->next;

    weld(prev_res, begin);
    weld(end, res);
    weld(prev_begin, next_end);
  } else {
    /* Цель достигнута сама по себе */
    return res;
  }

  return begin;
}


static void add_copy_tevar_time(clock_t duration, struct r05_state *state);

static void copy_nonempty_evar(
  struct r05_node *evar_b_sample, struct r05_node *evar_e_sample,
  struct r05_state *state
) {
  clock_t start_copy_time = clock();

  struct r05_node *bracket_stack = 0;

  while (! r05_empty_seq(evar_b_sample, evar_e_sample)) {
    struct r05_node *copy = r05_alloc_node(evar_b_sample->tag, state);

    if (is_open_bracket(copy)) {
      copy->info.link = bracket_stack;
      bracket_stack = copy;
    } else if (is_close_bracket(copy)) {
      struct r05_node *open_cobracket = bracket_stack;

      assert(bracket_stack != 0);
      bracket_stack = bracket_stack->info.link;
      r05_link_brackets(open_cobracket, copy);
    } else {
      copy->info = evar_b_sample->info;
    }

    move_left(&evar_b_sample, &evar_e_sample);
  }

  assert(bracket_stack == 0);

  add_copy_tevar_time(clock() - start_copy_time, state);
}


void r05_alloc_chars(const char buffer[], size_t len, struct r05_state *state) {
  size_t i;
  for (i = 0; i < len; ++i) {
    r05_alloc_char(buffer[i], state);
  }
}


void r05_alloc_tvar(struct r05_node *sample, struct r05_state *state) {
  if (is_open_bracket(sample)) {
    struct r05_node *end_of_sample = sample->info.link;
    copy_nonempty_evar(sample, end_of_sample, state);
  } else {
    r05_alloc_svar(sample, state);
  }
}


void r05_alloc_evar(
  struct r05_node *sample_b, struct r05_node *sample_e,
  struct r05_state *state
) {
  if (! r05_empty_seq(sample_b, sample_e)) {
    copy_nonempty_evar(sample_b, sample_e, state);
  }
}


void r05_alloc_string(const char *string, struct r05_state *state) {
  for (/* пусто */; *string != '\0'; ++string) {
    r05_alloc_char(*string, state);
  }
}


void r05_link_brackets(struct r05_node *left, struct r05_node *right) {
  left->info.link = right;
  right->info.link = left;
}


void r05_splice_tvar(struct r05_node *res, struct r05_node *var) {
  struct r05_node *var_end;
  if (is_open_bracket(var)) {
    var_end = var->info.link;
  } else {
    var_end = var;
  }

  list_splice(res, var, var_end);
}

void r05_splice_evar(
  struct r05_node *res, struct r05_node *begin, struct r05_node *end
) {
  list_splice(res, begin, end);
}


void r05_splice_to_freelist(
  struct r05_node *begin, struct r05_node *end, struct r05_state *state
) {
  state->free_ptr = state->begin_free_list.next;
  state->free_ptr = list_splice(state->free_ptr, begin, end);
}


void r05_splice_from_freelist(struct r05_node *pos, struct r05_state *state) {
  if (state->free_ptr != state->begin_free_list.next) {
    list_splice(pos, state->begin_free_list.next, state->free_ptr->prev);
  }
}


void r05_enum_function_code(
  struct r05_node *begin, struct r05_node *end, struct r05_state *state
) {
  (void) begin;
  (void) end;
  r05_recognition_impossible(state);
}


/*==============================================================================
   Внутренний профилировщик
==============================================================================*/



static void start_profiler(struct r05_state *state) {
  state->start_program_time = clock();
  state->in_generated = 0;
}


static void start_building_result(struct r05_state *state) {
  if (state->in_generated) {
    clock_t pattern_match;

    state->start_building_result_time = clock();
    pattern_match =
      state->start_building_result_time - state->start_pattern_match_time;
    state->total_pattern_match_time += pattern_match;

    if (state->in_e_loop > 0) {
      state->total_e_loop +=
        (state->start_building_result_time - state->start_e_loop);
      state->in_e_loop = 0;
    }
  }
}


static void after_step(struct r05_state *state) {
  if (state->in_generated) {
    clock_t building_result = clock() - state->start_building_result_time;
    state->total_building_result_time += building_result;
  }

  assert(state->in_e_loop == 0);

  state->in_generated = 0;
  state->in_e_loop = 0;
}


static void add_copy_tevar_time(clock_t duration, struct r05_state *state) {
  state->total_copy_tevar_time += duration;
}

static void add_match_repeated_tvar_time(
  clock_t duration, struct r05_state *state
) {
  if (state->in_e_loop) {
    state->total_match_repeated_tvar_time += duration;
  } else {
    state->total_match_repeated_tvar_time_outside_e += duration;
  }
}

static void add_match_repeated_evar_time(
  clock_t duration, struct r05_state *state
) {
  if (state->in_e_loop) {
    state->total_match_repeated_evar_time += duration;
  } else {
    state->total_match_repeated_evar_time_outside_e += duration;
  }
}

#ifdef R05_SHOW_STAT
struct time_item {
  const char *name;
  clock_t counter;
};

static int reverse_compare(const void *left_void, const void *right_void) {
  const struct time_item *left = left_void;
  const struct time_item *right = right_void;

  if (left->counter > right->counter) {
    return -1;
  } else if (left->counter < right->counter) {
    return +1;
  } else {
    return 0;
  }
}

static void print_profile(struct r05_state *state) {
  const double cfSECS_PER_CLOCK = 1.0 / CLOCKS_PER_SEC;

  clock_t full_time;
  clock_t refal_time;
  clock_t repeated_time;
  clock_t eloop_time;
  clock_t repeated_time_outside_e;

  enum { nItems = 11 };
  struct time_item items[nItems];

  size_t i;

  full_time = clock() - state->start_program_time;
  refal_time =
    state->total_pattern_match_time + state->total_building_result_time;
  repeated_time =
    state->total_match_repeated_tvar_time
    + state->total_match_repeated_evar_time;
  eloop_time = state->total_e_loop - repeated_time;
  repeated_time_outside_e =
    state->total_match_repeated_tvar_time_outside_e
    + state->total_match_repeated_evar_time_outside_e;

  /*
    Ложное предупреждение BCC 5.5:
    компилятор не допускает инициализацию структур и массивов переменными.
  */
  items[0].name = "\nTotal program time";
  items[0].counter = full_time;
  items[1].name = "Builtin time";
  items[1].counter = full_time - refal_time;
  items[2].name = "(Total refal time)";
  items[2].counter = refal_time;
  items[3].name = "Linear pattern time";
  items[3].counter = state->total_pattern_match_time
    - (eloop_time + repeated_time + repeated_time_outside_e);
  items[4].name = "Linear result time";
  items[4].counter =
    state->total_building_result_time - state->total_copy_tevar_time;
  items[5].name = "Open e-loop time (clear)";
  items[5].counter = eloop_time;
  items[6].name = "Repeated e-var match time (inside e-loops)";
  items[6].counter = state->total_match_repeated_evar_time;
  items[7].name = "Repeated e-var match time (outside e-loops)";
  items[7].counter = state->total_match_repeated_tvar_time_outside_e;
  items[8].name = "Repeated t-var match time (inside e-loops)";
  items[8].counter = state->total_match_repeated_tvar_time;
  items[9].name = "Repeated t-var match time (outside e-loops)";
  items[9].counter = state->total_match_repeated_tvar_time_outside_e;
  items[10].name = "t- and e-var copy time";
  items[10].counter = state->total_copy_tevar_time;

  qsort(items, nItems, sizeof(items[0]), reverse_compare);

  for (i = 0; i < nItems; ++i) {
    unsigned long value = items[i].counter;

    if (value > 0) {
      double percent = (full_time != 0) ? 100.0 * value / full_time : 0.0;
      fprintf(
        stderr, "%s: %.3f seconds (%.1f %%).\n",
        items[i].name, value * cfSECS_PER_CLOCK, percent
      );
    }
  }
}

#endif  /* R05_SHOW_STAT */

static void end_profiler(struct r05_state *state) {
  after_step(state);

#ifdef R05_SHOW_STAT
  print_profile(state);
#endif  /* R05_SHOW_STAT */
}


void r05_start_e_loop(struct r05_state *state) {
  assert(state->in_generated);

  if (state->in_e_loop++ == 0) {
    state->start_e_loop = clock();
  }
}


void r05_this_is_generated_function(struct r05_state *state) {
  state->start_pattern_match_time = clock();
  state->in_generated = 1;
}


void r05_stop_e_loop(struct r05_state *state) {
  assert(state->in_generated);

  if (--state->in_e_loop == 0) {
    state->total_e_loop += (clock() - state->start_e_loop);
  }
}


/*==============================================================================
   Операции для работы с А-термами
==============================================================================*/

/* временно А-термы создаются через malloc и с использованием стека скобок */
/* позже стек скобок будет убран и будет создан список свободных А-термов */
static void alloc_and_push_aterm_list(
  struct r05_node *arg_begin, struct r05_node *arg_end,
  struct r05_aterm *parent, struct r05_state *state
) {
  struct r05_aterm *aterm;
  aterm = malloc(sizeof(*aterm));
  aterm->arg_begin = arg_begin;
  aterm->arg_end = arg_end;
  aterm->parent = parent;
  aterm->next = state->aterm_list_ptr;
  state->aterm_list_ptr = aterm;
}

static struct r05_aterm *pop_aterm_list(struct r05_state *state) {
  struct r05_aterm *res = state->aterm_list_ptr;
  state->aterm_list_ptr = res->next;
  return res;
}

static int empty_aterm_list(struct r05_state *state) {
  return (state->aterm_list_ptr == NULL);
}

/* Стэк угловых скобок, временно оставлен */
static struct r05_node *s_stack_ptr = NULL;


static struct r05_node *pop_stack(void) {
  struct r05_node *res = s_stack_ptr;
  s_stack_ptr = s_stack_ptr->info.link;
  return res;
}

void r05_push_stack(struct r05_node *call_bracket, struct r05_state *state) {
  if (call_bracket->tag == R05_DATATAG_OPEN_CALL) {
    struct r05_node *close_bracket = pop_stack();
    alloc_and_push_aterm_list(call_bracket, close_bracket, NULL, state);
  } else {
    call_bracket->info.link = s_stack_ptr;
    s_stack_ptr = call_bracket;
  }
}


/*==============================================================================
   Рефал-машина
==============================================================================*/

static struct r05_node s_end_view_field;

static struct r05_node s_begin_view_field = {
  0, &s_end_view_field, R05_DATATAG_ILLEGAL, { '\0' }
};
static struct r05_node s_end_view_field = {
  &s_begin_view_field, 0, R05_DATATAG_ILLEGAL, { '\0' }
};


extern struct r05_function r05f_Go;

static void init_view_field(struct r05_state *state) {
  struct r05_node *open, *close;

  r05_reset_allocator(state);
  r05_alloc_open_call(&open, state);
  r05_alloc_function(&r05f_Go, state);
  r05_alloc_close_call(&close, state);
  r05_push_stack(close, state);
  r05_push_stack(open, state);
  r05_splice_from_freelist(s_begin_view_field.next, state);
}

static void main_loop(struct r05_state *state) {
  while (! empty_aterm_list(state)) {
    struct r05_node *function;
    struct r05_aterm *next_aterm;

    next_aterm = pop_aterm_list(state);
    state->arg_begin = next_aterm->arg_begin;
    state->arg_end = next_aterm->arg_end;

#if R05_SHOW_DEBUG
    if (state->step_counter >= (unsigned long) R05_SHOW_DEBUG) {
      make_dump(&state);
    }
#endif  /* R05_SHOW_DEBUG */

    function = state->arg_begin->next;
    if (R05_DATATAG_FUNCTION == function->tag) {
      (function->info.function->ptr)(
        state->arg_begin, state->arg_end, state
      );
    } else {
      r05_recognition_impossible(state);
    }
    after_step(state);

    ++ state->step_counter;
  }
}


static void print_indent(int level) {
  enum { cPERIOD = 4 };
  int i;

  putc('\n', stderr);

  if (level < 0) {
    putc('!', stderr);
    return;
  }

  for (i = 0; i < level; ++i) {
    /* Каждые cPERIOD позиций вместо пробела ставим точку. */
    int put_marker = ((i % cPERIOD) == (cPERIOD - 1));

    const char cSpace =  ' ';
    const char cMarker = '.';

    putc((put_marker ? cMarker : cSpace), stderr);
  }
}


static void print_seq(struct r05_node *begin, struct r05_node *end) {
  enum {
    cStateView = 100,
    cStateString,
    cStateFinish
  } state = cStateView;

  int indent = 0;
  int after_bracket = 0;
  int reset_after_bracket = 1;

  while ((state != cStateFinish) && ! r05_empty_seq(begin, end)) {
    if (reset_after_bracket) {
      after_bracket = 0;
      reset_after_bracket = 0;
    }

    if (after_bracket) {
      reset_after_bracket = 1;
    }

    switch (state) {
      case cStateView:
        switch (begin->tag) {
          case R05_DATATAG_ILLEGAL:
            if (0 == begin->prev) {
              fprintf(stderr, "[FIRST] ");
            } else if (0 == begin->next) {
              fprintf(stderr, "\n[LAST]");
              state = cStateFinish;
            } else {
              fprintf(stderr, "\n[NONE]");
            }
            move_left(&begin, &end);
            continue;

          case R05_DATATAG_CHAR:
            state = cStateString;
            fprintf(stderr, "\'");
            continue;

          case R05_DATATAG_NUMBER:
            fprintf(stderr, "%lu ", begin->info.number);
            move_left(&begin, &end);
            continue;

          case R05_DATATAG_FUNCTION:
            fprintf(stderr, "%s ", begin->info.function->name);
            move_left(&begin, &end);
            continue;

          case R05_DATATAG_OPEN_BRACKET:
            if (! after_bracket) {
              print_indent(indent);
            }
            ++indent;
            after_bracket = 1;
            reset_after_bracket = 0;
            fprintf(stderr, "(");
            move_left(&begin, &end);
            continue;

          case R05_DATATAG_CLOSE_BRACKET:
            --indent;
            fprintf(stderr, ")");
            move_left(&begin, &end);
            continue;

          case R05_DATATAG_OPEN_CALL:
            if (! after_bracket) {
              print_indent(indent);
            }
            ++indent;
            after_bracket = 1;
            reset_after_bracket = 0;
            fprintf(stderr, "<");
            move_left(&begin, &end);
            continue;

          case R05_DATATAG_CLOSE_CALL:
            --indent;
            fprintf(stderr, ">");
            move_left(&begin, &end);
            continue;

          default:
            r05_switch_default_violation(begin->tag);
        }

      case cStateString:
        switch (begin->tag) {
          case R05_DATATAG_CHAR: {
            unsigned char ch = begin->info.char_;
            switch (ch) {
              case '(': case ')':
              case '<': case '>':
                fprintf(stderr, "\\%c", ch);
                break;

              case '\n':
                fprintf(stderr, "\\n");
                break;

              case '\t':
                fprintf(stderr, "\\t");
                break;

              case '\\':
                fprintf(stderr, "\\\\");
                break;

              case '\'':
                fprintf(stderr, "\\\'");
                break;

              default:
                if (ch < '\x20') {
                  fprintf(stderr, "\\x%02x", ch);
                } else {
                  fprintf(stderr, "%c", ch);
                }
                break;
              }
              move_left(&begin, &end);
              continue;
            }

          default:
            state = cStateView;
            fprintf(stderr, "\' ");
            continue;
        }

      case cStateFinish:
        continue;

      default:
        r05_switch_default_violation(state);
    }
  }

  if (cStateString == state) {
    fprintf(stderr, "\'");
  }
}


static void dump_buried(struct r05_state *state);

static void make_dump(struct r05_state *state) {
  fprintf(stderr, "\nSTEP NUMBER %lu\n", state->step_counter);
  fprintf(stderr, "\nPRIMARY ACTIVE EXPRESSION:\n");
  print_seq(state->arg_begin, state->arg_end);
  fprintf(stderr, "\nVIEW FIELD:\n");
  print_seq(&s_begin_view_field, &s_end_view_field);

  dump_buried(state);

#ifdef R05_DUMP_FREE_LIST
  fprintf(stderr, "\nFREE LIST:\n");
  print_seq(&state->begin_free_list, &state->end_free_list);
#endif  /* ifdef R05_DUMP_FREE_LIST */

  fprintf(stderr,"\nEnd dump\n");
  fflush(stderr);
}


R05_NORETURN void r05_exit(int retcode, struct r05_state *state) {
  dump_buried(state);
  fflush(stderr);
  fflush(stdout);
  end_profiler(state);

#ifdef R05_SHOW_STAT
  fprintf(stderr, "Step count %lu\n", state->step_counter);
#endif  /* R05_SHOW_STAT */

  free_memory(state);
  fflush(stdout);

  exit(retcode);
}


R05_NORETURN void r05_recognition_impossible(struct r05_state *state) {
  fprintf(stderr, "\nRECOGNITION IMPOSSIBLE\n\n");
  make_dump(state);
  r05_exit(EXIT_CODE_RECOGNITION_IMPOSSIBLE, state);
}


R05_NORETURN void r05_builtin_error(const char *message,
  struct r05_state *state
) {
  fprintf(stderr, "\nBUILTIN FUNCTION ERROR: %s\n\n", message);
  make_dump(state);
  r05_exit(EXIT_CODE_RECOGNITION_IMPOSSIBLE, state);
}


R05_NORETURN void r05_builtin_error_errno(
  const char *message,
  struct r05_state *state
) {
  fprintf(
    stderr, "\nBUILTIN FUNCTION ERROR: %s\n(errno = %d: %s)\n\n",
    message, errno, strerror(errno)
  );
  make_dump(state);
  r05_exit(EXIT_CODE_RECOGNITION_IMPOSSIBLE, state);
}


static char **s_argv = 0;
static int s_argc = 0;

const char *r05_arg(int no) {
  if (no < s_argc) {
    return s_argv[no];
  } else {
    return "";
  }
}


R05_NORETURN void r05_switch_default_violation_impl(
  const char *expr, long value, const char *file, int line
) {
  fprintf(stderr, "%s:%d:SWITCH DEFAULT VIOLATION\n", file, line);
  fprintf(stderr, "    expression %s -> %ld is not handled\n", expr, value);
  abort();
}


/*==============================================================================
   Копилка
==============================================================================*/


struct buried_query {
  struct r05_node *left_bracket;
  struct r05_node *right_bracket;
  struct r05_node *value_begin;
  struct r05_node *value_end;
};

int buried_query(
  struct buried_query *res, struct r05_node *key_begin,
  struct r05_node *key_end, struct r05_state *state
) {
  struct r05_node *buried_begin = state->begin_buried.next;
  struct r05_node *left_bracket, *right_bracket;
  struct r05_node *in_brackets_b, *in_brackets_e;
  int found = 0;

  while (buried_begin != &state->end_buried && ! found) {
    struct r05_node *rep_key_b, *rep_key_e;

    left_bracket = buried_begin;
    right_bracket = left_bracket->info.link;
    in_brackets_b = left_bracket->next;
    in_brackets_e = right_bracket->prev;

    found =
      r05_repeated_evar_left(
        &rep_key_b, &rep_key_e,
        key_begin, key_end,
        &in_brackets_b, &in_brackets_e,
        state
      )
      && r05_char_left('=', &in_brackets_b, &in_brackets_e, state);

    buried_begin = right_bracket->next;
  }

  if (found) {
    res->left_bracket = left_bracket;
    res->right_bracket = right_bracket;
    res->value_begin = in_brackets_b;
    res->value_end = in_brackets_e;
  }

  return found;
}


enum brrp_behavior { BRRP_BR, BRRP_RP };

static void brrp_impl(
  struct r05_node *arg_begin, struct r05_node *arg_end,
  enum brrp_behavior behavior, struct r05_state *state
) {
  struct r05_node *callee = arg_begin->next;
  struct r05_node *key_b, *key_e, *val_b, *val_e;

  key_b = NULL;
  key_e = NULL;
  r05_prepare_argument(&val_b, &val_e, arg_begin, arg_end);
  do {
    if (r05_char_left('=', &val_b, &val_e, state)) {
      struct buried_query query;

      if (BRRP_RP == behavior && buried_query(&query, key_b, key_e, state)) {
        list_splice(query.right_bracket, val_b, val_e);
        list_splice(arg_end, query.value_begin, query.value_end);
      } else {
        struct r05_node *left_bracket = callee;
        struct r05_node *right_bracket = arg_end;
        left_bracket->tag = R05_DATATAG_OPEN_BRACKET;
        right_bracket->tag = R05_DATATAG_CLOSE_BRACKET;
        r05_link_brackets(left_bracket, right_bracket);
        list_splice(state->begin_buried.next, left_bracket, right_bracket);
        arg_end = arg_begin;
      }
      r05_splice_to_freelist(arg_begin, arg_end, state);
      return;
    }
  } while (r05_open_evar_advance(&key_b, &key_e, &val_b, &val_e, state));

  r05_recognition_impossible(state);
}


enum dgcp_behavior { DGCP_DG, DGCP_CP };

static void dgcp_impl(
  struct r05_node *arg_begin, struct r05_node *arg_end,
  enum dgcp_behavior behavior, struct r05_state *state
) {
  struct r05_node *key_begin, *key_end;
  struct buried_query query;
  int found;

  r05_prepare_argument(&key_begin, &key_end, arg_begin, arg_end);
  r05_reset_allocator(state);

  found = buried_query(&query, key_begin, key_end, state);

  if (found) {
    if (behavior == DGCP_DG) {
      r05_splice_evar(arg_begin, query.value_begin, query.value_end);
      r05_splice_to_freelist(query.left_bracket, query.right_bracket, state);
    } else {
      r05_alloc_evar(query.value_begin, query.value_end, state);
    }
  }

  r05_splice_from_freelist(arg_begin, state);
  r05_splice_to_freelist(arg_begin, arg_end, state);
}


void r05_br(
  struct r05_node *arg_begin, struct r05_node *arg_end,
  struct r05_state *state
) {
  brrp_impl(arg_begin, arg_end, BRRP_BR, state);
}

void r05_dg(
  struct r05_node *arg_begin, struct r05_node *arg_end,
  struct r05_state *state
) {
  dgcp_impl(arg_begin, arg_end, DGCP_DG, state);
}

void r05_cp(
  struct r05_node *arg_begin, struct r05_node *arg_end,
  struct r05_state *state
) {
  dgcp_impl(arg_begin, arg_end, DGCP_CP, state);
}

void r05_rp(struct r05_node *arg_begin, struct r05_node *arg_end,
  struct r05_state *state
) {
  brrp_impl(arg_begin, arg_end, BRRP_RP, state);
}


static void dump_buried(struct r05_state *state) {
#ifdef R05_DUMP_BURIED
  fprintf(stderr, "\nBURIED:\n");
  print_seq(&state->begin_buried, &state->end_buried);
#endif  /* ifdef R05_DUMP_BURIED */
}


/*==============================================================================
   Функция конкатенации
==============================================================================*/

R05_DEFINE_ENTRY_FUNCTION(Cata_, "Cat@") { /* @ декодируется в a_ */
  r05_this_is_generated_function(state);

  do {
    struct r05_node *eX_b_1;
    struct r05_node *eX_e_1;
    struct r05_node *eY_b_1;
    struct r05_node *eY_e_1;
    struct r05_node *bb[2] = { 0 };
    struct r05_node *be[2] = { 0 };
    struct r05_node *n[1] = { 0 };
    r05_prepare_argument(bb+0, be+0, arg_begin, arg_end);
    /* (e.X) e.Y */
    if (! r05_brackets_left(bb+1, be+1, bb+0, be+0, state))
      continue;
    eX_b_1 = bb[1];
    eX_e_1 = be[1];
    eY_b_1 = bb[0];
    eY_e_1 = be[0];

    r05_reset_allocator(state);
    r05_alloc_insert_pos(n+0, state);
    r05_splice_evar(n[0], eX_b_1, eX_e_1);
    r05_splice_evar(n[0], eY_b_1, eY_e_1);
    r05_splice_from_freelist(arg_begin, state);
    r05_splice_to_freelist(arg_begin, arg_end, state);
    return;
  } while (0);

  r05_recognition_impossible(state);
}

int main(int argc, char **argv) {
  s_argc = argc;
  s_argv = argv;

  struct r05_state state = {
    /* begin_free_list */
    {NULL, NULL, R05_DATATAG_ILLEGAL, { '\0' }},
    /* end_free_list */
    {NULL, NULL, R05_DATATAG_ILLEGAL, { '\0' }},
    /* later will be &end_free_list */
    NULL,
    /* arg_begin */
    NULL, 
    /* arg_end */
    NULL, 
    /* pool of memory_chunks */
    NULL,
    /* aterm_list_ptr */
    NULL,
    /* begin_buried */
    {NULL, NULL, R05_DATATAG_ILLEGAL, { '\0' }},
    /* end_buried */
    {NULL, NULL, R05_DATATAG_ILLEGAL, { '\0' }},
    /* memory_use */
    0,
    /* step_counter */
    0 
  };

  state.free_ptr = &state.end_free_list;
  weld(&state.begin_free_list, &state.end_free_list);
  weld(&state.begin_buried, &state.end_buried);

  init_view_field(&state);
  start_profiler(&state);
  main_loop(&state);
  r05_exit(0, &state);

#ifndef R05_NORETURN_DEFINED
  return 0;
#endif
}
