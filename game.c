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
#include <string.h>
#include "config.h"
#include "game.h"
#include "mem.h"

void game_free(Game* game)
{
  if (game) {
    if (game->board)
      free(game->board);
    free(game);
  }
}

Game* game_new(void)
{
  Game* game = MEM_ALLOC(Game);

  return game;
}

static char* __re_get_first_match(const char* pattern, const char* subject)
{
  int   erroffset;
  const char* error;
  char* match;
  int   ovector[30];
  int   rc;
  pcre* re;

  re = pcre_compile(pattern, PCRE_NEWLINE_LF, &error, &erroffset, NULL);
  if (re == NULL)
    return NULL;

  rc = pcre_exec(re, NULL, subject, strlen(subject), 0, 0, ovector, 30);
  if (rc <= 0)
    return NULL;

  match = MEM_ALLOC_N(char, ovector[3] - ovector[2]);
  strncpy(match, subject + ovector[2], ovector[3] - ovector[2]);

  pcre_free(re);

  return match;
}

static int __parse_custom_format(Game* game, FILE* board)
{
  char   boardline_re[20];
  char*  endptr;
  char   header_line[16];
  size_t i;
  char*  line;
  char*  s;

  fgets(header_line, 16, board);
  s = __re_get_first_match("^Rows:(\\d{1,10})$", header_line);
  if (!s) {
    free(s);
    return 1;
  }
  game->rows = (size_t)strtol(s, &endptr, 10);
  if (*endptr != '\0') {
    free(s);
    return 1;
  }

  fgets(header_line, 16, board);
  s = __re_get_first_match("^Cols:(\\d{1,10})$", header_line);
  if (!s) {
    free(s);
    return 1;
  }
  game->cols = (size_t)strtol(s, &endptr, 10);
  if (*endptr != '\0') {
    free(s);
    return 1;
  }

  /* Allocate memory for the board */
  if (game->board)
    free(game->board);
  game->board = MEM_ALLOC_N(char, game->cols * game->rows);

  /* Read game->rows lines describind the board */
  sprintf(boardline_re, "^([#.]{%u})$", game->cols);
  line = MEM_ALLOC_N(char, game->cols);

  for (i = 0; i < game->rows; i++) {
    fgets(line, game->cols + 2, board);
    s = __re_get_first_match(boardline_re, line);
    if (s)
      printf("%s\n", s);
  }

  free(line);
  free(s);

  return 0;
}

int game_parse_board(Game* game, GameConfig* config)
{
  FILE* board;
  int   exit_code;
  long  input_file_pos;

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
