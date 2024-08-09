#include "hda.h"
#include "memory.h"
#include "pic.h"
#include "interrupt.h"

extern uint16_t hda_vendorId;
extern uint16_t hda_deviceId;
extern uint32_t hda_memory_bar;
extern uint8_t hda_irq;
static uint8_t iss; // num of in streams
static uint8_t oss; // num of out streams
static uint8_t bss; // num of pidirecion streams
static uint32_t x;
static uint32_t y;
static uint32_t z;
static uint8_t in_strm_pay; // input stream payload
static uint8_t out_strm_pay; // output stream payloa
static uint16_t corb_sz; // corp size
static uint16_t rirb_sz; // rirp size
static uint16_t codec_id;
static hda_corb_entry_t* corb;
static hda_rirb_entry_t* rirb;

#define wait(x) for (int i = 0; i < x; i++)

#pragma region RW

void hda_write_byte(uint32_t reg, uint8_t value)
{
    *((volatile uint8_t *)(hda_memory_bar + reg)) = value;
}

uint8_t hda_read_byte(uint32_t reg)
{
    return *((volatile uint8_t *)(hda_memory_bar + reg));
}

void hda_write_word(uint32_t reg, uint16_t value)
{
    *((volatile uint16_t *)(hda_memory_bar + reg)) = value;
}

uint16_t hda_read_word(uint32_t reg)
{
    return *((volatile uint16_t *)(hda_memory_bar + reg));
}

void hda_write_dword(uint32_t reg, uint32_t value)
{
    *((volatile uint32_t *)(hda_memory_bar + reg)) = value;
}

uint32_t hda_read_dword(uint32_t reg)
{
    return *((volatile uint32_t *)(hda_memory_bar + reg));
}

#pragma endregion

static void hda_get_params(){
    // Num of ISS , OSS, BSS
    uint16_t gcap = hda_read_word(HDA_GCAP);
    iss = (gcap >> 8) & 0xF;
    oss = (gcap >> 12) & 0xF;
    bss = (gcap >> 3) & 0x1F;
    x = 0x80 + iss * 0x20;
    y = x + oss * 0x20;
    z = y + bss * 0x20;

    // Input & Output Stream payload;
    in_strm_pay = hda_read_word(HDA_INSTRMPAY);
    if(!in_strm_pay) in_strm_pay = hda_read_word(HDA_INPAY);
    out_strm_pay = hda_read_word(HDA_OUTSTRMPAY);
    if(!out_strm_pay) out_strm_pay = hda_read_word(HDA_OUTPAY);

    // CORB & RIRB Size
    uint8_t corp_sz_cap = hda_read_byte(HDA_CORBSIZE) >> 4;
    uint8_t rirp_sz_cap = hda_read_byte(HDA_RIRBSIZE) >> 4;
    for (uint8_t i = 0; i < 3; i++)
    {
        if(corp_sz_cap & (1 << i)) corb_sz = i;
        if(rirp_sz_cap & (1 << i)) rirb_sz = i;
    }
    
    
}

static void hda_init_corb_rirb(){
    hda_write_byte(HDA_CORBSIZE, (hda_read_byte(HDA_CORBSIZE) & 0xF0 )| corb_sz); // set corb size
    hda_write_byte(HDA_RIRBSIZE, (hda_read_byte(HDA_RIRBSIZE) & 0xF0 )| rirb_sz); // set rirb size

    uint16_t sz[] = {2, 16, 256};
    corb_sz = sz[corb_sz];
    rirb_sz = sz[rirb_sz];
    corb = (hda_corb_entry_t*) alloc_align(corb_sz*sizeof(hda_corb_entry_t), 128);
    rirb = (hda_rirb_entry_t*) alloc_align(rirb_sz*sizeof(hda_rirb_entry_t), 128);

    hda_write_dword(HDA_CORBADDR, (uint32_t)corb); // write corb address
    hda_write_dword(HDA_RIRBADDR, (uint32_t)rirb); // write rib address

    hda_write_word(HDA_CORBWP, 0); // reset corb write pointer
    hda_write_word(HDA_CORBRP, 1 << 15); // reset corb read pointer
    hda_write_word(HDA_RIRBWP, 1 << 15); // reset rirb write pointer

    hda_write_byte(HDA_CORBCTL, 0x3); // enable corb
    hda_write_byte(HDA_RIRBCTL, 0x3); // enable rirb
}

hda_rirb_entry_t hda_send_corb_verb(hda_corb_entry_t cmd){
    uint16_t corbwp = hda_read_word(HDA_CORBWP) & 0xFF;
    // uint16_t corbrp = hda_read_word(HDA_CORBRP) & 0xFF;
    uint16_t rirbrp = hda_read_word(HDA_RIRBWP) & 0xFF;

    corbwp = (corbwp + 1) % corb_sz;
    corb[corbwp] = cmd;
    hda_write_word(HDA_CORBWP, corbwp);

    do{
    while (rirbrp == (hda_read_word(HDA_RIRBWP) & 0xFF)) wait(1000);
    rirbrp = (rirbrp + 1) % rirb_sz;
    }while(rirb[rirbrp].solicited);

    return rirb[rirbrp];
}

