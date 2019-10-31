#pragma once

namespace client
{
    bool isClientSideError(ULONG errorCode);

    template <typename FuncType, typename ... Args>
    bool makeRpcCall(FuncType* func, Args ... args)
    {
        __try
        {
            __try
            {
                return !func(args...);
            }
            __except (isClientSideError(RpcExceptionCode()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
            {
                std::cout << "Client RPC exception, code: " << RpcExceptionCode() << std::endl;
                return false;
            }

        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "Server RPC exception, code: " << RpcExceptionCode() << std::endl;
            return false;
        }

        return true;
    }
}
