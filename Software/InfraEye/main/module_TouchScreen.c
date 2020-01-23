#include "XPT2046.h"

uint32_t Touch_Init(void)
{
	uint32_t u32Return = 0;

	u32Return = XPT_2046_Init();
	return(u32Return);
}

uint32_t Touch_GetSome(void)
{
	uint8_t u8_return = 0;
	uint32_t X = 0;

	X = XPT_2046_GET_X();
	return(X);
}
