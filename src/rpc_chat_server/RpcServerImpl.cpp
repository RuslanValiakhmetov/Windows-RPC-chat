#include "stdafx.h"
#include "Users.h"
#include "Utils.h"

namespace
{
    using WriteLock = std::unique_lock<std::shared_mutex>;
    using ReadLock = std::shared_lock<std::shared_mutex>;

    std::shared_mutex g_messageRWLock;
    std::string g_currentMessage;

    std::shared_mutex g_statusRWLock;
    std::string g_currentStatus;

    std::shared_mutex g_leaveChatRWLock;
    ULONG g_leavingChatUserId;

    server::Users g_users;

    std::string MakeMessage(ULONG userID, std::string message)
    {
        std::stringstream ss;
        ss << g_users.GetUserNameById(userID) << ": " << message;

        return ss.str();
    }

    std::string MakeStatus(ULONG userID, std::string message)
    {
        std::stringstream ss;
        ss << "\n\t\t" << g_users.GetUserNameById(userID) << " " << message << std::endl;

        return ss.str();
    }

    HANDLE CreateNotifier()
    {
        auto notifier = CreateEvent(NULL, FALSE, FALSE, NULL);
        THROW_IF(!notifier, "Cannot create notifier for current user");

        return notifier;
    }

    void WaitNotification(HANDLE notifier)
    {
        DWORD result = WaitForSingleObject(notifier, INFINITE);
        THROW_IF(WAIT_OBJECT_0 != result, "Notification waiting is failed for current user");

        ResetEvent(notifier);
    }
}

STATUS server_CheckUsers(handle_t IDL_handle,
    unsigned long userID,
    unsigned char* errorMessage)
{
    try
    {
        auto notifier = CreateNotifier();
        g_users.AddUserStatusNotifier(userID, notifier);

        while (g_users.IsUserOnline(userID))
        {
            WaitNotification(notifier);

            //
            // check if user is notified to leave the chat
            //
            {
                ReadLock lock(g_leaveChatRWLock);
                if (g_leavingChatUserId == userID)
                {
                    break;
                }
            }

            //
            // send user status (online/offline)
            //
            std::string status;
            {
                ReadLock statusLock(g_statusRWLock);
                status = g_currentStatus;
            }

            // call client's callback
            client_UpdateUsersStatusCb((UCHAR*)status.c_str());
        }
    }
    catch (const std::exception exc)
    {
        assert(errorMessage);
        memcpy(errorMessage, exc.what(), strlen(exc.what()) + 1);

        return 1;
    }
    
    return 0;
}

STATUS server_CheckMessage(handle_t IDL_handle,
    unsigned long userID,
    unsigned char* errorMessage)
{
    try
    {
        auto notifier = CreateNotifier();
        g_users.AddUserMessageNotifier(userID, notifier);

        while (g_users.IsUserOnline(userID))
        {
            WaitNotification(notifier);

            //
            // check if user is notified to leave the chat
            //
            {
                ReadLock lock(g_leaveChatRWLock);
                if (g_leavingChatUserId == userID)
                {
                    break;
                }
            }

            //
            // send last got message
            //
            std::string message;
            {
                ReadLock messLock(g_messageRWLock);
                message = g_currentMessage;
            }

            // call client's callback
            client_UpdateMessageCb((UCHAR*)message.c_str());
        }
    }
    catch (const std::exception exc)
    {
        assert(errorMessage);
        memcpy(errorMessage, exc.what(), strlen(exc.what()) + 1);

        return 1;
    }
    
    return 0;
}

STATUS server_SendMessage(handle_t IDL_handle,
    unsigned char* message,
    unsigned long userID,
    unsigned char* errorMessage)
{
    try
    {
        WriteLock messLock(g_messageRWLock);
        g_currentMessage = MakeMessage(userID, (char*)message);

        g_users.MessageNotificateAllUsers(userID);
    }
    catch (const std::exception exc)
    {
        assert(errorMessage);
        memcpy(errorMessage, exc.what(), strlen(exc.what()) + 1);

        return 1;
    }

    return 0;
}

STATUS server_SendUserInfo(handle_t IDL_handle,
    unsigned char* userName,
    unsigned long userID,
    unsigned char* errorMessage)
{
    try
    {
        //
        // get online users list 
        //
        std::string users = g_users.GetUsers();
        UCHAR* usersOnline = users.empty()
            ? (UCHAR*)"There are no users online"
            : (UCHAR*)users.c_str();

        // call client's callback
        client_GetOnlineUsersCb(usersOnline);

        // add new user to the list
        g_users.InsertUser(userID, std::string(reinterpret_cast<char*>(userName)));

        //
        // notificate others about new user
        //
        WriteLock statusLock(g_statusRWLock);
        g_currentStatus = MakeStatus(userID, "online now");

        g_users.StatusNotificateAllUsers(userID);
    }
    catch (const std::exception exc)
    {
        assert(errorMessage);
        memcpy(errorMessage, exc.what(), strlen(exc.what()) + 1);

        return 1;
    }

    return 0;
}

STATUS server_UserExit(handle_t IDL_handle,
    unsigned long userID,
    unsigned char* errorMessage)
{
    try
    {
        //
        // notificate others that current user is leaving the chat
        //
        {
            WriteLock statusLock(g_statusRWLock);
            g_currentStatus = MakeStatus(userID, "is left the chat");

            g_users.StatusNotificateAllUsers(userID);
        }

        {
            //
            // set self as leaving the chat
            //
            WriteLock lock(g_leaveChatRWLock);
            g_leavingChatUserId = userID;

            //
            // self-notification to finish wait notifications
            //
            g_users.MessageNotificateUser(userID);
            g_users.StatusNotificateUser(userID);
        }

        g_users.RemoveUser(userID);
    }
    catch (const std::exception exc)
    {
        assert(errorMessage);
        memcpy(errorMessage, exc.what(), strlen(exc.what()) + 1);

        return 1;
    }

    return 0;
}
