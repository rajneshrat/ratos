/* This file contain the register map of RTL8139 and what the bits of
   the register stand for.
   The data is copied form data sheet of "Realtek Single Chip Fast Ethernet
   Controller" ,date 1997.4.40.
*/
#define BIT0    0x1
#define BIT1    0x2
#define BIT2    0x4
#define BIT3    0x8
#define BIT4    0x10
#define BIT5    0x20
#define BIT6    0x40
#define BIT7    0x80
#define BIT8    0x100
#define BIT9    0x200
#define BIT10   0x400
#define BIT11   0x800
#define BIT12   0x1000
#define BIT13   0x2000
#define BIT14   0x4000
#define BIT15   0x8000
#define BIT16   0x10000
#define BIT17   0x20000
#define BIT18   0x40000
#define BIT19   0x80000
#define BIT20   0x100000
#define BIT21   0x200000
#define BIT22   0x400000
#define BIT23   0x800000
#define BIT24   0x1000000
#define BIT25   0x2000000
#define BIT26   0x4000000
#define BIT27   0x8000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000
//5.Register Descriptions
#define IDR0    0x0
#define TSD0    0x10
#define TSAD0   0x20
#define RBSTART 0x30
#define CR      0x37
#define CAPR    0x38
#define CBA     0x3a
#define IMR     0x3c
#define ISR     0x3e
#define TCR     0x40
#define RCR     0x44
#define CR9346  0x50
#define CONFIG0 0x51
#define CONFIG1 0x52
#define TSAD    0x60
#define CSCR    0x74
#define PARA78  0x78
#define PARA7c  0x7c
//5.1 	Transmit Configuration Register 	(Offset 0040-0043h,R/W)
#define TCR_IFG1        BIT25
#define TCR_IFG0        BIT24
#define TCR_LBK1        BIT18
#define TCR_LBK0        BIT17
#define TCR_CRC         BIT16
#define TCR_MXDMA2      BIT10
#define TCR_MXDMA1      BIT9
#define TCR_MXDMA0      BIT8
#define TCR_CLRABT      BIT0
//5.2 	Transmit Status of Descriptor0-3	(Offset 0010-001fh,R/W)
#define TSD_CRSL        BIT31
#define TSD_TABT        BIT30
#define TSD_OWC         BIT29
#define TSD_CDH         BIT28
#define TSD_NCC         BIT27+BIT26+BIT25+BIT24
#define TSD_EARTH       BIT21+BIT20+BIT19+BIT18+BIT17+BIT16
#define TSD_TOK         BIT15
#define TSD_TUN         BIT14
#define TSD_OWN         BIT13
#define TSD_SIZE        0x1fff
//5.3 	Interrupt Status(mask) Register        	(Offset 003e-003fh,R/W)
#define ISR_SERR	BIT15
#define ISR_FIFOOVW     BIT6
#define ISR_PUN         BIT5
#define ISR_RXOVW       BIT4
#define ISR_TER         BIT3
#define ISR_TOK         BIT2
#define ISR_RER         BIT1
#define ISR_ROK         BIT0
#define R39_INTERRUPT_MASK      0x7f
//5.4   Transmit Status of AllDescriptor        (Offset 0060-0062h,R/W)
#define TSAD_TOK3       BIT15
#define TSAD_TOK2       BIT14
#define TSAD_TOK1       BIT13
#define TSAD_TOK0       BIT12
#define TSAD_TUN3       BIT11
#define TSAD_TUN2       BIT10
#define TSAD_TUN1       BIT9
#define TSAD_TUN0       BIT8
#define TSAD_TABT3      BIT7
#define TSAD_TABT2      BIT6
#define TSAD_TABT1      BIT5
#define TSAD_TABT0      BIT4
#define TSAD_OWN3       BIT3
#define TSAD_OWN2       BIT2
#define TSAD_OWN1       BIT1
#define TSAD_OWN0       BIT0

