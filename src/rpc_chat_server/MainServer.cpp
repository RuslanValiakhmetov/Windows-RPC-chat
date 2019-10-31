#include "stdafx.h"
#include "Utils.h"

namespace
{
    using RpcBindVecFreeT = decltype(RpcBindingVectorFree);
    using BindingVector = BindingGuard<RPC_BINDING_VECTOR*, RpcBindVecFreeT>;

    RPC_STATUS RPC_ENTRY CALLBACK securityCallback(RPC_IF_HANDLE /*hInterface*/, void* /*pBindingHandle*/)
    {
        // always allow anyone
        return RPC_S_OK;
    }

    void initRpcConnection(BindingVector& rpcBindingVector)
    {
        //
        // register an interface with the RPC run-time library
        //
        RPC_STATUS status = RpcServerRegisterIf2(server_RpcChat_v1_0_s_ifspec
            , NULL
            , NULL
            , RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH
            , RPC_C_LISTEN_MAX_CALLS_DEFAULT
            , (unsigned)-1
            , securityCallback);

        THROW_IF(status, "Cannot register interface, error code - " << status << std::endl);

        //
        // tell the RPC run-time library to use the specified protocol sequence combined
        // with the specified endpoint for receiving remote procedure calls
        //
        status = RpcServerUseProtseqEp((RPC_WSTR)L"ncacn_ip_tcp"
            , RPC_C_PROTSEQ_MAX_REQS_DEFAULT
            , (RPC_WSTR)L"7777"
            , NULL);

        THROW_IF(status, "Cannot set the specified protocol sequence, error code - " << status << std::endl);

        //
        // get the binding handles over which remote procedure calls can be received
        //
        status = RpcServerInqBindings(&rpcBindingVector);
        THROW_IF(status, "Cannot get the binding handles, error code - " << status << std::endl);


        //
        // add to or replace server address information in the local endpoint-map database
        //
        status = RpcEpRegister(server_RpcChat_v1_0_s_ifspec, *rpcBindingVector, NULL, NULL);
        THROW_IF(status, "Cannot register server address information, error code - " << status << std::endl);
    }
}

int main()
{
    try
    {
        BindingVector rpcBindingVector(RpcBindingVectorFree);

        initRpcConnection(rpcBindingVector);

        uint32_t minimumCallThreads = 1;
        BOOL dontWait = FALSE;

        //
        // signal the RPC run-time library to listen for remote procedure calls
        //
        RPC_STATUS status = RpcServerListen(minimumCallThreads, RPC_C_LISTEN_MAX_CALLS_DEFAULT, dontWait);
        THROW_IF(status, "Cannot listen for remote procedure calls, error code - " << status << std::endl);
    }
    catch (std::exception & exc)
    {
        std::cout << exc.what();
        return -1;
    }

    return 0;
}
