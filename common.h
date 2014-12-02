#ifndef __COMMON_H__
#define __COMMON_H__

#include "connection.h"

// status codes
void send100(ConnObj* conn_state);
void send204(ConnObj* conn_state);
void send401(ConnObj* conn_state);
void send404(ConnObj* conn_state);
void send500(ConnObj* conn_state);
// helper methods
std::string getTimestamp();

#endif
