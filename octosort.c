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

// binary insertion sort for up to 8 elements

void FUNC(octo_tail_insert)(VAR *array, VAR *pta, CMPFUNC *cmp)
{
	VAR *pte, key;

	pte = pta--;

	if (cmp(pta, pte) > 0)
	{
		key = *pte;

		if (cmp(pta - 3, &key) > 0)
		{
			*pte-- = *pta--; *pte-- = *pta--; *pte-- = *pta--; *pte-- = *pta--;
		}
		
		if (pta >= array + 1 && cmp(pta - 1, &key) > 0)
		{
			*pte-- = *pta--; *pte-- = *pta--;
		}

		if (pta >= array && cmp(pta, &key) > 0)
		{
			*pte-- = *pta;
		}
		*pte = key;
	}
}

// sort arrays of length 4 to 8 with reverse order run detection

VAR *FUNC(octo_swap)(VAR array[], VAR *ptz, size_t start, size_t nmemb, CMPFUNC *cmp)
{
	VAR *pta, swap;
	size_t i;

	pta = array + start;

	if (cmp(&pta[0], &pta[1]) > 0)
	{
		if (cmp(&pta[2], &pta[3]) > 0)
		{
			if (cmp(&pta[1], &pta[2]) > 0)
			{
				goto Swapper;
			}
			swap = pta[2]; pta[2] = pta[3]; pta[3] = swap;
		}
		swap = pta[0]; pta[0] = pta[1]; pta[1] = swap;
	}
	else if (cmp(&pta[2], &pta[3]) > 0)
	{
		swap = pta[2]; pta[2] = pta[3]; pta[3] = swap;
	}

	if (cmp(&pta[1], &pta[2]) > 0)
	{
		if (cmp(&pta[0], &pta[2]) <= 0)
		{
			if (cmp(&pta[1], &pta[3]) <= 0)
			{
				swap = pta[1]; pta[1] = pta[2]; pta[2] = swap;
			}
			else
			{
				swap = pta[1]; pta[1] = pta[2]; pta[2] = pta[3]; pta[3] = swap;
			}
		}
		else if (cmp(&pta[0], &pta[3]) > 0)
		{
			swap = pta[1]; pta[1] = pta[3]; pta[3] = swap;
			swap = pta[0]; pta[0] = pta[2]; pta[2] = swap;
		}
		else if (cmp(&pta[1], &pta[3]) <= 0)
		{
			swap = pta[1]; pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = swap;
		}
		else
		{
			swap = pta[1]; pta[1] = pta[0]; pta[0] = pta[2]; pta[2] = pta[3]; pta[3] = swap;
		}
	}

	for (i = 4 ; i < nmemb ; i++)
	{
		FUNC(octo_tail_insert)(pta, &pta[i], cmp);
	}

	if (ptz)
	{
		do
		{
			swap = *ptz;
			*ptz++ = *--pta;
			*pta = swap;
		}
		while (ptz < pta);
	}
	return NULL;

	Swapper:

	if (ptz == NULL || cmp(&pta[-1], &pta[0]) > 0)
	{
		switch (nmemb)
		{
			case 8:
				if (cmp(&pta[6], &pta[7]) <= 0)
				{
					break;
				}
			case 7:
				if (cmp(&pta[5], &pta[6]) <= 0)
				{
					break;
				}
			case 6:
				if (cmp(&pta[4], &pta[5]) <= 0)
				{
					break;
				}
			case 5:
				if (cmp(&pta[3], &pta[4]) <= 0)
				{
					break;
				}
			case 4:
				return ptz ? ptz : pta;
		}
	}

	swap = pta[0]; pta[0] = pta[3]; pta[3] = swap;
	swap = pta[1]; pta[1] = pta[2]; pta[2] = swap;

	for (i = 4 ; i < nmemb ; i++)
	{
		FUNC(octo_tail_insert)(pta, &pta[i], cmp);
	}

	if (ptz)
	{
		do
		{
			swap = *ptz;
			*ptz++ = *--pta;
			*pta = swap;
		}
		while (ptz < pta);
	}
	return NULL;
}

// find the index of the first value within the range that is equal to array[index]

size_t FUNC(monobound_binary_first)(const VAR array[], const VAR value, const Range range, CMPFUNC *cmp)
{
	size_t top, mid, end = range.end;

	if (range.start >= end)
	{
		return range.start;
	}

	top = end - range.start;

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(&value, &array[end - mid]) <= 0)
		{
			end -= mid;
		}
		top -= mid;
	}

	if (cmp(&value, &array[end-1]) <= 0)
	{
		return --end;
	}
	return end;
}

// find the index of the last value within the range that is equal to array[index], plus 1

