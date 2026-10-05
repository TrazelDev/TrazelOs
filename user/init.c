#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "doomgeneric/doomgeneric/doomgeneric.h"
#include "doomgeneric/doomgeneric/doomkeys.h"

static int64_t _syscall(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
	int64_t ret;
	asm volatile("syscall"
				 : "=a"(ret)
				 : "a"(syscall_num), "D"(arg1), "S"(arg2), "d"(arg3)
				 : "rcx", "r11", "memory");
	return ret;
}

uint32_t* framebuffer_ptr;
void DG_Init() { framebuffer_ptr = (uint32_t*)_syscall(201, (uint64_t)&framebuffer_ptr, 0, 0); }

void DG_DrawFrame() {
	int fb_stride = 4096 / 4;

	// Render Doom's buffer to the top-left corner of the framebuffer
	for (int y = 0; y < DOOMGENERIC_RESY; y++) {
		for (int x = 0; x < DOOMGENERIC_RESX; x++) {
			uint32_t pixel = DG_ScreenBuffer[y * DOOMGENERIC_RESX + x];

			// Write to os framebuffer:
			framebuffer_ptr[y * fb_stride + x] = pixel;
		}
	}
}

// uint32_t DG_GetTicksMs() { return 0; }
// uint32_t DG_GetTicksMs() { return (uint32_t)_syscall(200, 0, 0, 0); }
uint32_t DG_GetTicksMs() {
	struct timeval tv;
	(uint32_t)_syscall(96, (size_t)(&tv), 0, 0);
	return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

void DG_SleepMs(uint32_t ms) {
	uint32_t start = DG_GetTicksMs();
	/* A quick-and-dirty spinlock sleep */
	while (DG_GetTicksMs() - start < ms) {
		asm volatile("pause");
	}
}
static unsigned char TranslateKey(uint16_t code) {
	switch (code) {
		// --- Movement (Arrow Keys / Numpad) ---
		case 0x48:
			return 173;	 // Up Arrow -> DOOMKEY_UPARROW
		case 0x50:
			return 175;	 // Down Arrow -> DOOMKEY_DOWNARROW
		case 0x4B:
			return 172;	 // Left Arrow -> DOOMKEY_LEFTARROW
		case 0x4D:
			return 174;	 // Right Arrow -> DOOMKEY_RIGHTARROW

		// --- Action / Modifiers ---
		case 0x1D:
			return 163;	 // Left Ctrl -> DOOMKEY_RCTRL (Fire)

		case 0x29:			 // space key
		case 16:			 // the 'q' key
			return KEY_USE;	 // Spacebar -> Open doors / Use
		case 0x2A:			 // Left Shift
		case 0x36:
			return 182;	 // Right Shift -> DOOMKEY_RSHIFT (Run)
		case 0x38:
			return 184;	 // Left Alt -> DOOMKEY_RALT (Strafe)

		// --- UI & Menu ---
		case 0x01:
			return 27;	// Escape -> DOOMKEY_ESCAPE
		case 0x1C:
			return 13;	// Enter -> DOOMKEY_ENTER
		case 0x0F:
			return 9;  // Tab -> DOOMKEY_TAB (Automap)
		case 0x15:
			return 'y';	 // Y key -> Confirm
		case 0x31:
			return 'n';	 // N key -> Cancel

		// --- Weapons (1-7) ---
		case 0x02:
			return '1';	 // Fist / Chainsaw
		case 0x03:
			return '2';	 // Pistol
		case 0x04:
			return '3';	 // Shotgun
		case 0x05:
			return '4';	 // Chaingun
		case 0x06:
			return '5';	 // Rocket Launcher
		case 0x07:
			return '6';	 // Plasma Rifle
		case 0x08:
			return '7';	 // BFG9000

		// --- Optional WASD Support ---
		case 0x11:
			return 173;	 // W
		case 0x1F:
			return 175;	 // S
		case 0x1E:
			return 172;	 // A
		case 0x20:
			return 174;	 // D

		default:
			return 0;  // Unmapped key
	}
}

int DG_GetKey(int* pressed, unsigned char* key) {
	int bytes_read = read(1, key, sizeof(unsigned char));

	if (bytes_read == -1) {
		return 0;
	}

	*pressed = (*key & 0x80) ? 0 : 1;
	*key &= 0x7F;
	*key = TranslateKey(*key);
	if (*key == 0) {
		return 0;
	}

	return 1;
}

void DG_SetWindowTitle(const char* title) {}

// Fake mkdir for doom
int mkdir(const char* path, mode_t mode) { return 0; }

int main() {
	doomgeneric_Create(0, NULL);
	while (1) {
		doomgeneric_Tick();
	}
	while (true);
	return 0;
}