//5.5   Media Status Register                   (Offset 0058	  ,R/W)
//5.6   Receive Configuration Register          (Offset 0044-0047h,R/W)
#define RCR_ERTH3       BIT27
#define RCR_ERTH2       BIT26
#define RCR_ERTH1       BIT25
#define RCR_ERTH0       BIT24
#define RCR_RXFTH2      BIT15
#define RCR_RXFTH1      BIT14
#define RCR_RXFTH0      BIT13
#define RCR_RBLEN1      BIT12
#define RCR_RBLEN0      BIT11
#define RCR_MXDMA2      BIT10
#define RCR_MXDMA1      BIT9
#define RCR_MXDMA0      BIT8
#define RCR_WRAP        BIT7
#define RCR_AER         BIT5
#define RCR_AR          BIT4
#define RCR_AB          BIT3
#define RCR_AM          BIT2
#define RCR_APM         BIT1
#define RCR_AAP         BIT0
//5.7   Multiple Interrupt Select Register      (Offset 005c-005dh,R/W)
//5.8   Receive Status in Rx Packet header---this is in Rx buffer
#define RxStutus_MAR    BIT15
#define RxStatus_PAM    BIT14
#define RxStatus_BAR    BIT13
#define RxStatus_ISE    BIT5
#define RxStatus_RUNT   BIT4
#define RxStatus_LONG   BIT3
#define RxStatus_CRC    BIT2
#define RxStatus_FAE    BIT1
#define RxStatus_ROK    BIT0

//5.9   ERSR:Early Rx Status Register           (Offset 0036h     ,R)
//5.10  Command Register                        (Offset 0037h     ,R/W)
#define CR_RST          BIT4
#define CR_RE           BIT3
#define CR_TE           BIT2
#define CR_BUFE         BIT0
//5.11  9346CR:9346 Command Register            (Offset 0050h     ,R/W)

//#define CR9346_EEM1     BIT7
//#define CR9346_EEM0     BIT6
#define CR9346_NORMAL   0x0
#define CR9346_AutoLoad BIT6
#define CR9346_PROGRAM  BIT7
#define CR9346_ConfigEn BIT7+BIT6
#define CR9346_EECS     BIT3
#define CR9346_EESK     BIT2
#define CR9346_EEDI     BIT1
#define CR9346_EEDO     BIT0
#define CR9346_ENB      BIT7+BIT3
//EERPOM Command

#define EEPROM_READ     BIT7+BIT8
#define EEPROM_WRITE    BIT6+BIT8
#define EEPROM_ERASE    BIT6+BIT7+BIT8
#define EEPROM_EnWRITE  BIT8+BIT5+BIT4
#define EEPROM_DisWRITE BIT8
//5.12  CONFIG0:Configuration Register 0        (Offset 0051h     ,R/W)
//5.13  CONFIG1:Configuration Register 1        (Offset 0052h     ,R/W)
#define CONFIG1_LEDS1   BIT7
#define CONFIG1_LEDS0   BIT6
#define CONFIG1_DVRLOAD BIT5
#define CONFIG1_MEMMAP  BIT3
#define CONFIG1_IOMAP   BIT2
#define CONFIG1_SLEEP   BIT1
#define CONFIG1_PWRDN   BIT0
//5.14  PCI Revision ID                         (Offset 005eh     ,R)
//5.15


//5.25  CS COnfiguration Register               (Offset 0074h-0075h,R/W)
#define CSCR_Testfun    BIT15
#define CSCR_LineCase   BIT14+BIT13+BIT12
#define CSCR_LinkChange BIT11
#define CSCR_LinkOK     BIT10
#define CSCR_LD         BIT9
#define CSCR_HEARTBEAT  BIT8
#define CSCR_JBEN       BIT7
#define CSCR_F_LINK_100 BIT6
#define CSCR_F_Connect  BIT5
#define CSCR_Con_status BIT3
#define CSCR_Con_sts_en BIT2
#define CSCR_PASS_SCR   BIT0


