/*
	Copyright (C) 2014-2021 Igor van den Hoven ivdhoven@gmail.com
*/

/*
	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the
	"Software"), to deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to
	the following conditions:

	The above copyright notice and this permission notice shall be
	included in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
	octosort 1.0
*/

/*
	octosort is based on WikiSort and quadsort

	WikiSort: https://github.com/BonzaiThePenguin/WikiSort
	quadsort: https://github.com/scandum/quadsort
	searches: https://github.com/scandum/binary_search
*/

#ifndef OCTOSORT_H
#define OCTOSORT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//#define cmp(a,b) (*(a) > *(b))

typedef int CMPFUNC (const void *a, const void *b);

// Set to 1 to see how it performs when given more memory

#define DYNAMIC_CACHE 0

// utilities

#define SWAP(value1, value2) {swap = value1;value1 = value2;value2 = swap;}

#define PULL(_to) \
	pull[pull_index].range = new_range(A.start, B.end); \
	pull[pull_index].count = count; \
	pull[pull_index].from = index; \
	pull[pull_index].to = _to

// not as fast as math.h's sqrt() but it's portable

size_t monobound_sqrt(const size_t size)
{
	size_t bot, mid, top, sum;

	bot = 0;
	top = 65536;

	while (top > 1)
	{
		mid = top / 2;
		sum = bot + mid;

		if (sum * sum <= size)
		{
			bot += mid;
		}
		top -= mid;
	}
	return bot;
}

size_t Min(const size_t a, const size_t b)
{
	return a < b ? a : b;
}

size_t Max(const size_t a, const size_t b)
{
	return a > b ? a : b;
}

// 63 -> 32, 64 -> 64, etc. this comes from Hacker's Delight

size_t FloorPowerOfTwo (const size_t value)
{
	size_t x = value;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
#if __LP64__
	x = x | (x >> 32);
#endif
	return x - (x >> 1);
}

// structure to represent ranges within the array

typedef struct
{
	size_t start;
	size_t end;
}
Range;

size_t range_length(Range range)
{
	return range.end - range.start;
}

Range new_range(const size_t start, const size_t end)
{
	return (Range) {start, end};
}


// calculate how to scale the index value to the range within the array
// the bottom-up merge sort only operates on values that are powers of two,
// so scale down to that power of two, then use a fraction to scale back again

typedef struct
{
	size_t size;
	size_t power_of_two;
	size_t numerator;
	size_t decimal;
	size_t denominator;
	size_t decimal_step;
	size_t numerator_step;
}
WikiIterator;

void WikiIterator_begin(WikiIterator *me)
{
	me->numerator = me->decimal = 0;
}

Range WikiIterator_nextRange(WikiIterator *me)
{
	size_t start = me->decimal;

	me->decimal += me->decimal_step;
	me->numerator += me->numerator_step;

	if (me->numerator >= me->denominator)
	{
		me->numerator -= me->denominator;
		me->decimal++;
	}

	return new_range(start, me->decimal);
}

size_t WikiIterator_finished(WikiIterator *me)
{
	return (me->decimal >= me->size);
}

size_t WikiIterator_nextLevel(WikiIterator *me)
{
	me->decimal_step += me->decimal_step;
	me->numerator_step += me->numerator_step;

	if (me->numerator_step >= me->denominator)
	{
		me->numerator_step -= me->denominator;
		me->decimal_step++;
	}

	return (me->decimal_step < me->size);
}

size_t WikiIterator_length(WikiIterator *me)
{
	return me->decimal_step;
}

WikiIterator WikiIterator_new(size_t size2, size_t min_level)
{
	WikiIterator me;

	me.size = size2;
	me.power_of_two = FloorPowerOfTwo(me.size);
	me.denominator = me.power_of_two/min_level;
	me.numerator_step = me.size % me.denominator;
	me.decimal_step = me.size/me.denominator;

	WikiIterator_begin(&me);

	return me;
}

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│                █████┐    ██████┐ ██████┐████████┐  │//
//│               ██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘  │//
//│               └█████┌┘   ██████┌┘  ██│     ██│     │//
//│               ██┌──██┐   ██┌──██┐  ██│     ██│     │//
//│               └█████┌┘   ██████┌┘██████┐   ██│     │//
//│                └────┘    └─────┘ └─────┘   └─┘     │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR char
#define FUNC(NAME) NAME##8
#define STRUCT(NAME) struct NAME##8

