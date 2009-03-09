/*
 * Copyright (C) 2009 Raphael Kubo da Costa <kubito@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <pcre.h>
#include <pthread.h>
#include <string.h>
#include "config.h"
#include "game.h"
#include "mem.h"

/**
 * Returns the string matched by the first subgroup pattern in a regular expression.
 *
 * This function is useful if the regular expression is used primarily to find
 * one single pattern inside the given string.
 * Please note that it's up to the programmer to specify a regular expression with
 * at least one subgroup.
 *
 * @param pattern The regular expression to use.
 * @param subject The string to be searched.
 *
 * @return The matched string.
 */
static char *__re_get_first_match(const char *pattern, const char *subject)
{
  int erroffset;
  const char *error;
  char *match;
  int ovector[30];
  int rc;
  pcre *re;

  re = pcre_compile(pattern, PCRE_NEWLINE_LF, &error, &erroffset, NULL);
  if (re == NULL)
    return NULL;

  rc = pcre_exec(re, NULL, subject, strlen(subject), 0, 0, ovector, 30);
  if (rc <= 0)
    return NULL;

  match = MEM_ALLOC_N(char, ovector[3] - ovector[2] + 1);
  strncpy(match, subject + ovector[2], ovector[3] - ovector[2]);

  pcre_free(re);

  return match;
}

/**
 * Parses the custom board file format.
 *
 * @param game  Pointer to a Game structure.
 * @param board The file to read.
 *
 * @retval 0 The file was parsed correctly.
 * @retval 1 The file could not be parsed.
 */
static int __parse_custom_format(Game *game, FILE * board)
{
  char boardline_re[20];
  char *endptr;
  char header_line[16];
  size_t i, j;
  char *line;
  char *s;

  /* First line - "Rows:NNN" */
  fgets(header_line, 16, board);
  s = __re_get_first_match("^Rows:(\\d{1,10})$", header_line);
  if (!s) {
    free(s);
    return 1;
  }
  game->rows = (size_t) strtol(s, &endptr, 10);
  if (*endptr != '\0') {
    free(s);
    return 1;
  } else {
    free(s);
  }

  /* Second line - "Cols:NNN" */
  fgets(header_line, 16, board);
  s = __re_get_first_match("^Cols:(\\d{1,10})$", header_line);
  if (!s) {
    free(s);
    return 1;
  }
  game->cols = (size_t) strtol(s, &endptr, 10);
  if (*endptr != '\0') {
    free(s);
    return 1;
  } else {
    free(s);
  }

  /* Allocate memory for the board */
  if (game->board)
    free(game->board);
  game->board = MEM_ALLOC_N(char, game->cols * game->rows);

  /* Read game->rows lines describing the board */
  sprintf(boardline_re, "^([#.]{%u})$", game->cols);
  line = MEM_ALLOC_N(char, game->cols + 2);
  for (i = 0; i < game->rows; i++) {
    fgets(line, game->cols + 2, board);

    s = __re_get_first_match(boardline_re, line);
    if (!s) {
      free(line);
      free(s);
      return 1;
    }

    for (j = 0; j < game->cols; j++) {
      if (s[j] == '#')
        game_set_alive(game, i, j);
      else
        game_set_dead(game, i, j);
    }

    free(s);
  }

  free(line);

  return 0;
}

/**
 * Analyzes a particular part of the game board and update its state to the next generation.
 *
 * @param t Pointer to a __ThreadInfo structure.
 */
