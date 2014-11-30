#ifndef __INVALID_REQUEST_H__
#define __INVALID_REQUEST_H__

#include "connection.h"

void send404(ConnObj* conn_state);
void send401(ConnObj* conn_state);

#endif
