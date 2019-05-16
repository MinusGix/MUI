#ifndef FILE_M_MUTIL
#define FILE_M_MUTIL

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>

#define MLIST_INITIAL_CAPACITY 30

#define CREATE_MLIST_HEADER(sname, vname, rtype)  typedef struct sname##_t { \
	rtype* *data; /* pointer to start of data */ \
	size_t size; /* 'used' slots that have data, may still be NULL */ \
	size_t capacity; /* available slots */ \
} sname; \
void vname##_init (sname* list); \
bool is_##vname##_full (sname* list); \
void vname##_double_capacity (sname* list); \
void vname##_append (sname* list, rtype* item); \
rtype* vname##_get (sname* list, size_t index); \
void vname##_set (sname* list, size_t index, rtype* item); \
void vname##_free (sname* list); \
size_t vname##_length (sname* list); \
size_t vname##_allocated_length (sname* list); \
rtype* vname##_pop (sname* list);

// I feel sinful
#define CREATE_MLIST(sname, vname, rtype) void vname##_init (sname* list) { \
	list->size = 0; \
	list->capacity = MLIST_INITIAL_CAPACITY; \
	list->data = malloc(sizeof(sname) * list->capacity); \
	/* TODO: check for null and return NULL if it is bad */ \
} \
bool is_##vname##_full (sname* list) { \
	return list->size >= list->capacity; \
} \
/*TODO: add shift, TODO: add unshift, TODO: add remove (removes item at index, freeing it)*/ \
void vname##_double_capacity (sname* list) { \
	/* TODO: check for overflows of size_t */ \
	if (list->size >= list->capacity) { \
		list->capacity *= 2; \
		list->data = realloc(list->data, sizeof(rtype*) * list->capacity); \
	} \
} \
void vname##_append (sname* list, rtype* item) { \
	if (is_##vname##_full(list)) { \
		vname##_double_capacity(list); \
	} \
	list->data[list->size++] = item; \
} \
rtype* vname##_get (sname* list, size_t index) { \
	if (index >= list->size) { \
		return NULL; \
	} \
	return list->data[index]; \
} \
void vname##_set (sname* list, size_t index, rtype* item) { \
	while (index >= list->capacity) { \
		vname##_append(list, NULL); \
	} \
	list->data[index] = item; \
} \
void vname##_free (sname* list) { \
	if (list->data != NULL) { \
		for (size_t i = 0; i < list->size; i++) { \
			if (list->data[i] != NULL) { \
				free(list->data[i]); \
			} \
		} \
		free(list->data); \
	} \
} \
size_t vname##_length (sname* list) { \
	return list->size; \
} \
size_t vname##_allocated_length (sname* list) { \
	return list->capacity; \
} \
rtype* vname##_pop (sname* list) { \
	size_t last_index = vname##_length(list) - 1; \
	rtype* item = vname##_get(list, last_index); \
	if (item != NULL) { \
		vname##_set(list, last_index, NULL); \
		list->size--; \
	} \
	return item; \
}


#define CREATE_MLINKEDLIST_HEADER(sname, vname, rtype) typedef struct sname##_node_t { \
	rtype data; \
	struct sname##_node_t* next; \
} sname##_node; \
typedef struct sname##_t { \
	sname##_node* head; \
} sname; \
sname##_node* create_##vname##_node (rtype data); \
sname* create_##vname(); \
void init_##vname (sname* list); \
void vname##_add (sname* list, rtype data); \
void vname##_reverse (sname* list); \
void vname##_free (sname* list); \
sname##_node* vname##_shift (sname* list); \
sname##_node* vname##_pop (sname* list);


#define CREATE_MLINKEDLIST(sname, vname, rtype) sname##_node* create_##vname##_node (rtype data) { \
	sname##_node* node = (sname##_node*) malloc(sizeof(sname##_node)); \
	if (node == NULL) { \
		return NULL; \
	} \
	node->data = data; \
	node->next = NULL; \
	return node; \
} \
void init_##vname (sname* list) { \
	list->head = NULL; \
} \
sname* vname##_create () { \
	sname* list = (sname*) malloc(sizeof(sname)); \
	if (list == NULL) { \
		return NULL; \
	} \
	init_##vname(list); \
	return list; \
} \
void vname##_add (sname* list, rtype data) { \
	if (list->head == NULL) { \
		list->head = create_##vname##_node(data); \
	} else { \
		sname##_node* current = list->head; \
		while (current->next != NULL) { \
			current = current->next; \
		} \
		current->next = create_##vname##_node(data); \
	} \
} \
sname##_node* vname##_shift (sname* list) { \
	if (list->head == NULL) { \
		return NULL; \
	} \
	sname##_node* ret = list->head; \
	list->head = list->head->next; \
	return ret; \
} \
/* your job to extract the value, and free the node */ \
sname##_node* vname##_pop (sname* list) { \
	if (list->head == NULL) { \
		return NULL; \
	} \
	sname##_node* prev = NULL; \
	sname##_node* current = list->head; \
	while (current->next != NULL) { \
		prev = current; \
		current = current->next; \
	} \
	if (prev == NULL) { \
		list->head = NULL; \
	} else { \
		prev->next = NULL; \
	} \
	return current; \
}

#endif