
#import <unistd.h>
#import <stdlib.h>
#import <stdio.h>
#import <sys/types.h>
#import <strings.h>

#import "ptrlist.h"

#ifdef TESTING
#define DEBUG
#endif TESTING

static __inline__ void
ptrlist_print(ptrlist_t * list)
{
    printf("ptrlist count %d, size %d\n", list->count, list->size);
    if (list->count > 0) {
	int i;
	for (i = 0; i < list->count; i++)
	    printf("%d. %x\n", i, (unsigned int)list->array[i]);
    }
    return;
}

void
ptrlist_init(ptrlist_t * list)
{
    bzero(list, sizeof(*list));
    return;
}

void
ptrlist_init_size(ptrlist_t * list, int size)
{
    bzero(list, sizeof(*list));
    if (size > 0) {
	list->size = size;
	list->array = malloc(sizeof(*list->array) * list->size);
	if (list->array == NULL) {
	    list->size = 0;
	}
    }
    return;
}

void
ptrlist_free(ptrlist_t * list)
{
    if (list->array)
	free(list->array);
    ptrlist_init(list);
    return;
}

int
ptrlist_index(ptrlist_t * list, void * element)
{
    int i;
    for (i = 0; i < ptrlist_count(list); i++) {
	if (ptrlist_element(list, i) == element)
	    return (i);
    }
    return (-1);
}

int
ptrlist_count(ptrlist_t * list)
{
    if (list == NULL || list->array == NULL)
	return (0);

    return (list->count);
}

void *
ptrlist_element(ptrlist_t * list, int i)
{
    if (list->array == NULL)
	return (NULL);
    if (i < list->count)
	return (list->array[i]);
    return (NULL);
}

boolean_t
ptrlist_remove(ptrlist_t * list, int i, void * * ret)
{
    int		nmove;

    if (list->array == NULL || i >= list->count	|| i < 0)
	return (FALSE);
    if (ret)
	*ret = list->array[i];
    nmove = (list->count - 1) - i;
    if (nmove > 0) {
	bcopy(list->array + (i + 1),
	      list->array + i,
	      nmove * sizeof(*list->array));
    }
    list->count--;
    return (TRUE);
}

static boolean_t
ptrlist_grow(ptrlist_t * list)
{
    if (list->array == NULL) {
	if (list->size == 0)
	    list->size = PTRLIST_NUMBER;
	list->count = 0;
	list->array = malloc(sizeof(*list->array) * list->size);
    }
    else if (list->size == list->count) {
#ifdef DEBUG
	printf("doubling %d to %d\n", list->size, list->size * 2);
#endif DEBUG
	list->size *= 2;
	list->array = realloc(list->array,
			      sizeof(*list->array) * list->size);
    }
    if (list->array == NULL)
	return (FALSE);
    return (TRUE);
}

boolean_t
ptrlist_add(ptrlist_t * list, void * element)
{
    if (ptrlist_grow(list) == FALSE)
	return (FALSE);

    list->array[list->count++] = element;
    return (TRUE);
}

boolean_t
ptrlist_insert(ptrlist_t * list, void * element, int where)
{
    if (where < 0)
	return (FALSE);

    if (where >= list->count)
	return (ptrlist_add(list, element));

    if (ptrlist_grow(list) == FALSE)
	return (FALSE);
    
    /* open up a space for 1 element */
    bcopy(list->array + where,
	  list->array + where + 1,
	  (list->count - where) * sizeof(*list->array));
    list->array[where] = element;
    list->count++;
    return (TRUE);

}

boolean_t
ptrlist_dup(ptrlist_t * dest, ptrlist_t * source)
{
    ptrlist_init(dest);
    dest->size = dest->count = source->count;
    if (source->count == 0)
	return (TRUE);
    dest->array = malloc(sizeof(*dest->array) * dest->size);
    if (dest->array == NULL)
	return (FALSE);
    bcopy(source->array, dest->array, sizeof(*dest->array) * dest->size);
    return (TRUE);
}

/* concatenates extra onto list */
boolean_t
ptrlist_concat(ptrlist_t * list, ptrlist_t * extra)
{
    if (extra->count == 0)
	return (TRUE);

    if ((extra->count + list->count) > list->size) {
	list->size = extra->count + list->count;
	if (list->array == NULL)
	    list->array = malloc(sizeof(*list->array) * list->size);
	else
	    list->array = realloc(list->array, 
				  sizeof(*list->array) * list->size);
    }
    if (list->array == NULL)
	return (FALSE);
    bcopy(extra->array, list->array + list->count, 
	  extra->count * sizeof(*list->array));
    list->count += extra->count;
    return (TRUE);
}


#ifdef TESTING
int 
main(int argc, char * argv[])
{
    int i;

    ptrlist_t list1, list2, list3;

    ptrlist_init_size(&list1, 1);
    ptrlist_init(&list2);
    ptrlist_init(&list3);

    for (i = 0; i < 10; i++) {
	ptrlist_add(&list1, (void *)i);
    }
    printf("list1:\n");
    ptrlist_print(&list1);

    for (i = 10; i < 20; i++) {
	ptrlist_add(&list2, (void *) i);
    }
    printf("\nlist2\n");
    ptrlist_print(&list2);

    ptrlist_dup(&list1, &list3);
    printf("\nlist3\n");
    ptrlist_print(&list3);
    
    ptrlist_concat(&list3, &list2);
    printf("\nlist3\n");
    ptrlist_print(&list3);

    {
	void * val;
	
	if (ptrlist_remove(&list3, 7, &val)) {
	    printf("Element at index 7 has value %d\n", val);
	}
	printf("\nlist3\n");
	ptrlist_print(&list3);
	if (ptrlist_remove(&list3, 0, &val)) {
	    printf("Element at index 0 has value %d\n", val);
	}
	printf("\nlist3\n");
	ptrlist_print(&list3);
	if (ptrlist_remove(&list3, ptrlist_count(&list3) - 1, &val)) {
	    printf("Element at index %d has value %d\n", 
		   ptrlist_count(&list3),  val);
	}
	printf("\nlist3\n");
	ptrlist_print(&list3);
	
    }

    for (i = -20; i < -10; i++) {
	ptrlist_insert(&list2, (void *)i, 0);
    }
    printf("\nlist2\n");
    ptrlist_print(&list2);

    ptrlist_free(&list1);
    ptrlist_free(&list2);
    ptrlist_free(&list3);
    exit(0);
    return 0;
}
#endif TESTING
