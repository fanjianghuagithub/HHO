
#ifndef __SPT513_H__
#define __SPT513_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPT513_IRQ_GPIO HAL_GPIO_PIN_P0_1
void spt513_init(void);
extern bool earin_flg;

#ifdef __cplusplus
}
#endif

#endif

