#include <unistd.h>
#include "ite/ite_mac.h"


#define CLK_FROM_PHY
#define PHY_INTR_EN

#if 0 // for reference
ITE_ETH_REAL = 1,
ITE_ETH_MAC_LB = 2,
ITE_ETH_PCS_LB_10 = 3,
ITE_ETH_PCS_LB_100 = 4,
ITE_ETH_MDI_LB_10 = 5,
ITE_ETH_MDI_LB_100 = 6
#endif



static struct ethtool_cmd ecmd = {0};
static struct mii_ioctl_data phy_reg = {0, 0, 0, 0};

#define phy_set_page(page)      do { phy_reg.reg_num=31; phy_reg.val_write=page; iteMacIoctl(&phy_reg, IOCSMIIREG); } while(0)
#define phy_reg_read(reg)       do { phy_reg.reg_num=(reg); phy_reg.val_read=0; iteMacIoctl(&phy_reg, IOCGMIIREG); } while(0)
#define phy_reg_write(reg,val)  do { phy_reg.reg_num=(reg); phy_reg.val_write=(val); iteMacIoctl(&phy_reg, IOCSMIIREG); } while(0)
#define phy_reset()             do { phy_reg.reg_num=0; phy_reg.val_write=0x9200; iteMacIoctl(&phy_reg, IOCSMIIREG); } while(0)


void
PhyInit(int ethMode)
{
    uint16_t tmp;
    uint32_t timeout;

    if (ethMode == ITE_ETH_MAC_LB)
        return;

	phy_reg.phy_id = CFG_NET_ETHERNET_PHY_ADDR;
	
    /* phy reset */
    phy_reset();
    timeout = 2000; // ms
    do {
        phy_reg_read(0);
        usleep(1000);
    } while((phy_reg.val_read & 0x8000) && timeout--);
    printf(" after reset: phy reg %d = 0x%04X \n", phy_reg.reg_num, phy_reg.val_read);
	if (!timeout)
		printf("phy reset timeout! \n");
	
    /** CRS can't toggle for RMII */
    phy_set_page(7);
    phy_reg_read(16);
    #if defined(CLK_FROM_PHY)
    phy_reg_write(16, (phy_reg.val_read|(0x1<<2)));
    #else
    phy_reg_write(16, (phy_reg.val_read|((0x1<<12)|(0x1<<2))));
    #endif
    phy_reg_read(16);
    //printf(" phy P7 reg %d = 0x%04X \n", phy_reg.reg_num, phy_reg.val_read);
    phy_set_page(0);

#if defined(PHY_INTR_EN)
    //phy_reg_read(30); /** clear interrupt */
    phy_set_page(7);
    phy_reg_read(19);
    //printf(" P7 reg %d = 0x%04X \n", phy_reg.reg_num, phy_reg.val_read);
	phy_reg.val_read &= ~0x30;  /** LED selection D[5:4]=00 */
    phy_reg_write(19, (phy_reg.val_read|(0x2000)));
    phy_reg_read(19);
    //printf(" P7 reg %d = 0x%04X \n", phy_reg.reg_num, phy_reg.val_read);
    phy_set_page(0);
#endif
    phy_reg_read(0);
    printf(" after init: phy reg %d = 0x%04X \n", phy_reg.reg_num, phy_reg.val_read);


    if (ethMode == ITE_ETH_PCS_LB_10) {
        printf(" PHY PCS 10 loopback! \n\n");
        phy_set_page(0);
        phy_reg_write(0, 0x4100);
        phy_set_page(7);
        tmp = 0x1FF8;
        #if defined(CLK_FROM_PHY)
        tmp &= ~(0x1 << 12);
        #endif
        phy_reg_write(16, tmp);
        phy_set_page(0);
        usleep(20 * 1000);
    } 
    else if(ethMode == ITE_ETH_PCS_LB_100) {
        printf(" PHY PCS 100 loopback! \n\n");
        phy_set_page(0);
        phy_reg_write(0, 0x6100);
        phy_set_page(7);
        tmp = 0x1FF8;
        #if defined(CLK_FROM_PHY)
        tmp &= ~(0x1 << 12);
        #endif
        phy_reg_write(16, tmp);
        phy_set_page(0);
        usleep(20 * 1000);
    }
    else if (ethMode == ITE_ETH_MDI_LB_10) {
        printf(" PHY MDI 10 loopback! \n\n");
        phy_set_page(0);
        phy_reg_write(0, 0x0100);
        usleep(100 * 1000);
    }
    else if (ethMode == ITE_ETH_MDI_LB_100) {
        printf(" PHY MDI 100 loopback! \n\n");
        phy_set_page(0);
        phy_reg_write(0, 0x2100);
        usleep(100 * 1000);
    }
}

/* phy interrupt status definition */
#define MII_INTRSTS         0x1e        /* Interrupt Indicators   */
#define INTR_LINKCHG		0x0800
#define INTR_DUPLEXCHG      0x2000
#define INTR_SPEEDCHG		0x4000
#define INTR_ANERR			0x8000
#define INTR_MSK			0xE800

/* in interrupt context */
static int yt8512c_link_change(void)
{
    uint32_t intr;

    phy_set_page(0);
    phy_reg_read(0x1e);
    intr = phy_reg.val_read & INTR_MSK;

#if 0
    if (intr & INTR_DUPLEXCHG)
        ithPrintf("phy: duplex change! \n");
    if (intr & INTR_SPEEDCHG)
        ithPrintf("phy: speed change! \n");
#endif
    if (intr & INTR_ANERR)
        ithPrintf("phy: ANERR! \n");
    if (intr & INTR_LINKCHG) {
        ithPrintf("phy: link change! \n");
        return 1;
    }

    return 0;
}

static int yt8512c_read_mode(int* speed, int* duplex)
{
    uint16_t _speed, _duplex, link;

    phy_set_page(0);
    phy_reg_read(0x11);

    _speed = (phy_reg.val_read & 0xC000) >> 14;
    _duplex = (phy_reg.val_read & 0x2000) >> 13;
    link = phy_reg.val_read & 0x0400;

    if (_duplex)
        (*duplex) = DUPLEX_FULL;
    else
        (*duplex) = DUPLEX_HALF;

    switch (_speed) {
    default:
    case 0:
        (*speed) = SPEED_10;
        break;
    case 1:
        (*speed) = SPEED_100;
        break;
    case 2:
        (*speed) = SPEED_1000;
        break;
    }

    return link ? 0 : 1; // 0 means link up
}

static uint32_t yt8512c_get_link_status(void)
{
    uint16_t link;

    phy_set_page(0);
    phy_reg_read(0x11);
    link = phy_reg.val_read & 0x0400;

    return link ? 1 : 0;
}

/**
* Check interrupt status for link change. 
* Call from mac driver's internal ISR for phy's interrupt.
*/
int(*itpPhyLinkChange)(void) = yt8512c_link_change;
/**
* Replace mac driver's ISR for phy's interrupt. 
*/
ITHGpioIntrHandler itpPhylinkIsr = NULL;
/**
* Returns 0 if the device reports link status up/ok 
*/
int(*itpPhyReadMode)(int* speed, int* duplex) = yt8512c_read_mode;
/**
* Get link status.
*/
uint32_t(*itpPhyLinkStatus)(void) = yt8512c_get_link_status;


