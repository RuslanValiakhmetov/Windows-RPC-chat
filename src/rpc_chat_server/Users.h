#pragma once

namespace server
{
    struct UserData
    {
        std::string UserName;
        std::vector<HANDLE> UserNotifiers;
    };

    class Users
    {
    public:
        Users() = default;

        void InsertUser(DWORD userID, std::string userName);
        void RemoveUser(DWORD userID);
        bool IsUserOnline(DWORD userID);
        
        std::string GetUsers();
        std::string GetUserNameById(DWORD userID);

        void MessageNotificateAllUsers(ULONG userID);
        void MessageNotificateUser(ULONG userID);
        void AddUserMessageNotifier(ULONG userID, HANDLE userNotification);

        void StatusNotificateAllUsers(ULONG userID);
        void StatusNotificateUser(ULONG userID);
        void AddUserStatusNotifier(ULONG userID, HANDLE userNotification);

    private:
        void NotificateAllUsers(int index, ULONG userID);
        void NotificateUser(int index, ULONG userID);
        void AddUserNotifier(int index, ULONG userID, HANDLE userNotification);

    private:
        std::mutex m_usersLock;
        std::map<DWORD, UserData> m_users;
    };
}
