#include "ethernet.h"
#include "low_level.h"
#include "memory.h"
#include "libc.h"

extern uint16_t ethernet_vendorId;
extern uint16_t ethernet_deviseId;
extern uint64_t ethernet_memory_bar;
extern uint8_t ethernet_irq;
e1000_tx_desc_t *tx_desc;
e1000_rx_desc_t *rx_desc;
uint32_t tx_next_desc;
uint32_t rx_next_desc;

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

uint64_t ethernet_read_mac()
{
    uint32_t ral = ethernet_read_reg(RAL0);
    uint32_t rah = ethernet_read_reg(RAH0);

    uint64_t mac = ral | ((uint64_t)rah << 32);
    return mac;
}

void ethernet_rx_init(uint64_t receive_desc_address, uint32_t receive_desc_length)
{
    // Disable receive
    ethernet_write_reg(RCTL, 0);
    ethernet_write_reg(RDBAL, receive_desc_address & 0xFFFFFFFF);
    ethernet_write_reg(RDBAH, receive_desc_address >> 32);
    ethernet_write_reg(RDLEN, receive_desc_length);
    ethernet_write_reg(RDH, 0);
    ethernet_write_reg(RDT, E1000_N_DESC - 1);
    ethernet_write_reg(RCTL, RCTL_EN | RCTL_BAM | RCTL_BSIZE_2048 | RCTL_SECRC);
}

void ethernet_tx_init(uint64_t transmit_desc_address, uint32_t transmit_desc_length)
{
    ethernet_write_reg(TDBAL, transmit_desc_address & 0xFFFFFFFF);
    ethernet_write_reg(TDBAH, transmit_desc_address >> 32);
    ethernet_write_reg(TDLEN, transmit_desc_length);
    ethernet_write_reg(TDH, 0);
    ethernet_write_reg(TDT, 0);
    ethernet_write_reg(TCTL, TCTL_EN | TCTL_PSP | TCTL_CT(15) | TCTL_COLD(64));
}

void ethernet_alloc_tx_desc(){
    tx_desc = (e1000_tx_desc_t *)alloc_align(sizeof(e1000_tx_desc_t) * E1000_N_DESC, 128);
    mem_set((char *)tx_desc, 0, sizeof(e1000_tx_desc_t) * E1000_N_DESC);

    char *tx_buffer = (char *)alloc_align(RX_BUFFER_SIZE * E1000_N_DESC, 128);

    for(int i = 0; i < E1000_N_DESC; i++){
        tx_desc[i].buffer_addr = (uint64_t)tx_buffer + i * RX_BUFFER_SIZE;
        tx_desc[i].length = RX_BUFFER_SIZE;
        tx_desc[i].status = TXD_STAT_DD;
    }

    ethernet_tx_init((uint64_t)tx_desc, sizeof(e1000_tx_desc_t) * E1000_N_DESC);
    
    tx_next_desc = 0;
}

void ethernet_alloc_rx_desc(){
    rx_desc = (e1000_rx_desc_t *)alloc_align(sizeof(e1000_rx_desc_t) * E1000_N_DESC, 128);
    mem_set((char *)rx_desc, 0, sizeof(e1000_rx_desc_t) * E1000_N_DESC);

    char *rx_buffer = (char *)alloc_align(RX_BUFFER_SIZE * E1000_N_DESC, 128);

    for(int i = 0; i < E1000_N_DESC; i++){
        rx_desc[i].buffer_addr = (uint64_t)rx_buffer + i * RX_BUFFER_SIZE;
        rx_desc[i].length = RX_BUFFER_SIZE;
        rx_desc[i].status = 0;
    }

    ethernet_rx_init((uint64_t)rx_desc, sizeof(e1000_rx_desc_t) * E1000_N_DESC);

    rx_next_desc = 0;
}

void ethernet_enable_interrupts()
{
    ethernet_write_reg(IMS, IMS_RXT | IMS_TXDW);
}

void ethernet_start()
{
    ethernet_write_reg(CTRL, CTRL_SLU | CTRL_ASDE);
}

uint8_t ethernet_send_packet(void *packet, uint32_t length)
{
    e1000_tx_desc_t* desc = tx_desc + tx_next_desc;

    if (!(desc->status & TXD_STAT_DD))
        return 0;

    if (length > RX_BUFFER_SIZE){
        return 0;
    }

    mem_copy((char*) packet, (char*) desc->buffer_addr, length);

    desc->length = length;
    desc->cmd = TXD_CMD_EOP | TXD_CMD_IFCS | TXD_CMD_RS;
    desc->status = 0;
    desc->special = 0;
    desc->css = 0;
    desc->cso = 0;

    uint32_t next_desc = (tx_next_desc + 1) % E1000_N_DESC;
    ethernet_write_reg(TDT, next_desc);

    uint32_t timeout = 1000000;
    while(timeout--){
        if(((volatile e1000_tx_desc_t*)desc)->status & TXD_STAT_DD){
            tx_next_desc = next_desc;
            return 1;
        }
    }

    return 0;
}

uint8_t ethernet_receive_packet(void *packet){
    for (unsigned attempt = 0; attempt < 1000000; attempt++) {
        volatile e1000_rx_desc_t *d = &rx_desc[rx_next_desc];
        if (d->status & RXD_STAT_DD) {
            if ((d->status & RXD_STAT_EOP) && d->errors == 0) {
                mem_copy(
                    (char*) rx_desc[rx_next_desc].buffer_addr, 
                    (char*) packet, 
                    rx_desc[rx_next_desc].length
                );
            }

            // Recycle the descriptor after processing its packet.
            d->status = 0;
            d->errors = 0;

            ethernet_write_reg(RDT, rx_next_desc);

            rx_next_desc = (rx_next_desc + 1) % E1000_N_DESC;
            
            return 1;
        }
    }

    return 0;
}

void ethernet_init()
{
    if(!ethernet_memory_bar){
        return;
    }

    ethernet_write_reg(CTRL, CTRL_RESET);
    uint32_t timeout = 1000;
    while ((ethernet_read_reg(CTRL) & CTRL_RESET) && timeout--);

    ethernet_alloc_tx_desc();
    ethernet_alloc_rx_desc();
    ethernet_enable_interrupts();
    ethernet_start();
}

uint8_t ethernet_available(){
    return ethernet_memory_bar != 0;
}

uint64_t ethernet_get_phys_memo_base(){
    return ethernet_memory_bar;
}

uint8_t ethernet_interrupt_handler(){
    uint32_t cause = ethernet_read_reg(ICR);

    if (cause){
        if (cause & E1000_ICR_RXT0) {
            // TODO: Handle RX timeout
        }

        if (cause & E1000_ICR_RXDMT0) {
            // TODO: Handle RX DMA
        }

        if (cause & E1000_ICR_TXDW) {
            // TODO: Handle TX DMA
        }

        if (cause & E1000_ICR_TXQE) {
            // TODO: Handle TX DMA
        }

        if (cause & E1000_ICR_LSC) {
            // TODO: Handle Link Status Change
        }

        if (cause & E1000_ICR_RXO) {
            // TODO: Handle RX Overrun
        }

        if (cause & E1000_ICR_RXSEQ) {
            // TODO: Handle RX Sequence Error
        }

        return 1;
    }

    return 0;
}