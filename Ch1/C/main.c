#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node
{
	struct Node* prev;
	struct Node* next;
	char* string;
};

struct DLinkedList
{
	struct Node* beginning;
	struct Node* end;
};

struct Node* new_node(char* value, int size)
{
	struct Node* node = (struct Node*)malloc(sizeof(struct Node));
	node->string = (char*)malloc(sizeof(char) * size);
	strncpy(node->string, value, size);
	return node;
}

void insert_before(struct Node* target, struct Node* new)
{
	if (target->prev != NULL)
	{
		new->next = target;
		new->prev = target->prev;

		target->prev->next = new;
		target->prev = new;
	}
	else
	{
		target->prev = new;
		new->next = target;
	}
}

void insert_after(struct Node* target, struct Node* new)
{
	if (target->next != NULL)
	{
		new->prev = target;
		new->next = target->next;

		target->next->prev = new;
		target->next = new;
	}
	else
	{
		target->next = new;
		new->prev = target;
	}

}

struct DLinkedList newDoubleLinkedList(char* init_value, int value_size)
{
	struct Node* init_node = new_node(init_value, value_size);
	init_node->prev = NULL;
	init_node->next = NULL;

	struct DLinkedList new_list = {
		init_node,
		init_node
	};

	return new_list;
}

struct Node* insert(struct DLinkedList* list, int index, char* value, int value_size)
{
	struct Node* target_node = list->beginning;
	for (int i = 0; (i < index - 1) && (target_node->next != NULL); i++)
	{
		target_node = target_node->next;
	}

	struct Node* new = new_node(value, value_size);
	
	insert_after(target_node, new);

	return new;
}

void delete(struct DLinkedList* list, int index)
{
	struct Node* target_node = list->beginning;
	for (int i = 0; (i < index) && (target_node->next != NULL); i++)
	{
		target_node = target_node->next;
	}
	
	target_node->prev->next = target_node->next;
	target_node->next->prev = target_node->prev;
	free(target_node->string);
	free(target_node);
}

struct Node* find(struct DLinkedList* list, char* value)
{
	struct Node* target_node = list->beginning;
	while (strcmp(target_node->string, value) && target_node->next != NULL)
	{
		target_node = target_node->next;
	}

	if (!strcmp(target_node->string, value))
		return target_node;
	return NULL;
}

void print_list(struct DLinkedList* list)
{
	struct Node* ptr = list->beginning;
	while (ptr->next != NULL)
	{
		printf("%s, ", ptr->string);
		ptr = ptr->next;
	}

	printf("%s\n", ptr->string);
}

int main()
{
	// Instantiate list and insert three values, while inserting the last one in the middle
	struct DLinkedList myList = newDoubleLinkedList("First value :P", 15);
	insert(&myList, 1, "Seconnnd value", 15);
	insert(&myList, 1, "2.5th value", 12);
	print_list(&myList); 

	// Find a value
	struct Node* result = find(&myList, "2.5th value");
	if (result != NULL)
	{
		printf("Found value!!\n");
	}
	else
	{
		printf("Value doesnt exist\n");
	}

	// Delete a value and print list
	delete(&myList, 1);
	print_list(&myList);

	return 0;
}
