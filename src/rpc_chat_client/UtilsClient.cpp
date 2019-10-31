#include "stdafx.h"
#include "UtilsClient.h"

#define STATUS_POSSIBLE_DEADLOCK        0xC0000194L
#define STATUS_INSTRUCTION_MISALIGNMENT 0xC00000AAL
#define STATUS_HANDLE_NOT_CLOSABLE      0xC0000235L

namespace client
{
    bool isClientSideError(ULONG errorCode)
    {
        return (errorCode != STATUS_ACCESS_VIOLATION)
            && (errorCode != STATUS_POSSIBLE_DEADLOCK)
            && (errorCode != STATUS_INSTRUCTION_MISALIGNMENT)
            && (errorCode != STATUS_DATATYPE_MISALIGNMENT)
            && (errorCode != STATUS_PRIVILEGED_INSTRUCTION)
            && (errorCode != STATUS_ILLEGAL_INSTRUCTION)
            && (errorCode != STATUS_BREAKPOINT)
            && (errorCode != STATUS_STACK_OVERFLOW)
            && (errorCode != STATUS_HANDLE_NOT_CLOSABLE)
            && (errorCode != STATUS_IN_PAGE_ERROR)
            && (errorCode != STATUS_ASSERTION_FAILURE)
            && (errorCode != STATUS_STACK_BUFFER_OVERRUN)
            && (errorCode != STATUS_GUARD_PAGE_VIOLATION);
    }
}
