#ifndef HOUSESTATUS_H
#define HOUSESTATUS_H

#include <stdint.h>

// Define the structure for home status
typedef struct {
	uint8_t window1;
	uint8_t window2;
	uint8_t door;
	uint8_t lock;
	uint8_t light;
	char keyboardChar;
} HouseStatus;

// Function prototypes (if needed)
// void initializeHomeStatus(HomeStatus *status);

#endif // HOUSESTATUS_H