uint8_t hda_send_imm_cmd(uint8_t codes_addr, uint8_t node_id, uint32_t verb, uint32_t* response){
    uint32_t cmd = (codes_addr << 28) | (node_id << 20) | verb;
    hda_write_dword(HDA_ICW, cmd);
    hda_write_word(HDA_ICS, 0x1);

    int i = 0;
    while(hda_read_word(HDA_ICS) != 0x2){
        wait(1000);
        i ++;
        if(i >= 5){
            return 0;
        }
    }

    *response = hda_read_dword(HDA_ICR);
    return 1;
}

uint8_t hda_get_node_id(uint8_t codes_addr, uint8_t type, uint8_t* id){
    uint32_t res;
    if(hda_send_imm_cmd(codes_addr, 0, 0xF0004, &res)){
        uint8_t nnodes = res & 0xFF;
        uint8_t node0 = (res >> 16) & 0xFF;
        for (uint8_t i = 0; i < nnodes; i++)
        {
            if(hda_send_imm_cmd(codes_addr, node0 + i, 0xF0004, &res)){
                uint8_t nwidgets = res & 0xFF;
                uint8_t widget0 = (res >> 16) & 0xFF;
                for (uint8_t j = 0; j < nwidgets; j++)
                {
                    if(hda_send_imm_cmd(codes_addr, widget0 + j, 0xF0009, &res)){
                        uint8_t t = (res >> 20) & 0xF;
                        if(t == type){
                            *id = widget0 + j;
                            return 1;
                        }
                    }
                }
            
            }
        }
        
    }
    return 0;
}

void hda_init_bdl(){
    #define N_BDL 2
    #define BUFFER_LN 0x1000
    #define SD_FMT 0x11

    uint32_t cbl = BUFFER_LN * N_BDL;

    for (uint8_t i = 0; i < iss; i++)
    {
        hda_bdl_entry_t* bdl = (hda_bdl_entry_t*)alloc_align(N_BDL*sizeof(hda_bdl_entry_t), 128);
        for (uint8_t i = 0; i < N_BDL; i++)
        {
            bdl[i].address = (uint32_t) alloc_align(BUFFER_LN, 128);
            bdl[i].length = BUFFER_LN;
            bdl[i].ioc = 1;
        }
        hda_write_dword(HDA_ISDnBDPL(i), (uint32_t)bdl);
        hda_write_word(HDA_ISDnLVI(i), N_BDL-1);
        hda_write_dword(HDA_ISDnCBL(i), cbl);
        hda_write_word(HDA_ISDnFMT(i), SD_FMT);

        hda_write_dword(HDA_ISDnCTL(i), (i+1) << 20);
    }
    

    for (uint8_t i = 0; i < oss; i++)
    {
        hda_bdl_entry_t* bdl = (hda_bdl_entry_t*)alloc_align(N_BDL*sizeof(hda_bdl_entry_t), 128);
        for (uint8_t i = 0; i < N_BDL; i++)
        {
            bdl[i].address = (uint32_t)alloc_align(BUFFER_LN, 128);
            bdl[i].length = BUFFER_LN;
            bdl[i].ioc = 1;
        }
        hda_write_dword(HDA_OSDnBDPL(i), (uint32_t)bdl);
        hda_write_word(HDA_OSDnLVI(i), N_BDL-1);
        hda_write_dword(HDA_OSDnCBL(i), cbl);
        hda_write_word(HDA_OSDnFMT(i), SD_FMT);  

        hda_write_dword(HDA_OSDnCTL(i), (i+1) << 20);
    }
    
}

uint8_t hda_run_stream(uint8_t codes_addr, uint8_t stream_id, uint8_t type){
    uint8_t node_id;
    if(hda_get_node_id(codes_addr, type, &node_id)){
        uint32_t res;
        if(hda_send_imm_cmd(codes_addr, node_id, 0x70600 | stream_id, &res)){
            if(type){
                hda_write_word(HDA_ISDnCTL(stream_id-1), 0x1E);
            }else{
                hda_write_word(HDA_OSDnCTL(stream_id-1), 0x1E);
            }
            return 1;
        }
    }
    return 0;
}

void hda_handler(){
    pic_sendEOI(hda_irq);
    asm("sti");
}

void hda_init()
{
    idt_set_entry(PIC_M_OFFSET + hda_irq, (uint32_t)hda_handler);
    irq_clear_mask(hda_irq);

    // reset
    hda_write_dword(HDA_GCTL, HDA_GCTL_CRST);
    do{
        wait(1000);
    } while (!(hda_read_dword(HDA_GCTL) & HDA_GCTL_CRST));

    codec_id = hda_read_word(HDA_STATESTS);
    
    // wake enabled
    hda_write_word(HDA_WAKEEN, 0xFFFF);

    // interrupts
    hda_write_dword(HDA_INTCTL, 0x3FFFFFFF);

    hda_get_params();

    hda_init_corb_rirb();

    hda_init_bdl();
}