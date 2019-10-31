#include "stdafx.h"
#include "UtilsClient.h"
#include "Utils.h"

#define MAX_ERROR_MESSAGE_SIZE 256

using namespace client;

namespace
{
    using RpcBindStrFreeT = decltype(RpcStringFree);
    using RpcBindFreeT = decltype(RpcBindingFree);

    using BindingString = BindingGuard<RPC_WSTR, RpcBindStrFreeT>;
    using BindingHandle = BindingGuard<RPC_BINDING_HANDLE, RpcBindFreeT>;

    using CheckFuncT = STATUS(*)(handle_t, ULONG, UCHAR*);

    // need to configure rpc
    BindingHandle g_bindingHandle(RpcBindingFree);

    std::thread g_checkUsersThread;
    std::thread g_checkMessageThread;
    bool g_needExit = false;

    BOOL WINAPI ctrlCHandler(DWORD /*signal*/)
    {
        //
        // notify other users that current user is leaving the chat
        //
        UCHAR errorMessage[MAX_ERROR_MESSAGE_SIZE];
        if (client_UserExit(*g_bindingHandle, GetCurrentProcessId(), errorMessage))
        {
            std::cout << "ERROR: " << errorMessage << std::endl;
        }

        return g_needExit = true;
    }

    void initRpcConnection(BindingString& bindingString, BindingHandle& bindingHandle)
    {
        //
        // create a string binding handle
        //
        RPC_STATUS status = RpcStringBindingCompose(NULL
            , (RPC_WSTR)L"ncacn_ip_tcp"
            , (RPC_WSTR)L"localhost"
            , (RPC_WSTR)L"7777"
            , NULL
            , &bindingString);

        THROW_IF(status, "Cannot create a string binding handle, error code - " << status << std::endl);

        //
        // get a binding handle from a string representation of a binding handle
        //
        status = RpcBindingFromStringBinding(*bindingString, &bindingHandle);
        THROW_IF(status, "Cannot get a binding handle, error code - " << status << std::endl);
    }

    void startChatting()
    {
        std::cout << "Enter your name: ";

        std::string name;
        std::getline(std::cin, name);

        auto pid = GetCurrentProcessId();
        UCHAR errorMessage[MAX_ERROR_MESSAGE_SIZE];

        bool status = makeRpcCall(client_SendUserInfo, *g_bindingHandle, (UCHAR*)name.c_str(), pid, errorMessage);
        THROW_IF(!status, "ERROR: " << errorMessage);

        while (!g_needExit)
        {
            std::string message;
            std::getline(std::cin, message);

            if (message.empty())
            {
                continue;
            }

            bool status = makeRpcCall(client_SendMessage, *g_bindingHandle, (UCHAR*)message.c_str(), pid, errorMessage);
            THROW_IF(!status, "ERROR: " << errorMessage);
        }
    }

    void CheckUsersThread(CheckFuncT checkFunction)
    {
        UCHAR errorMessage[MAX_ERROR_MESSAGE_SIZE];
        if (checkFunction(*g_bindingHandle, GetCurrentProcessId(), errorMessage))
        {
            std::cout << "ERROR: " << errorMessage << std::endl;
        }
    }
}

int main()
{
    //
    // register Ctrl-C handler to leave correctly chat
    //
    if (!SetConsoleCtrlHandler(ctrlCHandler, TRUE))
    {
        std::cout << "Cannot set Ctrl-C handler" << std::endl;
        return 1;
    }

    // need to configure rpc
    BindingString bindingString(RpcStringFree);
    int retValue = 0;

    try
    {
        initRpcConnection(bindingString, g_bindingHandle);
        startChatting();
    }
    catch (std::exception& exc)
    {
        std::cout << exc.what();
        retValue = -1;
    }

    //
    // wait while other threads are finished
    //

    if (g_checkUsersThread.joinable())
    {
        g_checkUsersThread.join();
    }
    if (g_checkMessageThread.joinable())
    {
        g_checkMessageThread.join();
    }

    return retValue;
}

//
// callback functions
//

void server_GetOnlineUsersCb(unsigned char* message)
{
    std::cout << message << std::endl;

    //
    // start threads which will check message
    // from other users and their status
    //

    g_checkUsersThread = std::thread{ CheckUsersThread, client_CheckUsers };
    g_checkMessageThread = std::thread{ CheckUsersThread, client_CheckMessage };
}

void server_UpdateMessageCb(unsigned char* message)
{
    std::cout << message << std::endl;
}

void server_UpdateUsersStatusCb(unsigned char* message)
{
    std::cout << message << std::endl;
}
