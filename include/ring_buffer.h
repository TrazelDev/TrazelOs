#pragma once
#include "types.h"

struct ring_buffer {
	uint64_t write_ptr;
	uint64_t read_ptr;
	uint64_t buffer_size;
	uint8_t* buffer;
};

bool ring_buffer_init(struct ring_buffer* ring_buffer, uint8_t* buffer, uint64_t buffer_size);

bool ring_buffer_push(struct ring_buffer* ring_buffer, uint8_t data);
bool ring_buffer_pop(struct ring_buffer* ring_buffer, uint8_t* data);
bool ring_buffer_is_empty(struct ring_buffer* ring_buffer);
