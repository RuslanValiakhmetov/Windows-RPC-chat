

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Tue Jan 19 05:14:07 2038
 */
/* Compiler settings for rpc_chat.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */


#ifndef __rpc_chat_h__
#define __rpc_chat_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __RpcChat_INTERFACE_DEFINED__
#define __RpcChat_INTERFACE_DEFINED__

/* interface RpcChat */
/* [version][uuid] */ 

typedef unsigned long STATUS;

/* client prototype */
STATUS client_SendUserInfo( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ unsigned char *userName,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);
/* server prototype */
STATUS server_SendUserInfo( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ unsigned char *userName,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);

/* client prototype */
STATUS client_SendMessage( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ unsigned char *message,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);
/* server prototype */
STATUS server_SendMessage( 
    /* [in] */ handle_t IDL_handle,
    /* [string][in] */ unsigned char *message,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);

/* client prototype */
STATUS client_UserExit( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);
/* server prototype */
STATUS server_UserExit( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);

/* client prototype */
STATUS client_CheckUsers( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);
/* server prototype */
STATUS server_CheckUsers( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);

/* client prototype */
STATUS client_CheckMessage( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);
/* server prototype */
STATUS server_CheckMessage( 
    /* [in] */ handle_t IDL_handle,
    /* [in] */ unsigned long userID,
    /* [string][out] */ unsigned char errorMessage[ 256 ]);

/* client prototype */
/* [callback] */ void client_GetOnlineUsersCb( 
    /* [string][in] */ unsigned char *message);
/* server prototype */
/* [callback] */ void server_GetOnlineUsersCb( 
    /* [string][in] */ unsigned char *message);

/* client prototype */
/* [callback] */ void client_UpdateUsersStatusCb( 
    /* [string][in] */ unsigned char *message);
/* server prototype */
/* [callback] */ void server_UpdateUsersStatusCb( 
    /* [string][in] */ unsigned char *message);

/* client prototype */
/* [callback] */ void client_UpdateMessageCb( 
    /* [string][in] */ unsigned char *message);
/* server prototype */
/* [callback] */ void server_UpdateMessageCb( 
    /* [string][in] */ unsigned char *message);



extern RPC_IF_HANDLE client_RpcChat_v1_0_c_ifspec;
extern RPC_IF_HANDLE RpcChat_v1_0_c_ifspec;
extern RPC_IF_HANDLE server_RpcChat_v1_0_s_ifspec;
#endif /* __RpcChat_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


