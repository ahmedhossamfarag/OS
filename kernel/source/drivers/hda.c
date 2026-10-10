#include "hda.h"
#include "memory.h"
#include "pic.h"
#include "apic.h"
#include "interrupt.h"
#include "libc.h"

extern uint16_t hda_vendorId;
extern uint16_t hda_deviceId;
extern uint64_t hda_memory_bar;
extern uint8_t hda_irq;
static uint8_t iss; // num of in streams
static uint8_t oss; // num of out streams
static uint8_t bss; // num of bidirecion streams
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
static hda_bdl_entry_t** iss_bdls;
static hda_bdl_entry_t** oss_bdls;

static uint8_t dac_node;
static uint8_t adc_node;
static uint8_t out_pin_node;
static uint8_t in_pin_node;

static struct {
    uint8_t stream_id;
    uint8_t type;
    uint8_t* buffer;
    uint64_t cr3;
    uint32_t size;
    uint32_t read_offset; // Output stream
    uint32_t write_offset; // Input stream
    uint8_t is_running;
} hda_args;

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

    hda_write_dword(HDA_CORBADDR, (uint64_t)corb & 0xFFFFFFFF); // write corb address
    hda_write_dword(HDA_RIRBADDR, (uint64_t)rirb & 0xFFFFFFFF); // write rib address

    hda_write_word(HDA_CORBWP, 0); // reset corb write pointer
    hda_write_word(HDA_CORBRP, 1 << 15); // reset corb read pointer
    hda_write_word(HDA_RIRBWP, 1 << 15); // reset rirb write pointer

    hda_write_byte(HDA_CORBCTL, 0x3); // enable corb
    hda_write_byte(HDA_RIRBCTL, 0x3); // enable rirb
}

hda_rirb_entry_t hda_send_corb_verb(hda_corb_entry_t cmd){
    uint16_t corbwp = hda_read_word(HDA_CORBWP) & 0xFF;
    
    corbwp = (corbwp + 1) % corb_sz;
    corb[corbwp] = cmd;
    hda_write_word(HDA_CORBWP, corbwp);

    // Wait until RIRB write pointer moves
    uint16_t initial_rirbwp = hda_read_word(HDA_RIRBWP) & 0xFF;
    uint16_t rirbwp;
    
    int timeout = 100000;
    do {
        rirbwp = hda_read_word(HDA_RIRBWP) & 0xFF;
        if (rirbwp != initial_rirbwp) break;
        wait(10);
        timeout--;
    } while (timeout > 0);

    uint16_t rirbrp = (initial_rirbwp) % rirb_sz;
    return rirb[rirbrp];
}

uint8_t hda_send_imm_cmd(uint8_t codes_addr, uint8_t node_id, uint32_t verb, uint32_t* response){
    uint32_t cmd = (codes_addr << 28) | (node_id << 20) | verb;
    
    // 1. Ensure ICB is clear before sending a new command
    if (hda_read_word(HDA_ICS) & 0x1) {
        // Clear stuck ICB/IRV if necessary
        hda_write_word(HDA_ICS, 0x3); 
    }

    hda_write_dword(HDA_ICW, cmd);
    
    // 2. Set ICB (Bit 0) to trigger execution
    hda_write_word(HDA_ICS, 0x1);

    int i = 0;
    // 3. Wait until ICB (Bit 0) clears to 0 (meaning hardware finished)
    while((hda_read_word(HDA_ICS) & 0x1) != 0){
        wait(1000);
        i++;
        if(i >= 50){ // Increased timeout slightly
            return 0;
        }
    }

    // 4. Check if Response Valid (IRV - Bit 1) is set
    if (!(hda_read_word(HDA_ICS) & 0x2)) {
        return 0; // No valid response
    }

    *response = hda_read_dword(HDA_ICR);
    
    // 5. Clear IRV by writing 1 to it so it's ready for the next command
    hda_write_word(HDA_ICS, 0x2);

    return 1;
}

