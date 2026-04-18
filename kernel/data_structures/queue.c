#include <kernel/include/heap.h>

#include "kernel/include/queue.h"

void init_queue(struct queue* queue) {
	queue->front = NULL;
	queue->rear = NULL;
}

void queue_enqueue(struct queue* queue, void* data) {
	struct queue_node* new_node = kmalloc(sizeof(struct queue_node));
	new_node->data = data;
	new_node->next = NULL;

	if (queue->rear == NULL) {
		queue->front = new_node;
		queue->rear = new_node;
	} else {
		queue->rear->next = new_node;
		queue->rear = new_node;
	}
}

void* queue_dequeue(struct queue* queue) {
	if (queue_is_empty(queue)) {
		return NULL;
	}

	struct queue_node* front_ptr = queue->front;
	void* data = queue->front->data;

	if (queue->front == queue->rear) {
		queue->rear = NULL;
	}
	queue->front = queue->front->next;

	kfree(front_ptr);
	return data;
}

void* queue_peek(struct queue* queue) {
	if (queue_is_empty(queue)) {
		return NULL;
	}

	return queue->front->data;
}

bool queue_is_empty(struct queue* queue) { return (queue->front == NULL); }
