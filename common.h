#ifndef __COMMON_H__
#define __COMMON_H__

#include "connection.h"

// status codes
void send100(ConnObj* conn_state);
void send201(ConnObj* conn_state);
void send204(ConnObj* conn_state);
void send400(ConnObj* conn_state);
void send401(ConnObj* conn_state);
void send403(ConnObj* conn_state);
void send404(ConnObj* conn_state);
void send405(ConnObj* conn_state);
void send411(ConnObj* conn_state);
void sendHTML(ConnObj* conn_state, std::string status);
void send500(ConnObj* conn_state);
// helper methods
std::string getTimestamp();
std::string getContentType(std::string filePath);
#endif
