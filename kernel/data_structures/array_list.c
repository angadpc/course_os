/*
 * array_list.c
 *
 *  Created on: Apr 21, 2015
 *      Author: kittenRainbow
 */

#include "../include/array_list.h"
#include "../include/klibc.h"

/**
 * Creates an arraylist with default bucket size
 * Params:
 * 		None
 * Return: arrl_handle*
 */
arrl_handle* arrl_create() {
	return arrl_create_fixed(DEFAULT_BUCKET_SIZE);
}

/**
 * Creates an arraylist with desired bucket size
 * Params:
 * 		uint32_t: desired bucket size
 * Return: arrl_handle*
 */
arrl_handle* arrl_create_fixed(uint32_t bucket_size) {
	arrl_handle* result = (arrl_handle *) kmalloc(sizeof(arrl_handle));
	result->list = (void***) kmalloc(sizeof(void**));
	result->list[0] = (void**) kmalloc(bucket_size * sizeof(void*));
	result->size = 0;
	result->capacity = bucket_size;
	result->bucket_size = bucket_size;
	return result;
}

/**
 * appends an element pointer to the end of the array
 * Params:
 * 		arrl_handle*: arraylist to which element will be added to
 * 		void*: pointer to element to be added
 * Return: void
 *
 */
void arrl_append(arrl_handle* arrl, void* elem) {
	uint32_t list_index;
	uint32_t bucket_index;

	list_index = arrl->size / arrl->bucket_size;
	bucket_index = arrl->size % arrl->bucket_size;

	if (arrl->capacity == arrl->size) {
		void*** list = (void***) kmalloc(
				(arrl->size / arrl->bucket_size + 1) * sizeof(void**));
		for (uint32_t index = 0; index < list_index; ++index) {
			list[index] = arrl->list[index];
		}

		kfree(arrl->list);
		arrl->list = list;
		list[list_index] = (void**) kmalloc(sizeof(void*) * arrl->bucket_size);
		arrl->capacity += arrl->bucket_size;
	}

	arrl->list[list_index][bucket_index] = elem;
	++arrl->size;
}

/**
 * returns an element from arraylist by index
 * Params:
 * 		arrl_handle*: arraylist from which element will be returned
 * 		void*: index of the element to be returned
 * Return: pointer to element to be returned
 *
 */
void* arrl_get(arrl_handle* arrl, uint32_t index) {
	uint32_t list_index;
	uint32_t bucket_index;

	if (index >= arrl->size) {
		return 0;
	}

	list_index = index / arrl->bucket_size;
	bucket_index = index % arrl->bucket_size;
	return arrl->list[list_index][bucket_index];
}

/**
 * removes the given element from arraylist
 * Params:
 * 		arrl_handle*: arraylist from which element will be removed
 * 		void*: index of the element to be removed
 * Return: pointer to element to be removed
 *
 */
void* arrl_remove(arrl_handle* arrl, void* elem) {
	uint32_t bucket_size;
	uint32_t last_bucket_size;
	uint32_t list_size;
	uint32_t bucket_index;
	uint32_t list_index;

	bucket_size = arrl->bucket_size;
	list_size = arrl->size / bucket_size;
	last_bucket_size = arrl->size % bucket_size;

	for (list_index = 0; list_index < list_size; ++list_index) {
		for (bucket_index = 0; bucket_index < bucket_size; ++bucket_index) {
			if (elem == arrl->list[list_index][bucket_index]) {
				goto out;
			}
		}
	}

	for (bucket_index = 0; bucket_index < last_bucket_size; ++bucket_index) {
		if (elem == arrl->list[list_index][bucket_index]) {
			goto out;
		}
	}

	return 0;

	out: os_memcpy(&(arrl->list[list_index][bucket_index + 1]),
			&(arrl->list[list_index][bucket_index]),
			(bucket_size - bucket_index - 1) * sizeof(void*));
	++list_index;
	bucket_index = 0;

	while (list_index < list_size) {
		arrl->list[list_index - 1][bucket_size - 1] = arrl->list[list_index][0];
		os_memcpy(&(arrl->list[list_index][bucket_index + 1]),
				&(arrl->list[list_index][bucket_index]),
				(bucket_size - bucket_index - 1) * sizeof(void*));
		++list_index;
	}

	if (arrl->size > list_size * bucket_size) {
		os_memcpy(&(arrl->list[list_index][bucket_index + 1]),
				&(arrl->list[list_index][bucket_index]),
				(last_bucket_size - 1) * sizeof(void*));
	}
	--arrl->size;

	return elem;
}

/**
 * frees given arraylist
 * Params:
 * 		arrl_handle*: arraylist to be freed
 * Return: void
 *
 */
