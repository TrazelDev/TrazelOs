#include <ring_buffer.h>

bool ring_buffer_init(struct ring_buffer* ring_buffer, uint8_t* buffer, uint64_t buffer_size) {
	if (buffer == NULL || buffer_size == 0) {
		return false;
	}

	ring_buffer->buffer = buffer;
	ring_buffer->buffer_size = buffer_size;
	ring_buffer->write_ptr = 0;
	ring_buffer->read_ptr = 0;

	return true;
}

bool ring_buffer_push(struct ring_buffer* ring_buffer, uint8_t data) {
	if ((ring_buffer->write_ptr + 1) % ring_buffer->buffer_size == ring_buffer->read_ptr) {
		return false;
	}

	ring_buffer->buffer[ring_buffer->write_ptr] = data;
	ring_buffer->write_ptr = (ring_buffer->write_ptr + 1) % ring_buffer->buffer_size;
	return true;
}

bool ring_buffer_pop(struct ring_buffer* ring_buffer, uint8_t* data) {
	if (ring_buffer->read_ptr == ring_buffer->write_ptr) {
		return false;
	}

	*data = ring_buffer->buffer[ring_buffer->read_ptr];
	ring_buffer->read_ptr = (ring_buffer->read_ptr + 1) % ring_buffer->buffer_size;
	return true;
}

bool ring_buffer_is_empty(struct ring_buffer* ring_buffer) {
	return ring_buffer->read_ptr == ring_buffer->write_ptr;
}
