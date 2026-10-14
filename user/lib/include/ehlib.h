#pragma once

#include <stdint.h>
#include <stddef.h>

extern "C" {

// ============================================================================
// 1. DWARF & LSDA Common Encoding Types
// ============================================================================

/**
 * @brief Pointer Encoding formats (DW_EH_PE_*).
 * @details GCC encodes pointers in LSDA and .eh_frame using compressed formats 
 *          relative to the PC, data section, or absolute memory addresses.
 */
typedef enum {
    DW_EH_PE_absptr = 0x00,
    DW_EH_PE_omit   = 0xff,
    DW_EH_PE_uleb128 = 0x01,
    DW_EH_PE_udata2  = 0x02,
    DW_EH_PE_udata4  = 0x03,
    DW_EH_PE_udata8  = 0x04,
    DW_EH_PE_sleb128 = 0x09,
    DW_EH_PE_sdata2  = 0x0A,
    DW_EH_PE_sdata4  = 0x0B,
    DW_EH_PE_sdata8  = 0x0C,
    
    // Relative modifiers
    DW_EH_PE_pcrel   = 0x10,
    DW_EH_PE_textrel = 0x20,
    DW_EH_PE_datarel = 0x30,
    DW_EH_PE_funcrel = 0x40,
    DW_EH_PE_aligned = 0x50,
    DW_EH_PE_indirect = 0x80
} Dwarf_Pointer_Encoding;


// ============================================================================
// 2. LEB128 Variable-Length Integer Decoding Helpers
// ============================================================================

/**
 * @brief Decodes an Unsigned Little Endian Base 128 (ULEB128) integer.
 * @when Reading compressed sizes, register numbers, and table offsets in DWARF/LSDA.
 * @expects Takes a pointer to the byte stream, advances the pointer past the integer, 
 *          and returns the decoded 64-bit unsigned value.
 */
uint64_t read_uleb128(const uint8_t** data);

/**
 * @brief Decodes a Signed Little Endian Base 128 (SLEB128) integer.
 * @when Reading signed offsets, stack adjustments, and relative displacements.
 * @expects Takes a pointer to the byte stream, advances the pointer past the integer, 
 *          and returns the decoded 64-bit signed value.
 */
int64_t read_sleb128(const uint8_t** data);


// ============================================================================
// 3. Encoded Pointer Reading Helper
// ============================================================================

/**
 * @brief Reads an encoded pointer based on DWARF Pointer Encoding rules.
 * @when Parsing LSDA headers and call-site tables where pointers can be PC-relative or absolute.
 * @expects Takes the current data stream pointer, the encoding type byte, and context 
 *          references (like current PC or base addresses). Returns the resolved absolute memory address.
 */
uintptr_t read_encoded_pointer(const uint8_t** data, uint8_t encoding, uintptr_t pc_val);


// ============================================================================
// 4. .eh_frame & DWARF CFI (Call Frame Information) Parsers
// ============================================================================

/**
 * @struct FrameDescriptionEntry
 * @brief Represents a parsed FDE from the .eh_frame section for a specific function.
 */
typedef struct {
    uintptr_t pc_begin;       // Start instruction address of the function
    uintptr_t pc_range;       // Size of the function in bytes
    const uint8_t* cie_ptr;   // Pointer to the associated Common Information Entry (CIE)
    const uint8_t* instructions;// Start of the FDE call frame instruction bytecode
    const uint8_t* instructions_end;
} FrameDescriptionEntry;

/**
 * @brief Locates the FDE in .eh_frame corresponding to a specific Instruction Pointer (PC).
 * @when Called by your low-level stack walker (`_Unwind_RaiseException`) for every frame.
 * @expects Scans or binary searches the `.eh_frame_hdr` or `.eh_frame` section 
 *          to find the FDE whose `[pc_begin, pc_begin + pc_range]` contains the given PC.
 */
int parse_fde_for_pc(uintptr_t pc, FrameDescriptionEntry* out_fde);

/**
 * @brief Reconstructs parent register states using DWARF CFI bytecode.
 * @when Called after finding an FDE, to figure out how to restore registers (`rsp`, `rbp`, `rip`) 
 *          from the current stack frame to the caller's frame.
 * @expects Evaluates the CIE initial instructions followed by the FDE instructions up to the target PC offset.
 */
int evaluate_dwarf_instructions(const FrameDescriptionEntry* fde, uintptr_t target_pc, struct _Unwind_Context* context);


// ============================================================================
// 5. LSDA (Language-Specific Data Area) Parsers for Personality Routines
// ============================================================================

/**
 * @struct LSDA_Header
 * @brief Parsed header information of a function's Language-Specific Data Area.
 */
typedef struct {
    uint8_t landing_pad_encoding;
    uint8_t type_table_encoding;
    const uint8_t* type_table;     // Points to array of std::type_info pointers
    const uint8_t* call_site_table;// Points to the call site table mapping PC ranges to actions
    const uint8_t* action_table;   // Points to the catch action chain records
} LSDA_Header;

/**
 * @brief Parses the header of a function's LSDA block.
 * @when Called inside `__gxx_personality_v0` when inspecting a stack frame.
 * @expects Reads encoding formats, base pointers, and offsets from the raw LSDA pointer 
 *          provided by the FDE.
 */
int parse_lsda_header(const uint8_t* lsda_address, LSDA_Header* out_lsda);

/**
 * @brief Searches the LSDA Call Site Table for an entry matching the current PC offset.
 * @when Determining if a `try` block or clean-up region is active at the current instruction.
 * @expects Iterates through the call-site table entries, comparing the function-relative PC 
 *          against call site ranges (`start`, `length`, `landing_pad`, `action_record`).
 *          Returns the action record offset if found.
 */
int find_call_site_action(const LSDA_Header* lsda, uintptr_t function_start_pc, uintptr_t current_pc, int* out_action_offset);

/**
 * @brief Evaluates whether a caught exception type matches a catch block's type info.
 * @when Matching a thrown exception type against a `catch (ExpectedType)` filter.
 * @expects Compares `std::type_info` pointers (handling pointer equality, `type_info` name matching, 
 *          and inheritance upcast checks). Returns 1 if it matches, 0 otherwise.
 */
int match_exception_type(const void* thrown_tinfo, const void* catch_tinfo);

}