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

#include <pthread.h>
#include <stdlib.h>

#define BOARD_SLICE_WIDTH 1

/**
 * The main structure used by the game.
 */
typedef struct {
  char *board; /**< The board as an array of 0's and 1's. */
  size_t cols; /**< The number of columns. */
  size_t rows; /**< The number of rows. */
} Game;

/**
 * Internal structure with information needed by a tick thread.
 */
typedef struct {
  size_t col;      /**< The board column to start in. */
  Game *game;      /**< The Game being used. */
  char *new_board; /**< The new board that will be worked on. */
  pthread_t tid;   /**< A thread id. */
  size_t width;    /**< The width of the stripe of the board being used. */
} __ThreadInfo;

/**
 * Frees memory allocated to a Game structure.
 *
 * @param game Pointer to be freed.
 */
void game_free(Game *game);

/**
 * Checks whether a given board position is in an alive state.
 *
 * @param game Pointer to a Game structure.
 * @param row  The row number.
 * @param col  The column number.
 *
 * @retval 0 The position is in a dead state.
 * @retval 1 The position is in an alive state.
 */
int game_is_alive(Game *game, size_t row, size_t col);

/**
 * Checks whether a given board position is in a dead state.
 *
 * @param game Pointer to a Game structure.
 * @param row  The row number.
 * @param col  The column number.
 *
 * @retval 0 The position is in an alive state.
 * @retval 1 The position is in a dead state.
 */
int game_is_dead(Game *game, size_t row, size_t col);

/**
 * Allocates memory for a new Game structure.
 *
 * @return A new Game pointer.
 */
Game *game_new(void);

/**
 * Parses a board file into an internal representation.
 *
 * Currently, this function only parses the custom file format
 * used by the program, but it should be trivial to add other
 * file formats.
 *
 * @param game Pointer to a Game structure.
 * @param config Pointer to a GameConfig structure.
 *
 * @retval 0 The board file was parsed correctly.
 * @retval 1 The board file could not be parsed.
 */
int game_parse_board(Game *game, GameConfig *config);

/**
 * Prints the current state of the board.
 *
 * @param game Pointer to a Game structure.
 */
void game_print_board(Game *game);

/**
 * Sets a specific position in the board to an alive state.
 *
 * @param game Pointer to a Game structure.
 * @param row  The row number.
 * @param col  The column number.
 */
void game_set_alive(Game *game, size_t row, size_t col);

/**
 * Sets a specific position in the board to a dead state.
 *
 * @param game Pointer to a Game structure.
 * @param row  The row number.
 * @param col  The column number.
 */
void game_set_dead(Game *game, size_t row, size_t col);

/**
 * Advances the cell board to a new generation (causes a 'tick').
 *
 * @param game Pointer to a Game structure.
 *
 * @retval 0 The tick has happened successfully.
 * @retval 1 The tick could not happen correctly.
 */
int game_tick(Game *game);

#endif
