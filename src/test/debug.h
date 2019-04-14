#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG
    #include "uart.h"
    #define DEBUG_INIT()    uart_init()
    #define DEBUG_STR(s)    uart_send_string(s)
    #define DEBUG_C(c)      uart_send(c)
    #define DEBUG_HEX8(c)   uart_hexstring(c, 8, true)
#else
    #define DEBUG_INIT()
    #define DEBUG_STR(s)
    #define DEBUG_C(c)
    #define DEBUG_HEX8(c)
#endif

#endif /* _DEBUG_H_ */
