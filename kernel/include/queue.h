#pragma once
#include <include/types.h>

struct queue_node {
	struct queue_node* next;
	void* data;
};

struct queue {
	struct queue_node* front;
	struct queue_node* rear;
};

void init_queue(struct queue* queue);
void queue_enqueue(struct queue* queue, void* data);

/** @return data on success, NULL if queue is empty */
void* queue_dequeue(struct queue* queue);

/** @return data on success, NULL if queue is empty */
void* queue_peek(struct queue* queue);

bool queue_is_empty(struct queue* queue);