void hda_init_bdl(){
    uint32_t cbl = BDL_BUFFER_LN * HDA_N_BDL;

    iss_bdls = (hda_bdl_entry_t**)alloc(iss*sizeof(hda_bdl_entry_t*));

    for (uint8_t i = 0; i < iss; i++)
    {
        hda_bdl_entry_t* bdl = (hda_bdl_entry_t*)alloc_align(HDA_N_BDL*sizeof(hda_bdl_entry_t), 128);
        for (uint8_t j = 0; j < HDA_N_BDL; j++)
        {
            bdl[j].address = (uint64_t) alloc_align(BDL_BUFFER_LN, 128) & 0xFFFFFFFF;
            bdl[j].length = BDL_BUFFER_LN;
            bdl[j].ioc = 1;
        }

        iss_bdls[i] = bdl;
        
        hda_write_dword(HDA_ISDnBDPL(i), (uint64_t)bdl & 0xFFFFFFFF);
        hda_write_word(HDA_ISDnLVI(i), HDA_N_BDL-1);
        hda_write_dword(HDA_ISDnCBL(i), cbl);
        hda_write_word(HDA_ISDnFMT(i), SD_FMT);

        hda_write_dword(HDA_ISDnCTL(i), (i+1) << 20);
    }
    

    oss_bdls = (hda_bdl_entry_t**)alloc(oss*sizeof(hda_bdl_entry_t*));

    for (uint8_t i = 0; i < oss; i++)
    {
        hda_bdl_entry_t* bdl = (hda_bdl_entry_t*)alloc_align(HDA_N_BDL*sizeof(hda_bdl_entry_t), 128);
        for (uint8_t j = 0; j < HDA_N_BDL; j++)
        {
            bdl[j].address = (uint64_t)alloc_align(BDL_BUFFER_LN, 128) & 0xFFFFFFFF;
            bdl[j].length = BDL_BUFFER_LN;
            bdl[j].ioc = 1;
        }

        oss_bdls[i] = bdl;

        hda_write_dword(HDA_OSDnBDPL(i), (uint64_t)bdl & 0xFFFFFFFF);
        hda_write_word(HDA_OSDnLVI(i), HDA_N_BDL-1);
        hda_write_dword(HDA_OSDnCBL(i), cbl);
        hda_write_word(HDA_OSDnFMT(i), SD_FMT);  

        hda_write_dword(HDA_OSDnCTL(i), (i+1) << 20);
    }
    
}

uint8_t hda_get_node_connection_entry_id(uint8_t codec_addr, uint8_t node_id, uint8_t entry_indx){
    uint32_t res;
    // Read node connection list length.
    if (hda_send_imm_cmd(codec_addr, node_id, 0xF000E, &res)){

        if(entry_indx >= res) return 0;

        // Read the first connection list entry.
        if (hda_send_imm_cmd(codec_addr, node_id, 0xF0200, &res)){
            return res & 0xFF;
        }
    }

    return 0;
}

uint8_t hda_get_node_connection_entry_index(uint8_t codec_addr, uint8_t node_id, uint8_t entry_id){
    uint32_t res;
    // Read node connection list length.
    if (hda_send_imm_cmd(codec_addr, node_id, 0xF000E, &res)){
        uint8_t list_length = res & 0xFF;

        for (uint8_t i = 0; i < list_length; i++) {
            if(hda_send_imm_cmd(codec_addr, node_id, 0xF0200 | i, &res)){
                if ((res & 0xFF) == entry_id) {
                    return i;
                }
            }
        }
    }

    return -1;
}

uint8_t hda_check_node_capability(uint8_t codec_addr, uint8_t node_id, uint8_t capability){
    uint32_t res;
    if(hda_send_imm_cmd(codec_addr, node_id, 0xF000C, &res)){
        uint8_t node_capability = res & 0xFF;
        if(node_capability & capability){
            return 1;
        }
    }

    return 0;
}

