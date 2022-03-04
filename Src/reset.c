#include "main.h"
#include "reset.h"

void SystemReset(void)
{
    __NVIC_SystemReset();
}
