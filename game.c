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
#include "config.h"
#include "game.h"
#include "mem.h"

void game_free(Game* game)
{
  if (game) {
    free(game);
  }
}

Game* game_new(void)
{
  Game* game = MEM_ALLOC(Game);

  return game;
}

int game_parse_board(Game* game, GameConfig* config)
{
  assert(game);
  assert(config);

  return 0;
}