uint8_t hda_get_node_ids(uint8_t codec_addr){
    uint32_t res;
    if(hda_send_imm_cmd(codec_addr, 0, 0xF0004, &res)){
        uint8_t nnodes = res & 0xFF;
        uint8_t node0 = (res >> 16) & 0xFF;
        for (uint8_t i = 0; i < nnodes; i++)
        {
            if(hda_send_imm_cmd(codec_addr, node0 + i, 0xF0004, &res)){
                uint8_t nwidgets = res & 0xFF;
                uint8_t widget0 = (res >> 16) & 0xFF;
                for (uint8_t j = 0; j < nwidgets; j++)
                {
                    if(hda_send_imm_cmd(codec_addr, widget0 + j, 0xF0009, &res)){
                        uint8_t t = (res >> 20) & 0xF;
                        switch (t)
                        {
                        case AC_WID_AUD_OUT:
                            dac_node = widget0 + j;
                            break;
                        case AC_WID_AUD_IN:
                            adc_node = widget0 + j;
                            break;
                        case AC_WID_PIN:
                            if (hda_check_node_capability(codec_addr, widget0 + j, PIN_INPUT_CAP)) {
                                in_pin_node = widget0 + j;
                            } else if (hda_check_node_capability(codec_addr, widget0 + j, PIN_OUTPUT_CAP)) {
                                out_pin_node = widget0 + j;
                            }
                            break;
                        default:
                            break;
                        }
                        if(dac_node && adc_node && out_pin_node && in_pin_node) return 1;
                    }
                }
            
            }
        }
        
    }

    if (!in_pin_node) {in_pin_node = out_pin_node;}

    if (!dac_node || !adc_node || !out_pin_node || !in_pin_node) return 0;

    return 1;
}

uint8_t hda_configure_codec_path(uint8_t codec_addr) {
    uint32_t res;
    uint8_t status;

    // 1. Get DAC, ADC, and Pin Complex node IDs
    if (!dac_node || !adc_node || !out_pin_node || !in_pin_node) return 0;

    // 2. Power up Audio Function Group (Node 0x01) and all relevant widgets to Full On (0x0)
    status = hda_send_imm_cmd(codec_addr, AFG_NODE_ID, 0x70500 | 0x0, &res); // AFG Power
    if (!status) return status;

    status = hda_send_imm_cmd(codec_addr, dac_node, 0x70500 | 0x0, &res); // DAC Power
    if (!status) return status;
    status = hda_send_imm_cmd(codec_addr, adc_node, 0x70500 | 0x0, &res); // ADC Power
    if (!status) return status;
    status = hda_send_imm_cmd(codec_addr, out_pin_node, 0x70500 | 0x0, &res); // OutPin Power
    if (!status) return status;
    status = hda_send_imm_cmd(codec_addr, in_pin_node, 0x70500 | 0x0, &res); // InPin Power
    if (!status) return status;

    // --- OUTPUT PATH CONFIGURATION ---

    // 3. Connect DAC output to the Pin Complex input connection list (Index 0)
    // This assumes the DAC is connected to the first input of the Pin Complex (index 0)
    status = hda_send_imm_cmd(codec_addr, out_pin_node, 0x70100 | 0x0, &res);
    if (!status) return status;

    // 4. Unmute DAC Output Amplifier
    status = hda_send_imm_cmd(codec_addr, dac_node, 0x30000 | 0xB07F, &res); 
    if (!status) return status;

    // 5. Unmute Out Pin Complex Output Amplifier and set Pin Control to Output Enabled
    status = hda_send_imm_cmd(codec_addr, out_pin_node, 0x30000 | 0xB07F, &res); 
    if (!status) return status;
    status = hda_send_imm_cmd(codec_addr, out_pin_node, 0x70700 | 0x40, &res);   // Out enable
    if (!status) return status;

    // --- INPUT PATH CONFIGURATION ---

    // 6. Connect Pin Complex (or mixer/source) to the ADC input connection list (Index 0)
    // Verb 0x70100 sets the connection select for the widget
    // This assumes the ADC is connected to the first input of the Pin Complex (index 0)
    status = hda_send_imm_cmd(codec_addr, adc_node, 0x70100 | 0x0, &res);
    if (!status) return status;

    // 7. Unmute ADC Input Amplifier (Bit 15 = 0 for Input amplifier)
    // Payload format: Bit 15 = 0 (Input), Bit 13 = L Unmute, Bit 12 = R Unmute, Bits 0-6 = Gain
    status = hda_send_imm_cmd(codec_addr, adc_node, 0x30000 | 0x707F, &res); // Unmute ADC input, max gain
    if (!status) return status;

    // 8. Unmute In Pin Set Pin Control for Input (Microphone/Line-In) Enabled (Bit 5 = In) and set VREF if needed
    // Bit 5 = Input Enable, Bits 0-2 = VREF (e.g., 0x2 for 50% VREF for microphones)
    status = hda_send_imm_cmd(codec_addr, in_pin_node,0x30000 | 0xB07F, &res);   
    if (!status) return status;
    status = hda_send_imm_cmd(codec_addr, in_pin_node, 0x70700 | 0x20, &res);   // In enable + VREF 50%
    if (!status) return status;

    return status;
}