size_t FUNC(monobound_binary_last)(const VAR array[], const VAR value, const Range range, CMPFUNC *cmp)
{
	size_t top, mid, start = range.start;

	if (start >= range.end)
	{
		return start;
	}

	top = range.end - start;

	while (top > 1)
	{
		mid = top / 2;

		if (cmp(&array[start + mid], &value) <= 0)
		{
			start += mid;
		}
		top -= mid;
	}

	if (cmp(&array[start], &value) <= 0)
	{
		return ++start;
	}
	return start;
}

// combine a linear search with a binary search to reduce the number of comparisons in situations
// where have some idea as to how many unique values there are and where the next value might be

size_t FUNC(FindFirstForward)(const VAR array[], const VAR value, const Range range, CMPFUNC *cmp, const size_t unique)
{
	size_t skip, index;

	skip = Max(range_length(range) / unique, 1);

	for (index = range.start + skip ; cmp(&value, &array[index - 1]) > 0 ; index += skip)
	{
		if (index >= range.end - skip)
		{
			return FUNC(monobound_binary_first)(array, value, new_range(index, range.end), cmp);
		}
	}
	return FUNC(monobound_binary_first)(array, value, new_range(index - skip, index), cmp);
}

size_t FUNC(FindLastForward)(const VAR array[], const VAR value, const Range range, CMPFUNC *cmp, const size_t unique)
{
	size_t skip, index;

	if (range_length(range) == 0)
		return range.start;

	skip = Max(range_length(range)/unique, 1);

	for (index = range.start + skip; cmp(&array[index - 1], &value) <= 0 ; index += skip)
	{
		if (index >= range.end - skip)
		{
			return FUNC(monobound_binary_last)(array, value, new_range(index, range.end), cmp);
		}
	}
	return FUNC(monobound_binary_last)(array, value, new_range(index - skip, index), cmp);
}

size_t FUNC(FindFirstBackward)(const VAR array[], const VAR value, const Range range, CMPFUNC *cmp, const size_t unique)
{
	size_t skip, index;

	if (range_length(range) == 0)
		return range.start;

	skip = Max(range_length(range)/unique, 1);

	for (index = range.end - skip; index > range.start && cmp(&value, &array[index - 1]) <= 0 ; index -= skip)
	{
		if (index < range.start + skip)
		{
			return FUNC(monobound_binary_first)(array, value, new_range(range.start, index), cmp);
		}
	}
	return FUNC(monobound_binary_first)(array, value, new_range(index, index + skip), cmp);
}

size_t FUNC(FindLastBackward)(const VAR array[], const VAR value, const Range range, CMPFUNC *cmp, const size_t unique)
{
	size_t skip, index;

	if (range_length(range) == 0)
		return range.start;

	skip = Max(range_length(range)/unique, 1);

	for (index = range.end - skip; index > range.start && cmp(&array[index - 1], &value) > 0 ; index -= skip)
	{
		if (index < range.start + skip)
		{
			return FUNC(monobound_binary_last)(array, value, new_range(range.start, index), cmp);
		}
	}
	return FUNC(monobound_binary_last)(array, value, new_range(index, index + skip), cmp);
}

// monobound binary insertion sort

void FUNC(monobound_sort)(VAR array[], const Range range, CMPFUNC *cmp)
{
	VAR *start, *pta, *end, key;
	size_t i, mid, top, nmemb;

	start = array + range.start;
	nmemb = range.end - range.start;

	for (i = 1 ; i < nmemb ; i++)
	{
		pta = end = start + i;

		if (cmp(--pta, end) <= 0)
		{
			continue;
		}
		top = i;

		while (top > 1)
		{
			mid = top / 2;

			if (cmp(pta - mid, end) > 0)
			{
				pta -= mid;
			}
			top -= mid;
		}

		key = *end;

		memmove(pta + 1, pta, (end - pta) * sizeof(VAR));

		*pta = key;
	}
}

// swap a series of values in the array

void FUNC(forward_block_swap)(VAR array[], const size_t start1, const size_t start2, size_t block_size)
{
	VAR *pta, *ptb, swap;

	pta = array + start1;
	ptb = array + start2;

	while (block_size--)
	{
		swap = *pta; *pta++ = *ptb; *ptb++ = swap;
	}
}

void FUNC(backward_block_swap)(VAR array[], const size_t start1, const size_t start2, size_t block_size)
{
	VAR *pta, *ptb, swap;

	pta = array + start1 + block_size;
	ptb = array + start2 + block_size;

	while (block_size--)
	{
		swap = *--pta; *pta = *--ptb; *ptb = swap;
	}
}

// rotate the values in an array ([0 1 2 3] becomes [1 2 3 0] if we rotate by 1)
// this assumes that 0 <= amount <= range.length()

