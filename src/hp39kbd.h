#ifndef _HP39KBD_H
#define _HP39KBD_H

#include "s3c2410.h"

#define modifier_key_pressed(bit) ((*GPFDAT >> (bit)) & 1)
#define on_pressed    modifier_key_pressed(0)
#define comma_pressed modifier_key_pressed(4)
#define alpha_pressed modifier_key_pressed(5)
#define shift_pressed modifier_key_pressed(6)

#define any_normal_key_pressed ((*GPGDAT & 0xFE) != 0xFE)
#define any_key_pressed ( \
	any_normal_key_pressed || on_pressed || \
	comma_pressed || alpha_pressed || shift_pressed \
)

int get_key(void);

#endif
