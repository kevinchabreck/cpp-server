#ifndef __HEAD_H__
#define __HEAD_H__

#include "connection.h"

int headResponse(Request* req, ConnObj* conn_state);
std::string get_relpath(std::string URI);

#endif
