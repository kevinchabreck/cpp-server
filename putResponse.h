#ifndef __PUTRESPONSE_H__
#define __PUTRESPONSE_H__

#include <sys/types.h>
#include <sys/socket.h>
#include "ConnObj.h"
#include <sys/stat.h>


void putResponse(Request* req, ConnObj* conn_state);

#endif
