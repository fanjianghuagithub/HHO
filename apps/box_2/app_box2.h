
#ifndef __APP_BOX_H__
#define __APP_BOX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_key.h"
int app_box_uart_open(void);

//int app_uart_send2box(uint8_t *buf,uint8_t len);
int app_uart_send2box(uint8_t * buf, uint8_t len);
void init_delay_thread(void);




#ifdef __cplusplus
}
#endif

#endif