#include "octosort.c"

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│           ▄██┐   █████┐    ██████┐ ██████┐████████┐│//
//│          ████│  ██┌───┘    ██┌──██┐└─██┌─┘└──██┌──┘│//
//│          └─██│  ██████┐    ██████┌┘  ██│     ██│   │//
//│            ██│  ██┌──██┐   ██┌──██┐  ██│     ██│   │//
//│          ██████┐└█████┌┘   ██████┌┘██████┐   ██│   │//
//│          └─────┘ └────┘    └─────┘ └─────┘   └─┘   │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR short
#define FUNC(NAME) NAME##16
#define STRUCT(NAME) struct NAME##16

#include "octosort.c"

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │       ██████┐ ██████┐    ██████┐ ██████┐████████┐ │//
// │       └────██┐└────██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │        █████┌┘ █████┌┘   ██████┌┘  ██│     ██│    │//
// │        └───██┐██┌───┘    ██┌──██┐  ██│     ██│    │//
// │       ██████┌┘███████┐   ██████┌┘██████┐   ██│    │//
// │       └─────┘ └──────┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR int
#define FUNC(NAME) NAME##32
#define STRUCT(NAME) struct NAME##32

#include "octosort.c"

//////////////////////////////////////////////////////////
// ┌───────────────────────────────────────────────────┐//
// │        █████┐ ██┐  ██┐   ██████┐ ██████┐████████┐ │//
// │       ██┌───┘ ██│  ██│   ██┌──██┐└─██┌─┘└──██┌──┘ │//
// │       ██████┐ ███████│   ██████┌┘  ██│     ██│    │//
// │       ██┌──██┐└────██│   ██┌──██┐  ██│     ██│    │//
// │       └█████┌┘     ██│   ██████┌┘██████┐   ██│    │//
// │        └────┘      └─┘   └─────┘ └─────┘   └─┘    │//
// └───────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR long long
#define FUNC(NAME) NAME##64
#define STRUCT(NAME) struct NAME##64

#include "octosort.c"

//////////////////////////////////////////////////////////
//┌────────────────────────────────────────────────────┐//
//│  ▄██┐  ██████┐  █████┐    ██████┐ ██████┐████████┐ │//
//│ ████│  └────██┐██┌──██┐   ██┌──██┐└─██┌─┘└──██┌──┘ │//
//│ └─██│   █████┌┘└█████┌┘   ██████┌┘  ██│     ██│    │//
//│   ██│  ██┌───┘ ██┌──██┐   ██┌──██┐  ██│     ██│    │//
//│ ██████┐███████┐└█████┌┘   ██████┌┘██████┐   ██│    │//
//│ └─────┘└──────┘ └────┘    └─────┘ └─────┘   └─┘    │//
//└────────────────────────────────────────────────────┘//
//////////////////////////////////////////////////////////

#undef VAR
#undef FUNC
#undef STRUCT

#define VAR long double
#define FUNC(NAME) NAME##128
#define STRUCT(NAME) struct NAME##128

#include "octosort.c"


////////////////////////////////////////////////////////////////////////////////
//┌──────────────────────────────────────────────────────────────────────────┐//
//│    ██████┐  ██████┐████████┐ ██████┐ ███████┐ ██████┐ ██████┐ ████████┐  │//
//│   ██┌───██┐██┌────┘└──██┌──┘██┌───██┐██┌────┘██┌───██┐██┌──██┐└──██┌──┘  │//
//│   ██│   ██│██│        ██│   ██│   ██│███████┐██│   ██│██████┌┘   ██│     │//
//│   ██│   ██│██│        ██│   ██│   ██│└────██│██│   ██│██┌──██┐   ██│     │//
//│   └██████┌┘└██████┐   ██│   └██████┌┘███████│└██████┌┘██│  ██│   ██│     │//
//│    └─────┘  └─────┘   └─┘    └─────┘ └──────┘ └─────┘ └─┘  └─┘   └─┘     │//
//└──────────────────────────────────────────────────────────────────────────┘//
////////////////////////////////////////////////////////////////////////////////

void octosort(void *array, size_t nmemb, size_t size, CMPFUNC *cmp)
{
	if (nmemb < 2)
	{
		return;
	}

	switch (size)
	{
		case sizeof(char):
			return octosort8(array, nmemb, NULL, 0, cmp);

		case sizeof(short):
			return octosort16(array, nmemb, NULL, 0, cmp);

		case sizeof(int):
			return octosort32(array, nmemb, NULL, 0, cmp);

		case sizeof(long long):
			return octosort64(array, nmemb, NULL, 0, cmp);

		case sizeof(long double):
			return octosort128(array, nmemb, NULL, 0, cmp);

		default:
			return assert(size == sizeof(char) || size == sizeof(short) || size == sizeof(int) || size == sizeof(long long) || size == sizeof(long double));
	}
}

#undef DYNAMIC_CACHE
#undef PULL
#undef SWAP

#undef VAR
#undef FUNC
#undef STRUCT

#endif
