#include "stdafx.h"
#include "Users.h"
#include "Utils.h"

#define MESSAGE_NOTIFICATION_INDEX 0
#define STATUS_NOTIFICATION_INDEX 1

namespace server
{
    void Users::InsertUser(DWORD userID, std::string userName)
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        std::stringstream ss;
        ss << userName << "_" << userID;

        auto result = m_users.insert({ userID, { ss.str(), { nullptr/*message*/, nullptr/*status*/ } } });
        THROW_IF(!result.second, "Cannot add user")
    }

    void Users::RemoveUser(DWORD userID)
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        auto it = m_users.find(userID);
        THROW_IF(m_users.end() == it, "Cannot remove user. User does not exist");

        for (auto notifier : it->second.UserNotifiers)
        {
            if (notifier)
            {
                CloseHandle(notifier);
            }
        }

        m_users.erase(it);
    }

    bool Users::IsUserOnline(DWORD userID)
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        if (m_users.empty())
        {
            return false;
        }

        auto it = m_users.find(userID);
        if (m_users.end() == it)
        {
            return false;
        }

        return true;
    }

    std::string Users::GetUsers()
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        if (m_users.empty())
        {
            return std::string();
        }

        std::stringstream ss;
        ss << "Users online:" << std::endl;
        for (auto& user : m_users)
        {
            ss << "\t" << user.second.UserName << std::endl;
        }

        return ss.str();
    }

    std::string Users::GetUserNameById(DWORD userID)
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        auto it = m_users.find(userID);
        THROW_IF(m_users.end() == it, "Cannot get name, user does not exist");

        return it->second.UserName;
    }

    void Users::MessageNotificateAllUsers(ULONG userID)
    {
        NotificateAllUsers(MESSAGE_NOTIFICATION_INDEX, userID);
    }

    void Users::MessageNotificateUser(ULONG userID)
    {
        NotificateUser(MESSAGE_NOTIFICATION_INDEX, userID);
    }

    void Users::AddUserMessageNotifier(ULONG userID, HANDLE userNotification)
    {
        AddUserNotifier(MESSAGE_NOTIFICATION_INDEX, userID, userNotification);
    }

    void Users::StatusNotificateAllUsers(ULONG userID)
    {
        NotificateAllUsers(STATUS_NOTIFICATION_INDEX, userID);
    }

    void Users::StatusNotificateUser(ULONG userID)
    {
        NotificateUser(STATUS_NOTIFICATION_INDEX, userID);
    }

    void Users::AddUserStatusNotifier(ULONG userID, HANDLE userNotification)
    {
        AddUserNotifier(STATUS_NOTIFICATION_INDEX, userID, userNotification);
    }

    void Users::NotificateAllUsers(int index, ULONG userID)
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        for (auto& user : m_users)
        {
            //
            // skip self-notification
            //
            if (userID == user.first)
            {
                continue;
            }

            auto result = SetEvent(user.second.UserNotifiers[index]);
            THROW_IF(!result, "Cannot notificate user " << userID);
        }
    }

    void Users::NotificateUser(int index, ULONG userID)
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        auto it = m_users.find(userID);
        THROW_IF(m_users.end() == it, "User " << userID << " has not " << index << " notifier");

        auto result = SetEvent(it->second.UserNotifiers[index]);
        THROW_IF(!result, "Cannot notificate user " << userID);
    }

    void Users::AddUserNotifier(int index, ULONG userID, HANDLE userNotification)
    {
        std::lock_guard<std::mutex> lock(m_usersLock);

        auto it = m_users.find(userID);
        THROW_IF(m_users.end() == it, "User " << userID << " does not exist");

        m_users[userID].UserNotifiers[index] = userNotification;
    }
}