uint8_t hda_run_stream(uint8_t codec_addr, uint8_t stream_tag, uint8_t type) {
    // Ensure stream tag is valid (1 to 15)
    if (stream_tag == 0 || stream_tag > 15) return 0;

    if(!type && !dac_node){
        return 0;
    }

    if(type && !adc_node){
        return 0;
    }

    uint8_t node_id = type ? adc_node : dac_node;

    // 1. Tell the Codec which stream tag and channel to listen to
    // Verb 0x70600: bits 4-7 = stream tag, bits 0-3 = channel (channel 0)
    uint32_t codec_cmd = 0x70600 | (stream_tag << 4) | 0x0;
    uint32_t res;
    if (!hda_send_imm_cmd(codec_addr, node_id, codec_cmd, &res)) {
        return 0; // Failed to configure codec converter stream
    }

    // Select stream register offset (Output vs Input)
    // Assuming stream_tag 1 maps to index 0
    uint8_t idx = stream_tag - 1;
    uint32_t ctl_reg = type ? (uint32_t)HDA_ISDnCTL(idx) : (uint32_t)HDA_OSDnCTL(idx);

    // 2. Stream Reset Sequence (Mandatory by HDA Spec)
    // Set SRST (Bit 0 = 1)
    uint32_t val = hda_read_dword(ctl_reg);
    hda_write_dword(ctl_reg, val | 0x1);
    
    // Wait for SRST to set (Bit 0 == 1)
    int timeout = 1000;
    while (!(hda_read_dword(ctl_reg) & 0x1) && timeout--) { wait(10); }

    // Clear SRST (Bit 0 = 0) and set Stream Tag in Control Byte 2 (Bits 20-23)
    val = hda_read_dword(ctl_reg);
    val &= ~0x1;                    // Clear SRST
    val &= ~(0xF << 20);            // Clear old stream tag bits
    val |= (stream_tag << 20);      // Set new stream tag
    hda_write_dword(ctl_reg, val);

    // Wait for SRST to clear (Bit 0 == 0)
    timeout = 1000;
    while ((hda_read_dword(ctl_reg) & 0x1) && timeout--) { wait(10); }

    // 3. Start the Stream: Set RUN (Bit 1) and Interrupt Enables (IOCE, FEIE, DEIE)
    val = hda_read_dword(ctl_reg);
    val |= 0x1E; // Set bits 1, 2, 3, 4 (RUN, IOC, FIFO Error, Descriptor Error)
    hda_write_dword(ctl_reg, val);

    return 1;
}

uint8_t hda_is_output_stream_running(uint8_t stream_id) {
    // stream_id is 1-indexed (e.g., 1, 2, 3...)
    // Convert to 0-indexed for macro lookup: stream_id - 1
    uint8_t index = stream_id - 1;
    
    // Read the Output Stream Control register (OSDnCTL)
    uint32_t ctl_reg = hda_read_dword(HDA_OSDnCTL(index));
    
    // Check if the RUN bit (Bit 1, value 0x2) is set
    if (ctl_reg & 0x2) {
        return 1; // Stream is active/running
    }
    
    return 0; // Stream is stopped
}