void arrl_free(arrl_handle* arrl) {
	uint32_t array_size = arrl->capacity / arrl->bucket_size;
	for (uint32_t index = 0; index < array_size; ++index) {
		kfree(arrl->list[index]);
	}
	kfree(arrl->list);
	kfree(arrl);
}

/*
 void arrl_remove_all(arrl_handle* arrl, void* elem) {
 uint32_t bucket_size;
 uint32_t next_index;
 uint32_t bucket_index;
 uint32_t index;
 uint32_t start_index;

 bucket_size = arrl->bucket_size;
 start_index = 0;
 size = arrl->list[arrl->size / arrl->bucket_size][arrl->size
 % arrl->bucket_size - 1];

 index = __find_next(arrl, start_index, elem);
 if (index == -1) {
 return;
 }

 do {
 next_index = __find_next(arrl, index, elem);
 if (next_index == 0) {
 arrl_remove(arrl, elem);
 return;
 }
 for (uint32_t i = index; i < next_index / arrl->bucket_size; ++i) {
 os_memcpy(arrl->list[i] + ,
 arrl->list[list_index][bucket_index],
 (bucket_size - bucket_index - 1) * sizeof(void*));
 }

 bucket_index = 0;
 arrl->list[list_index][bucket_size - 1] = arrl->list[list_index + 1][0];
 ++list_index;

 os_memcpy(arrl->list[list_index][bucket_index + 1],
 arrl->list[list_index][bucket_index],
 ((arrl->size % bucket_index) - 1) * sizeof(void*));
 --arrl->size;
 } while (list_index != list size);
 }

 */

/**
 * returns 1 if element exists in arraylist, 0 otherwise
 * Params:
 * 		arrl_handle*: arraylist to search element in
 * 		void*: pomiter to element to be searched
 * Return: 1 if arraylist contains element, 0 otherwise
 *
 */
uint32_t arrl_contains(arrl_handle* arrl, void* elem) {
	uint32_t list_index;
	uint32_t bucket_index;
	uint32_t bucket_size;
	uint32_t last_bucket_size;
	uint32_t list_size;

	bucket_size = arrl->bucket_size;
	last_bucket_size = arrl->size % bucket_size;
	list_size = arrl->size / bucket_size;

	for (list_index = 0; list_index < list_size; ++list_index) {
		for (bucket_index = 0; bucket_index < bucket_size; ++bucket_index) {
			if (elem == arrl->list[list_index][bucket_index]) {
				return 1;
			}
		}
	}

	for (uint32_t bucket_index = 0; bucket_index < last_bucket_size;
			++bucket_index) {
		if (elem == arrl->list[list_index][bucket_index]) {
			return 1;
		}
	}
	return 0;
}

/**
 * returns the index of given element in arraylist
 * Params:
 * 		arrl_handle*: arraylist in which element will be looked in
 * 		void*: element whose index is being searched for
 * Return: index of the element if found, -1 otherwise
 *
 */
uint32_t arrl_index_of(arrl_handle* arrl, void* elem) {
	uint32_t list_index;
	uint32_t bucket_index;
	uint32_t bucket_size;
	uint32_t last_bucket_size;
	uint32_t list_size;

	bucket_size = arrl->bucket_size;
	last_bucket_size = arrl->size % bucket_size;
	list_size = arrl->size / bucket_size;

	for (list_index = 0; list_index < list_size; ++list_index) {
		for (bucket_index = 0; bucket_index < bucket_size; ++bucket_index) {
			if (elem == arrl->list[list_index][bucket_index]) {
				return list_index * arrl->bucket_size + bucket_index;
			}
		}
	}

	for (bucket_index = 0; bucket_index < last_bucket_size; ++bucket_index) {
		if (elem == arrl->list[list_index][bucket_index]) {
			return list_index * arrl->bucket_size + bucket_index;
		}
	}

	return -1;
}

/**
 * returns the size of the arraylist
 * Params:
 * 		arrl_handle*: arraylist whose size needs to be returned
 * Return: size of the arraylist
 *
 */
uint32_t arrl_count(arrl_handle* arrl) {
	return arrl->size;
}

uint32_t __find_next(arrl_handle* arrl, uint32_t start, void* elem) {
	uint32_t list_index;
	uint32_t list_size;
	uint32_t bucket_size;

	for (uint32_t list_index = start / arrl->bucket_size;
			list_index < list_size; ++list_index) {
		for (uint32_t bucket_index = start % arrl->bucket_size;
				bucket_index < bucket_size; ++bucket_index) {
			if (arrl->list[list_index][bucket_index] == elem) {
				return list_index * arrl->bucket_size + bucket_index;
			}
		}
	}
	return -1;
}

