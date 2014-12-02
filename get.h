#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include <sys/types.h>
#include "connection.h"

bool beenModified(struct tm* ping, std::string file);
void  getResponse(Request* req, ConnObj* conn_state);

#endif