uint8_t hda_is_input_stream_running(uint8_t stream_id) {
    // stream_id is 1-indexed (e.g., 1, 2, 3...)
    // Convert to 0-indexed for macro lookup: stream_id - 1
    uint8_t index = stream_id - 1;
    
    // Read the Input Stream Control register (ISDnCTL)
    uint32_t ctl_reg = hda_read_dword(HDA_ISDnCTL(index));
    
    // Check if the RUN bit (Bit 1, value 0x2) is set
    if (ctl_reg & 0x2) {
        return 1; // Stream is active/running
    }
    
    return 0; // Stream is stopped
}

uint8_t hda_stop_stream(uint8_t stream_id, uint8_t type) {
    uint8_t idx = stream_id - 1;
    uint32_t ctl_reg = type ? (uint32_t)HDA_ISDnCTL(idx) : (uint32_t)HDA_OSDnCTL(idx);

    // 1. Read current control register value
    uint32_t val = hda_read_dword(ctl_reg);

    // 2. Clear the RUN bit (Bit 1)
    val &= ~0x2; 
    hda_write_dword(ctl_reg, val);

    // 3. Wait for hardware confirmation (RUN bit drops to 0)
    int timeout = 1000; 
    while ((hda_read_dword(ctl_reg) & 0x2) && timeout > 0) {
        wait(10); 
        timeout--;
    }

    if (timeout == 0) {
        return 0; // Timeout: Failed to stop cleanly
    }

    return 1; // Stream stopped successfully
}

uint32_t get_current_buffer_index(uint8_t stream_idx, uint8_t is_input) {
    // Read current byte offset processed by DMA
    uint32_t lpib = is_input ? hda_read_dword(HDA_ISDnLPIB(stream_idx)) 
                             : hda_read_dword(HDA_OSDnLPIB(stream_idx));

    // Determine which BDL entry this offset falls into
    // BDL_BUFFER_LN is the size of each individual buffer descriptor block
    uint32_t bdl_index = lpib / BDL_BUFFER_LN;
    return bdl_index % HDA_N_BDL;
}

uint8_t hda_play_sound(void* buffer, uint32_t size, uint64_t cr3) {
    if(!buffer || size < BDL_BUFFER_LN) return 0;
    if (hda_args.is_running) return 0;

    hda_args.stream_id = 1;
    hda_args.type = 0;
    hda_args.buffer = buffer;
    hda_args.cr3 = cr3;
    hda_args.size = size - (size % BDL_BUFFER_LN);

    void* bdl_buffer = (void*) oss_bdls[0][0].address;

    mem_copy((char*) buffer, (char*) bdl_buffer, BDL_BUFFER_LN);

    hda_args.read_offset = 0;
    hda_args.write_offset = 0;

    if(hda_run_stream(codec_id, hda_args.stream_id, hda_args.type)){
        hda_args.is_running = 1;
        return 1;
    }

    return 0;
}

uint8_t hda_record_sound(void* buffer, uint32_t size, uint64_t cr3) {
    if(!buffer || size < BDL_BUFFER_LN) return 0;
    if (hda_args.is_running) return 0;

    hda_args.stream_id = 1;
    hda_args.type = 1;
    hda_args.buffer = buffer;
    hda_args.cr3 = cr3;
    hda_args.size = size - (size % BDL_BUFFER_LN);

    hda_args.read_offset = 0;
    hda_args.write_offset = 0;

    if(hda_run_stream(codec_id, hda_args.stream_id, hda_args.type)){
        hda_args.is_running = 1;
        return 1;
    }

    return 0;
}

void handle_read_complete(){
    if(!hda_args.is_running) return;

    if (hda_args.read_offset + BDL_BUFFER_LN >= hda_args.size) {
        hda_stop_stream(hda_args.stream_id, hda_args.type);
        hda_args.is_running = 0;
        return;
    }

    uint64_t current_cr3;
    asm volatile("mov %%cr3, %0" : "=r"(current_cr3));
    asm volatile("mov %0, %%cr3" : : "r"(hda_args.cr3));

    hda_args.read_offset += BDL_BUFFER_LN;
    hda_args.buffer += BDL_BUFFER_LN;

    void* bdl_buffer = (void*) oss_bdls[0][0].address;
    mem_copy((char*) hda_args.buffer, (char*) bdl_buffer, BDL_BUFFER_LN);

    asm volatile("mov %0, %%cr3" : : "r"(current_cr3));
}

