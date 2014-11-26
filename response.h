#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <sys/types.h>
#include <sys/socket.h>
#include "ConnObj.h"
#include <sys/stat.h>

bool beenModified(struct tm* ping, std::string file);
ssize_t getResponse(Request* req, ConnObj* conn_state);

#endif
