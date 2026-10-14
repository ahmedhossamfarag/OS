#pragma once

#include <stdint.h>

extern "C" {

// Reason codes returned by unwinder functions and personality routines
typedef enum {
    _URC_NO_REASON = 0,
    _URC_FOREIGN_EXCEPTION_CAUGHT = 1,
    _URC_FATAL_PHASE2_ERROR = 2,
    _URC_FATAL_PHASE1_ERROR = 3,
    _URC_NORMAL_STOP = 4,
    _URC_END_OF_STACK = 5,
    _URC_HANDLER_FOUND = 6,
    _URC_INSTALL_CONTEXT = 7,
    _URC_CONTINUE_UNWIND = 8
} _Unwind_Reason_Code;

// Actions requested by the unwinder during the two-phase walk
typedef int _Unwind_Action;
static const _Unwind_Action _UA_SEARCH_PHASE = 1;
static const _Unwind_Action _UA_CLEANUP_PHASE = 2;
static const _Unwind_Action _UA_HANDLER_FRAME = 4;
static const _Unwind_Action _UA_FORCE_UNWIND = 8;
static const _Unwind_Action _UA_END_OF_STACK = 16;

// Opaque structures representing the thread's execution context and exception state
struct _Unwind_Context;
struct _Unwind_Exception;

// The structure prepended to all thrown exceptions, containing unwinder housekeeping data
struct _Unwind_Exception {
    uint64_t exception_class;
    void (*exception_cleanup)(_Unwind_Reason_Code reason, struct _Unwind_Exception* exc);
    uintptr_t private_1;
    uintptr_t private_2;
} __attribute__((__aligned__));

/**
 * @brief Starts the stack-unwinding process.
 * @when Called by `__cxa_throw` to initiate exception handling.
 * @expects Performs a two-phase stack walk:
 *          - Phase 1 (Search): Walks up frames asking personality routines if a handler exists.
 *          - Phase 2 (Cleanup/Unwind): Walks frames again, executing destructors and restoring registers 
 *            until it jumps directly into the target `catch` block.
 */
_Unwind_Reason_Code _Unwind_RaiseException(struct _Unwind_Exception* exception);

/**
 * @brief Resumes stack unwinding after a cleanup action finishes.
 * @when Called when an intermediate frame cleans up local objects during Phase 2 
 *          and needs to continue passing the exception up to older frames.
 * @expects Continues the Phase 2 stack walk from the current context upward.
 */
void _Unwind_Resume(struct _Unwind_Exception* exception);

/**
 * @brief Deletes/destroys an unhandled exception object.
 * @when Called if an exception is thrown but never caught, leading to termination.
 * @expects Invokes the exception's custom cleanup routine and frees its memory.
 */
void _Unwind_DeleteException(struct _Unwind_Exception* exception);

// --- Register & Context Accessors (Used by Personality Routines & Stack Walkers) ---

/**
 * @brief Gets the Instruction Pointer (IP / PC) for a given stack context frame.
 */
uintptr_t _Unwind_GetIP(struct _Unwind_Context* context);

/**
 * @brief Sets the Instruction Pointer (IP / PC) for a stack frame (used when installing context).
 */
void _Unwind_SetIP(struct _Unwind_Context* context, uintptr_t new_value);

/**
 * @brief Gets the Canonical Frame Address (CFA / stack pointer anchor) of a frame.
 */
uintptr_t _Unwind_GetCFA(struct _Unwind_Context* context);

/**
 * @brief Retrieves a general-purpose CPU register value from the saved frame context.
 */
uintptr_t _Unwind_GetGR(struct _Unwind_Context* context, int index);

/**
 * @brief Modifies a general-purpose CPU register value in the saved frame context.
 */
void _Unwind_SetGR(struct _Unwind_Context* context, int index, uintptr_t new_value);

/**
 * @brief The signature of a compiler-generated personality routine (`__gxx_personality_v0`).
 * @when Called by your unwinder for *every* stack frame it traverses during Phase 1 and Phase 2.
 * @expects Reads the frame's LSDA metadata, checks if the frame can handle the exception type 
 *          (Phase 1) or execute local object destructors (Phase 2), and returns appropriate 
 *          `_Unwind_Reason_Code` values.
 */
_Unwind_Reason_Code __gxx_personality_v0(
    int version,
    _Unwind_Action actions,
    uint64_t exceptionClass,
    struct _Unwind_Exception* exceptionObject,
    struct _Unwind_Context* context
);

}