void handle_write_complete(){
    if(!hda_args.is_running) return;

    uint64_t current_cr3;
    asm volatile("mov %%cr3, %0" : "=r"(current_cr3));
    asm volatile("mov %0, %%cr3" : : "r"(hda_args.cr3));

    void* bdl_buffer = (void*) iss_bdls[0][0].address;
    mem_copy((char*) bdl_buffer, (char*) hda_args.buffer, BDL_BUFFER_LN);

    asm volatile("mov %0, %%cr3" : : "r"(current_cr3));

    if (hda_args.write_offset + BDL_BUFFER_LN >= hda_args.size) {
        hda_stop_stream(hda_args.stream_id, hda_args.type);
        hda_args.is_running = 0;
        return;
    }

    hda_args.write_offset += BDL_BUFFER_LN;
    hda_args.buffer += BDL_BUFFER_LN;

}

void hda_handler(){
    // 1. Read Global Interrupt Status
    uint32_t intsts = hda_read_dword(HDA_INTSTS);
    
    // Check if controller caused it (bit 31)
    if (intsts & (1 << 31)) {
        // Check RIRB interrupt (bit 4 of RIRBSTS or check INTSTS bits)
        uint8_t rirbsts = hda_read_byte(HDA_RIRBSTS);
        if (rirbsts & 0x01) { // RIRB interrupt flag
            // Clear RIRB interrupt status by writing 1 back to it
            hda_write_byte(HDA_RIRBSTS, rirbsts | 0x01);
        }

        // Check Stream interrupts (Bits 0-spatial for streams)
        // Iterate through ISS and OSS to clear stream status (SD_STS)
        for (uint8_t i = 0; i < iss; i++) {
            uint8_t sdsts = hda_read_byte(HDA_ISDnSTS(i));
            if (sdsts & 0x1C) { // Buffer completion, FIFO error, descriptor error
                if (sdsts & 0x4) { // Buffer completion
                    handle_write_complete();
                }
                hda_write_byte(HDA_ISDnSTS(i), sdsts); // Clear by writing back
            }
        }
        for (uint8_t i = 0; i < oss; i++) {
            uint8_t sdsts = hda_read_byte(HDA_OSDnSTS(i));
            if (sdsts & 0x1C) {
                if (sdsts & 0x4) {
                    handle_read_complete();
                }
                hda_write_byte(HDA_OSDnSTS(i), sdsts);
            }
        }
    }
    pic_sendEOI(hda_irq);
    apic_sendEOI();
}

extern void isr_hda_handler();

void hda_init()
{
    idt_set_entry(PIC_M_OFFSET + hda_irq, (uint64_t)isr_hda_handler);
    irq_clear_mask(hda_irq);
    ioapic_set_irq(hda_irq, PIC_M_OFFSET + hda_irq, 0);

    // Assert reset: CRST = 0
    hda_write_dword(HDA_GCTL,
                hda_read_dword(HDA_GCTL) & ~HDA_GCTL_CRST);
    do{
        wait(1000);
    } while ((hda_read_dword(HDA_GCTL) & HDA_GCTL_CRST));

    // Deassert reset: CRST = 1
    hda_write_dword(HDA_GCTL,
                    hda_read_dword(HDA_GCTL) | HDA_GCTL_CRST);

    do{
        wait(1000);
    } while (!(hda_read_dword(HDA_GCTL) & HDA_GCTL_CRST));


   uint16_t statests = hda_read_word(HDA_STATESTS);
    for (int i = 0; i < 4; i++) {
        if (statests & (1 << i)) {
            codec_id = i; // This is your active codec address (0 to 3)
            break;
        }
    }
    
    // wake enabled
    hda_write_word(HDA_WAKEEN, 0xFFFF);

    // interrupts
    hda_write_dword(HDA_INTCTL, (1 << 31) | 0x3FFFFFFF);

    hda_get_params();

    hda_init_corb_rirb();

    hda_init_bdl();

    hda_get_node_ids(codec_id);

    hda_configure_codec_path(codec_id);
}
