/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2024.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* svmsg_file.h

   Header file for svmsg_file_server.c and svmsg_file_client.c.
*/
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h>                     /* For definition of offsetof() */
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "tlpi_hdr.h"

#define SERVER_KEY 0x1aaaaaa1           /* Key for server's message queue */

#define MAX_CLIENTS 64

/**
 * Types for request / response messages sent from server to client
 */
typedef enum {
      REQ_MT_UNKNOWN    = 0
    , REQ_MT_INIT       = 1     // New client initialization
    , REQ_MT_INCR               // Increment client's counter
    , REQ_MT_DECR               // Decrement client's counter
    , REQ_MT_GET_CTR            // Retrieve current state of counter
    , REQ_MT_QUIT               // This client wants to quit

    // Server will send RESP_MT_QUIT to all active clients.
    , REQ_MT_EXIT               // Client asks application to exit

    // Response message types send by server to client.
    , RESP_MT_FAILURE           // Some failure indicator recd from server
    , RESP_MT_DATA              // Message contains data (counter)
    , RESP_MT_END               // End of message stream (unused)
    , RESP_MT_INCR  = REQ_MT_INCR
    , RESP_MT_DECR  = REQ_MT_DECR
    , RESP_MT_QUIT  = REQ_MT_QUIT


} req_resp_type_t;


typedef struct requestMsg {             /* Requests (client to server) */
    req_resp_type_t mtype;              /* Request msg type */
    int             clientId;           /* ID of client's message queue */
    int             client_idx;         /* Client's index on server-side array */
    int64_t         counter;            /* Client's counter value */
} requestMsg;

#define REQ_MSG_SIZE    sizeof(requestMsg)

// At client initialization time, we don't know its index on server-side
#define REQ_CLIENT_INDEX_UNKNOWN    ((int) -1)

typedef struct responseMsg {            /* Responses (server to client) */
    req_resp_type_t mtype;              /* One of request/response type IDs */
    int             clientId;           /* ID of client's message queue */
    int             client_idx;         /* Client's index on server-side array */
    int64_t         counter;            /* Client's counter value */
} responseMsg;

#define RESP_MSG_SIZE sizeof(responseMsg)

