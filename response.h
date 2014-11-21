#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <sys/types.h>
#include <sys/socket.h>
#include "ConnObj.h"

ssize_t sendResponse(Request* req, ConnObj* conn_state);

#endif
