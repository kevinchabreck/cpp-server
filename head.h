#ifndef __HEAD_H__
#define __HEAD_H__

#include <sys/types.h>
#include <sys/socket.h>
#include "ConnObj.h"

void headResponse(Request* req, ConnObj* conn_state);

#endif