void FUNC(Rotate)(VAR array[], const size_t amount, const Range range)
{
	size_t start = range.start;
	size_t left  = amount;
	size_t right = range.end - range.start - amount;
	size_t min   = left <= right ? left : right;

	// Gries-Mills rotation

	while (min > 1)
	{
		if (left <= right)
		{
			do
			{
				FUNC(forward_block_swap)(array, start, start + left, left);

				start += left;
				right -= left;
			}
			while (left <= right);

			min = right;
		}
		else
		{
			do
			{
				FUNC(backward_block_swap)(array, start + left - right, start + left, right);

				left -= right;
			}
			while (right <= left);

			min = left;
		}
	}

	if (min)
	{
		if (left <= right)
		{
			VAR swap = array[start];
			memmove(&array[start], &array[start + 1], (right) * sizeof(VAR));
			array[start + right] = swap;
		}
		else
		{
			VAR swap = array[start + left];
			memmove(&array[start + 1], &array[start], (left) * sizeof(VAR));
			array[start] = swap;
		}
	}
}

// merge two ranges from one array into another array

void FUNC(forward_merge_into)(VAR *dest, VAR *from, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *l, *r, *m, *e; // left, right, middle, end

	l = from;
	r = from + block;
	m = r;
	e = l + nmemb;

	while (1)
	{
		if (cmp(l, r) <= 0)
		{
			*dest++ = *l++;

			if (l == m)
			{
				do *dest++ = *r++; while (r < e);

				return;
			}
		}
		else
		{
			*dest++ = *r++;

			if (r == e)
			{
				do *dest++ = *l++; while (l < m);

				return;
			}
		}
	}
}

void FUNC(external_backward_merge)(VAR *array, VAR *swap, size_t nmemb, size_t block, CMPFUNC *cmp)
{
	VAR *r, *m, *e, *s; // right, middle, end, swap

	m = array + block;
	e = array + nmemb - 1;
	r = m--;

	if (cmp(m, r) <= 0)
	{
		return;
	}

	while (cmp(m, e) <= 0)
	{
		e--;
	}

	s = swap;

	do *s++ = *r++; while (r <= e);

	s--;

	*e-- = *m--;

	if (cmp(array, swap) <= 0)
	{
		while (1)
		{
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;
			}
			else
			{
				*e-- = *s--;

				if (s < swap)
				{
					return;
				}
			}
		}
	}
	else
	{
		while (1)
		{
			if (cmp(m, s) > 0)
			{
				*e-- = *m--;

				if (m < array)
				{
					do *e-- = *s--; while (s >= swap);

					return;
				}
			}
			else
			{
				*e-- = *s--;
			}
		}
	}
}

// merge operation using an external buffer

void FUNC(MergeExternal)(VAR array[], const Range A, const Range B, CMPFUNC *cmp, VAR *cache)
{
	VAR *A_index = &cache[0];
	VAR *B_index = &array[B.start];
	VAR *insert_index = &array[A.start];
	VAR *A_last = &cache[range_length(A)];
	VAR *B_last = &array[B.end];

	if (range_length(B) > 0 && range_length(A) > 0)
	{
		while (1)
		{
			if (cmp(A_index, B_index) <= 0)
			{
				*insert_index++ = *A_index++;

				if (A_index == A_last)
					break;
			}
			else
			{
				*insert_index++ = *B_index++;

				if (B_index == B_last)
					break;
			}
		}
	}
	// copy the remainder of A into the final array

	memcpy(insert_index, A_index, (A_last - A_index) * sizeof(VAR));
}

// merge operation using an internal buffer

// whenever we find a value to add to the final array, swap it with the value that's
// already in that spot when this algorithm is finished, the 'I' range will contain
// its original contents, but in a different order

void FUNC(MergeInternal)(VAR array[], const Range A, const Range B, CMPFUNC *cmp, const Range I)
{
	VAR swap, *pta, *ptb, *pti;
	size_t a = 0, b = 0, i = 0;
	size_t length_A = range_length(A);
	size_t length_B = range_length(B);

	if (length_A > 0 && length_B > 0)
	{
		pta = array + A.start;
		ptb = array + B.start;
		pti = array + I.start;

		while (1)
		{
			if (cmp(&pti[a], &ptb[b]) <= 0)
			{
				swap = pta[i]; pta[i++] = pti[a]; pti[a] = swap;

				if (++a >= length_A)
					break;
			}
			else
			{
				swap = pta[i]; pta[i++] = ptb[b]; ptb[b] = swap;

				if (++b >= length_B)
					break;
			}
		}
	}
	FUNC(backward_block_swap)(array, I.start + a, A.start + i, length_A - a);
}

// merge operation without a buffer

// this just repeatedly binary searches into B and rotates A into position.
// the paper suggests using the 'rotation-based Hwang and Lin algorithm' here,
// but I decided to stick with this because it had better situational performance

// (Hwang and Lin is designed for merging subarrays of very different sizes,
// but WikiSort almost always uses subarrays that are roughly the same size)

// normally this is incredibly suboptimal, but this function is only called
// when none of the A or B blocks in any subarray contained 2√A unique values,
// which places a hard limit on the number of times this will ACTUALLY need
// to binary search and rotate.

// according to my analysis the worst case is √A rotations performed on √A items
// once the constant factors are removed, which ends up being O(n)

