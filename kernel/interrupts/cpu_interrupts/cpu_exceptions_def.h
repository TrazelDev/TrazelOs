#pragma once

enum cpu_exceptions_indexes {
	CEI_DIVIDE_ERROR = 0x0,
	CEI_DOUBLE_FAULT = 0x8,
	CEIPAGE_FAULT = 0xE,
};
