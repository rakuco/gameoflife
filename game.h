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

#ifndef __GAME_H
#define __GAME_H

typedef struct {
  char *board;
  size_t cols;
  size_t rows;
} Game;

void game_free(Game *game);
int game_is_alive(Game *game, size_t row, size_t col);
int game_is_alive(Game *game, size_t row, size_t col);
Game *game_new(void);
int game_parse_board(Game *game, GameConfig *config);
void game_print_board(Game *game);
void game_set_alive(Game *game, size_t row, size_t col);
void game_set_dead(Game *game, size_t row, size_t col);
int game_tick(Game *game);

#endif
