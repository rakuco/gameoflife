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

#ifndef __MEM_H
#define __MEM_H

#include <stdlib.h>

/**
 * Wrapper over MEM_ALLOC_N. Equivalent to allocating
 * memory for 1 entry of type /type/.
 */
#define MEM_ALLOC(type) (MEM_ALLOC_N(type, 1))

/**
 * Allocates memory for numelem elements of the given type.
 */
#define MEM_ALLOC_N(type, numelem) ((type*)__memAllocate(numelem, sizeof(type)))

/**
 * This function should not be called directly. It's a wrapper
 * over calloc.
 * If memory can't be properly allocated, it displays an error
 * message and exits the program.
 *
 * @param numelem The number of the elements in the array.
 * @param elemsize The size of each element.
 */
void *__memAllocate(size_t numelem, size_t elemsize);

#endif