// again, this is NOT a general-purpose solution – it only works well in this case!
// kind of like how the O(n^2) insertion sort is used in some places

void FUNC(MergeInPlace)(VAR array[], Range A, Range B, CMPFUNC *cmp, VAR *cache, const size_t cache_size)
{
	if (range_length(A) == 0 || range_length(B) == 0)
	{
		return;
	}

	while (1)
	{
		// find the first place in B where the first item in A needs to be inserted
		size_t mid = FUNC(monobound_binary_first)(array, array[A.start], B, cmp);

		// rotate A into place
		size_t amount = mid - A.end;

		FUNC(Rotate)(array, range_length(A), new_range(A.start, mid));

		if (B.end == mid)
		{
			break;
		}

		// calculate the new A and B ranges

		B.start = mid;
		A = new_range(A.start + amount, B.start);
		A.start = FUNC(monobound_binary_last)(array, array[A.start], A, cmp);

		if (range_length(A) == 0)
		{
			break;
		}
	}
}

// bottom-up merge sort combined with an in-place merge algorithm for O(1) memory use

void FUNC(octosort)(VAR array[], size_t size, VAR *external_cache, size_t cache_size, CMPFUNC *cmp)
{
	VAR swap, stack_cache[512], *cache = external_cache;

	#if DYNAMIC_CACHE
		// turns into a full-throttle merge sort since everything fits into the cache

		if (cache == NULL)
		{
			cache_size = 1 + size / 2;

			cache = (VAR *) malloc(cache_size * sizeof(VAR));

			if (cache == NULL)
			{
				external_cache = cache = stack_cache;

				cache_size = 512;
			}
		}
	#else
		// since the cache size is fixed, it's still O(1) memory
		// the minimum stack size is typically 8192 KB, so 512 elements should fit comfortably
		// removing the cache entirely gives 60% of the performance of qsort()

		if (cache == NULL)
		{
			cache = stack_cache;

			cache_size = 512;
		}
	#endif

	// if the array is of size 1, 2, 3 .. 8 sort them like so:

	if (size <= 8)
	{
		FUNC(monobound_sort)(array, new_range(0, size), cmp);

		goto End;
	}

	WikiIterator iterator = WikiIterator_new(size, 4);

	VAR *pto = NULL;

	// sort groups of 4-8 items at a time

	while (!WikiIterator_finished(&iterator))
	{
		Range range = WikiIterator_nextRange(&iterator);

		pto = FUNC(octo_swap)(array, pto, range.start, range.end - range.start, cmp);
	}

	if (pto)
	{
		VAR *pta = array + size - 1;
		VAR *ptz = pto;

		do
		{
			swap = *ptz;
			*ptz = *pta;
			*pta = swap;
		}
		while (++ptz < --pta);

		if (pto == array)
		{
			goto End;
		}
	}

	// then merge sort the higher levels, which can be 8-15, 16-31, 32-63, 64-127, etc.

	while (1)
	{
		// if every A and B block will fit into the cache, use a special branch specifically for merging with the cache
		// (we use < rather than <= since the block size might be one more than iterator.length())
		if (WikiIterator_length(&iterator) < cache_size)
		{
			// perform a quad merge if the four subarrays fit into the cache

			// array: [A][B][C][D]
			// cache: [A  B]       Step 1
			// cache:       [C  D] Step 2
			// array: [A  B  C  D] Step 3

			if ((WikiIterator_length(&iterator) + 1) * 4 <= cache_size && (WikiIterator_length(&iterator) + 1) * 4 <= size)
			{
				WikiIterator_begin(&iterator);

				while (!WikiIterator_finished(&iterator))
				{
					Range A = WikiIterator_nextRange(&iterator);
					Range B = WikiIterator_nextRange(&iterator);
					Range C = WikiIterator_nextRange(&iterator);
					Range D = WikiIterator_nextRange(&iterator);

					if (cmp(&array[A.end - 1], &array[B.start]) <= 0)
					{
						if (cmp(&array[C.end - 1], &array[D.start]) <= 0)
						{
							if (cmp(&array[B.end - 1], &array[C.start]) <= 0)
							{
								continue; // A through D are in order, skip doing anything else
							}
							// A and B are in order and C and D are in order, copy to cache
							memcpy(&cache[0],                 &array[A.start], range_length(A) * sizeof(VAR));
							memcpy(&cache[A.end - A.start], &array[B.start], range_length(B) * sizeof(VAR));
							memcpy(&cache[B.end - A.start], &array[C.start], range_length(C) * sizeof(VAR));
							memcpy(&cache[C.end - A.start], &array[D.start], range_length(D) * sizeof(VAR));

							goto Step3;
						}
						// A and B are in order, copy to cache
						memcpy(&cache[0],                 &array[A.start], range_length(A) * sizeof(VAR));
						memcpy(&cache[A.end - A.start], &array[B.start], range_length(B) * sizeof(VAR));

						goto Step2;
					}
					// Step1:

					// A and B are not in order, merge to cache
					FUNC(forward_merge_into)(cache, array + A.start, B.end - A.start, A.end - A.start, cmp);

					if (cmp(&array[C.end - 1], &array[D.start]) <= 0) // C and D are in order, copy to cache
					{
						memcpy(&cache[B.end - A.start], &array[C.start], range_length(C) * sizeof(VAR));
						memcpy(&cache[C.end - A.start], &array[D.start], range_length(D) * sizeof(VAR));
					}
					else
					{
						Step2:

						// C and D are not in order, merge to cache
						FUNC(forward_merge_into)(&cache[B.end - A.start], &array[C.start], D.end - C.start, C.end - C.start, cmp);
					}
					Step3:

					// merge A through D from the cache back into the array
					FUNC(forward_merge_into)(&array[A.start], &cache[0], D.end - A.start, B.end - A.start, cmp);
				}

				// we merged two levels at the same time, so we're done with this level already
				// iterator.nextLevel() is called again at the bottom of this outer merge loop

				WikiIterator_nextLevel(&iterator);
			}
			else
			{
				WikiIterator_begin(&iterator);

				while (!WikiIterator_finished(&iterator))
				{
					Range A = WikiIterator_nextRange(&iterator);
					Range B = WikiIterator_nextRange(&iterator);

					if (cmp(&array[A.end - 1], &array[B.start]) <= 0)
					{
						continue; // A and B are in order, skip doing anything else
					}
					// A and B are not in order, merge through the cache
					FUNC(external_backward_merge)(array + A.start, cache, B.end - A.start, range_length(A), cmp);
				}
			}
		}
		else
		{
			// this is where the in-place merge logic starts!
			// 1. pull out two internal buffers each containing √A unique values
			//	1a. adjust block_size and buffer_size if we couldn't find enough unique values
			// 2. loop over the A and B subarrays within this level of the merge sort
			// 3. break A and B into blocks of size 'block_size'
			// 4. "tag" each of the A blocks with values from the first internal buffer
			// 5. roll the A blocks through the B blocks and drop/rotate them where they belong
			// 6. merge each A block with any B values that follow, using the cache or the second internal buffer
			// 7. sort the second internal buffer if it exists
			// 8. redistribute the two internal buffers back into the array

			size_t block_size = monobound_sqrt(WikiIterator_length(&iterator));

			size_t buffer_size = WikiIterator_length(&iterator) / block_size + 1;

			// as an optimization, we really only need to pull out the internal buffers once for each level of merges
			// after that we can reuse the same buffers over and over, then redistribute it when we're finished with this level

			Range buffer1, buffer2, A, B;
			size_t find_separately = 0;
			size_t index, last, count, find, start, pull_index = 0;

			struct
			{
				size_t from;
				size_t to;
				size_t count;
				Range range;
			}
			pull[2];

			pull[0].from = pull[0].to = pull[0].count = 0; pull[0].range = new_range(0, 0);
			pull[1].from = pull[1].to = pull[1].count = 0; pull[1].range = new_range(0, 0);

			buffer1 = new_range(0, 0);
			buffer2 = new_range(0, 0);

			find = buffer_size * 2; // find two internal buffers of size 'buffer_size' each

			if (block_size <= cache_size)
			{
				find = buffer_size; // if every A block fits into the cache then we won't need the second internal buffer
			}
			else if (find > WikiIterator_length(&iterator))
			{
				find_separately = 1; // we can't fit both buffers into the same A or B subarray, so find two buffers separately
				find = buffer_size;
			}

			// we need to find either a single contiguous space containing 2√A unique values, which will be split up into two buffers of size √A each,
			// or we need to find one buffer of < 2√A unique values, and a second buffer of √A unique values,
			// OR if we couldn't find that many unique values, we need the largest possible buffer we can get

			// in the case where it couldn't find a single buffer of at least √A unique values,
			// all of the Merge steps must be replaced by a different merge algorithm (MergeInPlace)

			WikiIterator_begin(&iterator);

			while (!WikiIterator_finished(&iterator))
			{
				A = WikiIterator_nextRange(&iterator);
				B = WikiIterator_nextRange(&iterator);

				// just store information about where the values will be pulled from and to,
				// as well as how many values there are, to create the two internal buffers

				// check A for the number of unique values we need to fill an internal buffer
				// these values will be pulled out to the start of A
				for (last = A.start, count = 1; count < find; last = index, count++)
				{
					index = FUNC(FindLastForward)(array, array[last], new_range(last + 1, A.end), cmp, find - count);

					if (index == A.end)
						break;
				}
				index = last;

				if (count >= buffer_size)
				{
					// keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer
					PULL(A.start);
					pull_index = 1;

					if (count == buffer_size + buffer_size)
					{
						// we were able to find a single contiguous section containing 2√A unique values,
						// so this section can be used to contain both of the internal buffers we'll need
						buffer1 = new_range(A.start, A.start + buffer_size);
						buffer2 = new_range(A.start + buffer_size, A.start + count);
						break;
					}
					else if (find == buffer_size + buffer_size)
					{
						// we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values,
						// so we still need to find a second separate buffer of at least √A unique values
						buffer1 = new_range(A.start, A.start + count);
						find = buffer_size;
					}
					else if (block_size <= cache_size)
					{
						// we found the first and only internal buffer that we need, so we're done!
						buffer1 = new_range(A.start, A.start + count);
						break;
					}
					else if (find_separately)
					{
						// found one buffer, but now find the other one
						buffer1 = new_range(A.start, A.start + count);
						find_separately = 0;
					}
					else
					{
						// we found a second buffer in an 'A' subarray containing √A unique values, so we're done!
						buffer2 = new_range(A.start, A.start + count);
						break;
					}
				}
				else if (pull_index == 0 && count > range_length(buffer1))
				{
					// keep track of the largest buffer we were able to find
					buffer1 = new_range(A.start, A.start + count);
					PULL(A.start);
				}

				// check B for the number of unique values we need to fill an internal buffer
				// these values will be pulled out to the end of B
				for (last = B.end - 1, count = 1; count < find; last = index - 1, count++)
				{
					index = FUNC(FindFirstBackward)(array, array[last], new_range(B.start, last), cmp, find - count);
					if (index == B.start)
						break;
				}
				index = last;

				if (count >= buffer_size)
				{
					// keep track of the range within the array where we'll need to "pull out" these values to create the internal buffer
					PULL(B.end);
					pull_index = 1;

					if (count == buffer_size + buffer_size)
					{
						// we were able to find a single contiguous section containing 2√A unique values,
						// so this section can be used to contain both of the internal buffers we'll need
						buffer1 = new_range(B.end - count, B.end - buffer_size);
						buffer2 = new_range(B.end - buffer_size, B.end);
						break;
					}
					else if (find == buffer_size + buffer_size)
					{
						// we found a buffer that contains at least √A unique values, but did not contain the full 2√A unique values,
						// so we still need to find a second separate buffer of at least √A unique values
						buffer1 = new_range(B.end - count, B.end);
						find = buffer_size;
					}
					else if (block_size <= cache_size)
					{
						// we found the first and only internal buffer that we need, so we're done!
						buffer1 = new_range(B.end - count, B.end);
						break;
					}
					else if (find_separately)
					{
						// found one buffer, but now find the other one
						buffer1 = new_range(B.end - count, B.end);
						find_separately = 0;
					}
					else
					{
						// buffer2 will be pulled out from a 'B' subarray, so if the first buffer was pulled out from the corresponding 'A' subarray,
						// we need to adjust the end point for that A subarray so it knows to stop redistributing its values before reaching buffer2
						if (pull[0].range.start == A.start) pull[0].range.end -= pull[1].count;

						// we found a second buffer in an 'B' subarray containing √A unique values, so we're done!
						buffer2 = new_range(B.end - count, B.end);
						break;
					}
				}
				else if (pull_index == 0 && count > range_length(buffer1))
				{
					// keep track of the largest buffer we were able to find
					buffer1 = new_range(B.end - count, B.end);
					PULL(B.end);
				}
			}

			// pull out the two ranges so we can use them as internal buffers
			for (pull_index = 0; pull_index < 2; pull_index++)
			{
				Range range;
				size_t length = pull[pull_index].count;

				if (pull[pull_index].to < pull[pull_index].from)
				{
					// we're pulling the values out to the left, which means the start of an A subarray
					index = pull[pull_index].from;
					for (count = 1; count < length; count++)
					{
						index = FUNC(FindFirstBackward)(array, array[index - 1], new_range(pull[pull_index].to, pull[pull_index].from - (count - 1)), cmp, length - count);
						range = new_range(index + 1, pull[pull_index].from + 1);
						FUNC(Rotate)(array, range_length(range) - count, range);
						pull[pull_index].from = index + count;
					}
				}
				else if (pull[pull_index].to > pull[pull_index].from)
				{
					// we're pulling values out to the right, which means the end of a B subarray
					index = pull[pull_index].from + 1;
					for (count = 1; count < length; count++)
					{
						index = FUNC(FindLastForward)(array, array[index], new_range(index, pull[pull_index].to), cmp, length - count);
						range = new_range(pull[pull_index].from, index - 1);
						FUNC(Rotate)(array, count, range);
						pull[pull_index].from = index - 1 - count;
					}
				}
			}

			// adjust block_size and buffer_size based on the values we were able to pull out
			buffer_size = range_length(buffer1);
			block_size = WikiIterator_length(&iterator)/buffer_size + 1;

			// the first buffer NEEDS to be large enough to tag each of the evenly sized A blocks,
			// so this was originally here to test the math for adjusting block_size above
			// assert((WikiIterator_length(&iterator) + 1)/block_size <= buffer_size);

			// now that the two internal buffers have been created, it's time to merge each A+B combination at this level of the merge sort!
			WikiIterator_begin(&iterator);

			while (!WikiIterator_finished(&iterator))
			{
				A = WikiIterator_nextRange(&iterator);
				B = WikiIterator_nextRange(&iterator);

				// remove any parts of A or B that are being used by the internal buffers
				start = A.start;

				if (start == pull[0].range.start)
				{
					if (pull[0].from > pull[0].to)
					{
						A.start += pull[0].count;

						// if the internal buffer takes up the entire A or B subarray, then there's nothing to merge
						// this only happens for very small subarrays, like √4 = 2, 2 * (2 internal buffers) = 4,
						// which also only happens when cache_size is small or 1 since it'd otherwise use MergeExternal
						if (range_length(A) == 0)
							continue;
					}
					else if (pull[0].from < pull[0].to)
					{
						B.end -= pull[0].count;
						if (range_length(B) == 0)
							continue;
					}
				}

				if (start == pull[1].range.start)
				{
					if (pull[1].from > pull[1].to)
					{
						A.start += pull[1].count;
						if (range_length(A) == 0)
							continue;
					}
					else if (pull[1].from < pull[1].to)
					{
						B.end -= pull[1].count;
						if (range_length(B) == 0)
							continue;
					}
				}

				if (cmp(&array[A.end - 1], &array[B.start]) > 0) // A and B are not in order, so merge them
				{
					Range blockA, firstA, lastA, lastB, blockB;
					size_t indexA, findA;

					// break the remainder of A into blocks. firstA is the uneven-sized first A block
					blockA = new_range(A.start, A.end);
					firstA = new_range(A.start, A.start + range_length(blockA) % block_size);

					// swap the first value of each A block with the value in buffer1
					for (indexA = buffer1.start, index = firstA.end; index < blockA.end; indexA++, index += block_size) 
					{
						SWAP(array[indexA], array[index]);
					}

					// start rolling the A blocks through the B blocks!
					// whenever we leave an A block behind, we'll need to merge the previous A block with any B blocks that follow it, so track that information as well
					lastA = firstA;
					lastB = new_range(0, 0);
					blockB = new_range(B.start, B.start + Min(block_size, range_length(B)));
					blockA.start += range_length(firstA);
					indexA = buffer1.start;

					// if the first unevenly sized A block fits into the cache, copy it there for when we go to Merge it
					// otherwise, if the second buffer is available, block swap the contents into that
					if (range_length(lastA) <= cache_size)
					{
						memcpy(&cache[0], &array[lastA.start], range_length(lastA) * sizeof(VAR));
					}
					else if (range_length(buffer2) > 0)
					{
						FUNC(forward_block_swap)(array, lastA.start, buffer2.start, range_length(lastA));
					}

					if (range_length(blockA) > 0)
					{
						while (1)
						{
							// if there's a previous B block and the first value of the minimum A block is <= the last value of the previous B block,
							// then drop that minimum A block behind. or if there are no B blocks left then keep dropping the remaining A blocks.

							if (range_length(blockB) == 0 || (range_length(lastB) > 0 && cmp(&array[indexA], &array[lastB.end - 1]) <= 0))
							{
								// figure out where to split the previous B block, and rotate it at the split
								size_t B_split = FUNC(monobound_binary_first)(array, array[indexA], lastB, cmp);
								size_t B_remaining = lastB.end - B_split;

								// swap the minimum A block to the beginning of the rolling A blocks
								size_t minA = blockA.start;
								for (findA = minA + block_size; findA < blockA.end; findA += block_size)
								{
									if (cmp(&array[minA], &array[findA]) > 0)
									{
										minA = findA;
									}
								}
								FUNC(forward_block_swap)(array, blockA.start, minA, block_size);

								// swap the first item of the previous A block back with its original value, which is stored in buffer1
								SWAP(array[blockA.start], array[indexA]);
								indexA++;

								 // locally merge the previous A block with the B values that follow it if lastA fits into the external cache
								 // we'll use that (with MergeExternal), or if the second internal buffer exists we'll use that (with MergeInternal),
								 // or failing that we'll use a strictly in-place merge algorithm (MergeInPlace)

								if (range_length(lastA) <= cache_size)
								{
									FUNC(MergeExternal)(array, lastA, new_range(lastA.end, B_split), cmp, cache);
								}
								else if (range_length(buffer2) > 0)
								{
									FUNC(MergeInternal)(array, lastA, new_range(lastA.end, B_split), cmp, buffer2);
								}
								else
								{
									FUNC(MergeInPlace)(array, lastA, new_range(lastA.end, B_split), cmp, cache, cache_size);
								}

								if (range_length(buffer2) > 0 || block_size <= cache_size)
								{
									// copy the previous A block into the cache or buffer2, since that's where we need it to be when we go to merge it anyway

									if (block_size <= cache_size)
									{
										memcpy(&cache[0], &array[blockA.start], block_size * sizeof(VAR));
									}
									else
									{
										FUNC(forward_block_swap)(array, blockA.start, buffer2.start, block_size);
									}

									// this is equivalent to rotating, but faster
									// the area normally taken up by the A block is either the contents of buffer2, or data we don't need anymore since we memcopied it
									// either way, we don't need to retain the order of those items, so instead of rotating we can just block swap B to where it belongs
									FUNC(forward_block_swap)(array, B_split, blockA.start + block_size - B_remaining, B_remaining);
								}
								else
								{
									// we are unable to use the 'buffer2' trick to speed up the rotation operation since buffer2 doesn't exist, so perform a normal rotation
									FUNC(Rotate)(array, blockA.start - B_split, new_range(B_split, blockA.start + block_size));
								}

								// update the range for the remaining A blocks, and the range remaining from the B block after it was split
								lastA = new_range(blockA.start - B_remaining, blockA.start - B_remaining + block_size);
								lastB = new_range(lastA.end, lastA.end + B_remaining);

								// if there are no more A blocks remaining, this step is finished!
								blockA.start += block_size;
								if (range_length(blockA) == 0)
								{
									break;
								}

							}
							else if (range_length(blockB) < block_size)
							{
								// move the last B block, which is unevenly sized, to before the remaining A blocks, by using a rotation
								// the cache is disabled here since it might contain the contents of the previous A block
								FUNC(Rotate)(array, blockB.start - blockA.start, new_range(blockA.start, blockB.end));

								lastB = new_range(blockA.start, blockA.start + range_length(blockB));
								blockA.start += range_length(blockB);
								blockA.end += range_length(blockB);
								blockB.end = blockB.start;
							}
							else
							{
								// roll the leftmost A block to the end by swapping it with the next B block
								FUNC(forward_block_swap)(array, blockA.start, blockB.start, block_size);
								lastB = new_range(blockA.start, blockA.start + block_size);

								blockA.start += block_size;
								blockA.end += block_size;
								blockB.start += block_size;

								if (blockB.end > B.end - block_size)
								{
									blockB.end = B.end;
								}
								else
								{
									blockB.end += block_size;
								}
							}
						}
					}

					// merge the last A block with the remaining B values
					if (range_length(lastA) <= cache_size)
					{
						FUNC(MergeExternal)(array, lastA, new_range(lastA.end, B.end), cmp, cache);
					}
					else if (range_length(buffer2) > 0)
					{
						FUNC(MergeInternal)(array, lastA, new_range(lastA.end, B.end), cmp, buffer2);
					}
					else
					{
						FUNC(MergeInPlace)(array, lastA, new_range(lastA.end, B.end), cmp, cache, cache_size);
					}
				}
			}

			// when we're finished with this merge step we should have the one or two internal buffers left over, where the second buffer is all jumbled up
			// insertion sort the second buffer, then redistribute the buffers back into the array using the opposite process used for creating the buffer

			// While an unstable sort like quicksort could be applied here, in benchmarks it was consistently slightly slower than a simple insertion sort,
			// even for tens of millions of items. this may be because insertion sort is quite fast when the data is already somewhat sorted, like it is here

			FUNC(monobound_sort)(array, buffer2, cmp);

			for (pull_index = 0; pull_index < 2; pull_index++)
			{
				size_t amount, unique = pull[pull_index].count * 2;
				if (pull[pull_index].from > pull[pull_index].to)
				{
					// the values were pulled out to the left, so redistribute them back to the right
					Range buffer = new_range(pull[pull_index].range.start, pull[pull_index].range.start + pull[pull_index].count);
					while (range_length(buffer) > 0)
					{
						index = FUNC(FindFirstForward)(array, array[buffer.start], new_range(buffer.end, pull[pull_index].range.end), cmp, unique);
						amount = index - buffer.end;
						FUNC(Rotate)(array, range_length(buffer), new_range(buffer.start, index));
						buffer.start += (amount + 1);
						buffer.end += amount;
						unique -= 2;
					}
				}
				else if (pull[pull_index].from < pull[pull_index].to)
				{
					// the values were pulled out to the right, so redistribute them back to the left
					Range buffer = new_range(pull[pull_index].range.end - pull[pull_index].count, pull[pull_index].range.end);
					while (range_length(buffer) > 0)
					{
						index = FUNC(FindLastBackward)(array, array[buffer.end - 1], new_range(pull[pull_index].range.start, buffer.start), cmp, unique);
						amount = buffer.start - index;
						FUNC(Rotate)(array, amount, new_range(index, buffer.end));
						buffer.start -= amount;
						buffer.end -= (amount + 1);
						unique -= 2;
					}
				}
			}
		}

		// double the size of each A and B subarray that will be merged in the next level

		if (!WikiIterator_nextLevel(&iterator))
		{
			break;
		}
	}

	End:

	#if DYNAMIC_CACHE

	if (cache != external_cache)
	{
		free(cache);
	}

	#endif

	return;
}
