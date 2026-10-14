#pragma once

#include <stddef.h>

// Forward declaration of type_info from standard library
namespace std {
    class type_info;
}

extern "C" {

/**
 * @brief Allocates memory for an exception object and its internal ABI header.
 * @when Called by the compiler immediately before executing a `throw` expression.
 * @expects Must allocate (thrown_size + sizeof(__cxa_exception)) bytes. The returned 
 *          pointer points directly to the payload area where the exception object 
 *          is constructed in-place. Must handle out-of-memory safely (e.g., via 
 *          an emergency buffer).
 */
void* __cxa_allocate_exception(size_t thrown_size);

/**
 * @brief Frees an allocated exception object if it is discarded or cleaned up.
 * @when Called if an exception is constructed but the throw statement fails 
 *          before unwinding starts, or during cleanup if an exception leaks.
 * @expects Deallocates the memory block allocated by __cxa_allocate_exception.
 */
void __cxa_free_exception(void* thrown_exception);

/**
 * @brief Initiates the exception throwing process.
 * @when Called right after the exception object is constructed during a `throw`.
 * @expects Takes the exception payload pointer, its type_info, and its destructor. 
 *          It wraps the object into the internal exception header and hands control 
 *          over to the low-level unwinder (`_Unwind_RaiseException`). 
 *          **This function never returns normally.**
 */
void __cxa_throw(void* thrown_exception, 
                 std::type_info* tinfo, 
                 void (*dest)(void*));

/**
 * @brief Prepares the runtime and thread state when entering a `catch` block.
 * @when Called at the very beginning of a `catch (Type& e)` handler body.
 * @expects Takes the raw exception object pointer passed by the unwinder, 
 *          increments its reference count, marks it as currently being handled, 
 *          adds it to the thread's active exception stack, and returns the 
 *          adjusted pointer to the user's exception object payload.
 */
void* __cxa_begin_catch(void* excPtr);

/**
 * @brief Cleans up state when exiting a `catch` block.
 * @when Called at the end of a `catch` block (or if an exception leaves the catch).
 * @expects Decrements the reference count of the current exception. If the 
 *          reference count drops to zero, it destroys and frees the exception object 
 *          and pops it from the thread-local active exception stack.
 */
void __cxa_end_catch();

/**
 * @brief Re-throws the currently active exception (`throw;`).
 * @when Called when a `throw;` statement appears outside of an active throw context.
 * @expects Grabs the current exception from the thread-local state and restarts 
 *          the unwinding process via `_Unwind_RaiseException`.
 */
void __cxa_rethrow();

/**
 * @brief Retrieves the pointer to the actual thrown object from the exception header.
 * @when Used internally by the personality routine or catch dispatchers.
 * @expects Offsets the pointer from the internal ABI exception header to locate 
 *          the user's exception payload.
 */
void* __cxa_get_exception_ptr(void* exceptionObject);

}