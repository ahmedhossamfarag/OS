#include "ethernet.h"
#include "low_level.h"
#include "memory.h"
#include "libc.h"
#include "vga_print.h"
#include "strlib.h"

extern uint16_t ethernet_vendorId;
extern uint16_t ethernet_deviseId;
extern uint64_t ethernet_memory_bar;
extern uint8_t ethernet_irq;
e1000_tx_desc_t *tx_desc;
e1000_rx_desc_t *rx_desc;
uint32_t tx_next_desc;

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
    ethernet_write_reg(RDT, receive_desc_length - 1);
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
    tx_desc = (e1000_tx_desc_t *)alloc_align(sizeof(e1000_tx_desc_t) * N_DESC, 128);

    mem_set((char *)tx_desc, 0, sizeof(e1000_tx_desc_t) * N_DESC);

    for(int i = 0; i < N_DESC; i++){
        tx_desc[i].status = TXD_STAT_DD;
    }

    ethernet_tx_init((uint64_t)tx_desc, sizeof(e1000_tx_desc_t) * N_DESC);
    
    tx_next_desc = 0;
}

void ethernet_alloc_rx_desc(){
    rx_desc = (e1000_rx_desc_t *)alloc_align(sizeof(e1000_rx_desc_t) * N_DESC, 128);
    mem_set((char *)rx_desc, 0, sizeof(e1000_rx_desc_t) * N_DESC);

    char *rx_buffer = (char *)alloc_align(RX_BUFFER_SIZE * N_DESC, 128);

    for(int i = 0; i < N_DESC; i++){
        rx_desc[i].buffer_addr = (uint64_t)rx_buffer + i * RX_BUFFER_SIZE;
        rx_desc[i].length = RX_BUFFER_SIZE;
        rx_desc[i].status = 0;
    }

    ethernet_rx_init((uint64_t)rx_desc, sizeof(e1000_rx_desc_t) * N_DESC);
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

    print("Sending packet\n");
    print("Status: ");
    println(sxint(desc->status));
    print("TCTL: ");
    println(sxint(ethernet_read_reg(TCTL)));

    if (!(desc->status & TXD_STAT_DD))
        return 0;

    desc->buffer_addr = (uint64_t)packet;
    desc->length = length;
    desc->cmd = TXD_CMD_EOP | TXD_CMD_IFCS | TXD_CMD_RS;
    desc->status = 0;
    desc->special = 0;
    desc->css = 0;
    desc->cso = 0;

    uint32_t next_desc = (tx_next_desc + 1) % N_DESC;
    ethernet_write_reg(TDT, next_desc);

    uint32_t timeout = 1000000;
    while(timeout--){
        if(desc->status & TXD_STAT_DD){
            tx_next_desc = next_desc;
            return 1;
        }
    }
    print("Timeout\n");
    return 0;
}

void ethernet_test(){
    #define RCTL_LBM_MASK (3u << 6)

    uint32_t rctl = ethernet_read_reg(RCTL);
    rctl &= ~RCTL_LBM_MASK;
    rctl |= RCTL_LBM_MAC | RCTL_EN | RCTL_BAM | RCTL_SECRC;
    ethernet_write_reg(RCTL, rctl);

    uint8_t* test_frame = (uint8_t*) alloc_align(60, 128);

    for (int i = 0; i < 6; i++)
        test_frame[i] = 0xFF;          // Destination: broadcast

    uint64_t mac = ethernet_read_mac();

    mem_copy((char*)&test_frame[6], (char*) &mac, 6);    // Your NIC's source MAC

    test_frame[12] = 0x88;             // Experimental EtherType
    test_frame[13] = 0xB5;

    mem_copy((char*)&test_frame[14], (char*) "Hello, world!", 13);  // Payload

    uint8_t result = ethernet_send_packet(test_frame, 60);
    print("Send result: ");
    println(sxint(result));
}

void ethernet_init()
{
    if(!ethernet_memory_bar){
        return;
    }

    print("Ethernet init\n");
    print("Vendor ID: ");
    println(sxint(ethernet_vendorId));
    print("Device ID: ");
    println(sxint(ethernet_deviseId));
    print("Memory BAR: ");
    println(sxint(ethernet_memory_bar));
    print("IRQ: ");
    println(sxint(ethernet_irq));

    uint32_t ctrl = ethernet_read_reg(CTRL);
    print("CTRL: ");
    println(sxint(ctrl));

    uint32_t status = ethernet_read_reg(STATUS);
    print("STATUS: ");
    println(sxint(status));

    uint64_t mac = ethernet_read_mac();
    print("MAC: ");
    println(sxint(mac));

    ethernet_write_reg(CTRL, CTRL_RESET);
    uint32_t timeout = 1000;
    while ((ethernet_read_reg(CTRL) & CTRL_RESET) && timeout--);

    print("Resetted");

    ethernet_alloc_tx_desc();
    ethernet_alloc_rx_desc();
    ethernet_enable_interrupts();
    ethernet_start();

    ethernet_test();
}

uint8_t ethernet_available(){
    return ethernet_memory_bar != 0;
}

uint64_t ethernet_get_phys_memo_base(){
    return ethernet_memory_bar;
}