static void *__process_slice(void *t)
{
  char live_count;
  size_t row, col;
  __ThreadInfo *tinfo = (__ThreadInfo*)t;

  for (col = tinfo->col; (col < (tinfo->col + tinfo->width)) && (col < tinfo->game->cols); col++) {
    for (row = 0; row < tinfo->game->rows; row++) {
      live_count = 0;

      /* Count the living neighbour cells */
      if (game_is_alive(tinfo->game, row, col+1))   live_count++;
      if (game_is_alive(tinfo->game, row+1, col))   live_count++;
      if (game_is_alive(tinfo->game, row+1, col+1)) live_count++;
      if (row > 0) {
        if (game_is_alive(tinfo->game, row-1, col))   live_count++;
        if (game_is_alive(tinfo->game, row-1, col+1)) live_count++;
      }
      if (col > 0) {
        if (game_is_alive(tinfo->game, row, col-1))   live_count++;
        if (game_is_alive(tinfo->game, row+1, col-1)) live_count++;
      }
      if ((row > 0) && (col > 0))
        if (game_is_alive(tinfo->game, row-1, col-1)) live_count++;

      /* Apply the game's rules to the current cell */
      if ((live_count < 2) || (live_count > 3))
        tinfo->new_board[row * tinfo->game->cols + col] = 0;
      else if (live_count == 3)
        tinfo->new_board[row * tinfo->game->cols + col] = 1;
      else
        tinfo->new_board[row * tinfo->game->cols + col] = tinfo->game->board[row * tinfo->game->cols + col];
    }
  }

  return NULL;
}

void game_free(Game *game)
{
  if (game) {
    if (game->board)
      free(game->board);
    free(game);
  }
}

int game_is_alive(Game *game, size_t row, size_t col)
{
  assert(game);
  assert(game->board);

  if ((row >= game->rows) || (col >= game->cols))
    return 0;

  return game->board[row * game->cols + col] == 1;
}

int game_is_dead(Game *game, size_t row, size_t col)
{
  return !game_is_alive(game, row, col);
}

Game *game_new(void)
{
  Game *game = MEM_ALLOC(Game);

  game->board = NULL;
  game->cols = 0;
  game->rows = 0;

  return game;
}

int game_parse_board(Game *game, GameConfig *config)
{
  FILE *board;
  int exit_code;
  long input_file_pos;

  assert(game);
  assert(config);
  assert(config->input_file);

  board = config->input_file;

  input_file_pos = ftell(board);
  fseek(board, 0, SEEK_SET);

  exit_code = __parse_custom_format(game, board);

  fseek(board, input_file_pos, SEEK_SET);

  return exit_code;
}

void game_print_board(Game *game)
{
  size_t col, row;

  assert(game);
  assert(game->board);

  for (row = 0; row < game->rows; row++) {
    for (col = 0; col < game->cols; col++) {
      printf("%c", game_is_alive(game, row, col) ? '#' : '.');
    }
    printf("\n");
  }
}

void game_set_alive(Game *game, size_t row, size_t col)
{
  assert(game);
  assert(game->board);
  assert(row < game->rows);
  assert(col < game->cols);

  game->board[row * game->cols + col] = 1;
}

void game_set_dead(Game *game, size_t row, size_t col)
{
  assert(game);
  assert(game->board);
  assert(row < game->rows);
  assert(col < game->cols);

  game->board[row * game->cols + col] = 0;
}

int game_tick(Game *game)
{
  char *new_board;
  int retval = 0;
  size_t slice_count;
  __ThreadInfo *tinfo;
  size_t tnum = 0;

  slice_count = (game->cols / BOARD_SLICE_WIDTH) + (game->cols % BOARD_SLICE_WIDTH ? 1 : 0);

  new_board = MEM_ALLOC_N(char, game->rows * game->cols);
  tinfo = MEM_ALLOC_N(__ThreadInfo, slice_count);

  for (tnum = 0; tnum < slice_count; tnum++) {
    tinfo[tnum].col = tnum * BOARD_SLICE_WIDTH;
    tinfo[tnum].game = game;
    tinfo[tnum].new_board = new_board;
    tinfo[tnum].width = BOARD_SLICE_WIDTH;

    if (pthread_create(&tinfo[tnum].tid, NULL, &__process_slice, &tinfo[tnum])) {
      fprintf(stderr, "Error while creating thread %u. Waiting for other threads to finish.\n", tnum);
      retval = 1;
    }
  }

  for (tnum = 0; tnum < slice_count; tnum++) {
    if (pthread_join(tinfo[tnum].tid, NULL))
      retval = 1;
  }

  /* Make game use the new board and drop the old one */
  free(game->board);
  game->board = new_board;

  free(tinfo);

  return retval;
}
