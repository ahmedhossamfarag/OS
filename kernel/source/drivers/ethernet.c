#include "ethernet.h"
#include "low_level.h"
#include "memory.h"
#include "libc.h"

extern uint16_t ethernet_vendorId;
extern uint16_t ethernet_deviseId;
extern uint32_t ethernet_memory_bar;
extern uint8_t ethernet_irq;
e1000_tx_desc_t *tx_desc;
e1000_rx_desc_t *rx_desc;

void ethernet_write_reg(uint32_t reg, uint32_t value)
{
    *((volatile uint32_t *)(ethernet_memory_bar + reg)) = value;
}

uint32_t ethernet_read_reg(uint32_t reg)
{
    return *((volatile uint32_t *)(ethernet_memory_bar + reg));
}

void ethernet_write_mac_address(uint64_t mac)
{
    ethernet_write_reg(RAL0, (uint32_t)mac);
    ethernet_write_reg(RAH0, (uint32_t)(mac >> 32) | (1 << 31));
}

void ethernet_rx_init(uint64_t receive_desc_address, uint32_t receive_desc_length)
{
    ethernet_write_reg(RDBAL, receive_desc_address);
    ethernet_write_reg(RDBAH, receive_desc_address >> 32);
    ethernet_write_reg(RDLEN, receive_desc_length);
    ethernet_write_reg(RDH, 0);
    ethernet_write_reg(RDT, receive_desc_length - 1);
    ethernet_write_reg(RCTL, RCTL_EN);
}

void ethernet_tx_init(uint64_t transmit_desc_address, uint32_t transmit_desc_length)
{
    ethernet_write_reg(TDBAL, transmit_desc_address);
    ethernet_write_reg(TDBAH, transmit_desc_address >> 32);
    ethernet_write_reg(TDLEN, transmit_desc_length);
    ethernet_write_reg(TDH, 0);
    ethernet_write_reg(TDT, 0);
    ethernet_write_reg(TCTL, TCTL_EN);
}

void ethernet_enable_interrupts()
{
    ethernet_write_reg(IMS, IMS_RXT | IMS_TXDW);
}

void ethernet_start()
{
    ethernet_write_reg(CTRL, CTRL_SLU | CTRL_ASDE);
}

void ethernet_send_packet(void *packet, uint32_t length)
{
    tx_desc->buffer_addr = (uint32_t)packet;
    tx_desc->length = length;
    tx_desc->cmd = TXD_CMD_EOP | TXD_CMD_RS;
    tx_desc->status = 0;

    ethernet_write_reg(TDH, 0);
    ethernet_write_reg(TDT, 0);
}

void ethernet_init()
{
    if(!ethernet_memory_bar){
        return;
    }

    ethernet_write_reg(CTRL, CTRL_RESET);
    while (ethernet_read_reg(CTRL) & CTRL_RESET)
        ;
    uint64_t mac_addr = 0x123456789ABC;
    ethernet_write_mac_address(mac_addr);
    tx_desc = (e1000_tx_desc_t *)alloc(sizeof(e1000_tx_desc_t) * N_DESC);
    rx_desc = (e1000_rx_desc_t *)alloc(sizeof(e1000_rx_desc_t) * N_DESC);
    ethernet_tx_init((uint32_t)tx_desc, sizeof(e1000_tx_desc_t) * N_DESC);
    ethernet_rx_init((uint32_t)rx_desc, sizeof(e1000_rx_desc_t) * N_DESC);
    ethernet_enable_interrupts();
    ethernet_start();
}
