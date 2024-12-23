#include "ite/ite_sd.h"
#include "sdio_reg.h"
#include "hal_sdio.h"

//#define sdio_claim_host             iteSdioClaimHost
//#define sdio_release_host           iteSdioReleaseHost
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

int  wifi_read(struct sdio_func *func, u32 addr, u32 cnt, void *pdata);
int  wifi_write(struct sdio_func *func, u32 addr, u32 cnt, void *pdata);
u8   wifi_readb(struct sdio_func *func, u32 addr);
u16  wifi_readw(struct sdio_func *func, u32 addr);
u32  wifi_readl(struct sdio_func *func, u32 addr);
void wifi_writeb(struct sdio_func *func, u32 addr, u8 val);
void wifi_writew(struct sdio_func *func, u32 addr, u16 v);
void wifi_writel(struct sdio_func *func, u32 addr, u32 v);
u8   wifi_readb_local(struct sdio_func *func, u32 addr);
void wifi_writeb_local(struct sdio_func *func, u32 addr, u8 val);


/* ==================== Porting from host interrupt ==================== */
#include "ite/ite_mt7682.h"

bool sdio_get_interrupt_flag(void);
void sdio_reset_interrupt_flag(void);
uint32_t sdio_get_interrupt_reg(void);
uint32_t sdio_read_isr(void);
bool sdio_hif_enable_interrupt(struct sdio_func *func);
int  sdio_create_interrupt_tasklet(void);

int  sdio_rw_process(struct sdio_func *func);
int  sdio_rx_process(struct sdio_func *func);
int  sdio_tx_process(struct sdio_func *func);
int  sdio_func1_rd(struct sdio_func *func, uint32_t u4Register, void *pBuffer,  uint32_t Length);
int  sdio_func1_wr(struct sdio_func *func, uint32_t u4Register, void *pBuffer,  uint32_t Length);

hal_sdio_status_t sdio_interrupt_callback_init(void); //MT7682 sdio_init

int  sdio_set_interrupt_request(struct sdio_func *func);
void gpio_set_interrupt_request(struct sdio_func *func);
/* ========================================================== */



typedef struct _SDIO_BUS_OPS{
	int (*bus_probe)(void);
	int (*bus_remove)(void);
	int (*enable)(struct sdio_func*func);	/*enables a SDIO function for usage*/
	int (*disable)(struct sdio_func *func);
	int (*reg_driver)(struct sdio_driver*); /*register sdio function device driver callback*/
	void (*unreg_driver)(struct sdio_driver *);
	int (*claim_irq)(struct sdio_func *func, void(*handler)(struct sdio_func *));
	int (*release_irq)(struct sdio_func*func);
	void (*claim_host)(struct sdio_func*func);	/*exclusively claim a bus for a certain SDIO function*/
	void (*release_host)(struct sdio_func *func);
	unsigned char (*readb)(struct sdio_func *func, unsigned int addr, int *err_ret);/*read a single byte from a SDIO function*/
	unsigned short (*readw)(struct sdio_func *func, unsigned int addr, int *err_ret);	/*read a 16 bit integer from a SDIO function*/
	unsigned int (*readl)(struct sdio_func *func, unsigned int addr, int *err_ret); /*read a 32 bit integer from a SDIO function*/
	void (*writeb)(struct sdio_func *func, unsigned char b,unsigned int addr, int *err_ret);	/*write a single byte to a SDIO function*/
	void (*writew)(struct sdio_func *func, unsigned short b,unsigned int addr, int *err_ret);	/*write a 16 bit integer to a SDIO function*/
	void (*writel)(struct sdio_func *func, unsigned int b,unsigned int addr, int *err_ret); /*write a 32 bit integer to a SDIO function*/
	int (*memcpy_fromio)(struct sdio_func *func, void *dst,unsigned int addr, int count);/*read a chunk of memory from a SDIO functio*/
	int (*memcpy_toio)(struct sdio_func *func, unsigned int addr,void *src, int count);  /*write a chunk of memory to a SDIO function*/
}SDIO_BUS_OPS;
extern SDIO_BUS_OPS ite_sdio_bus_ops;

extern struct sdio_func *wifi_sdio_func;


