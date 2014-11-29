#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "ConnObj.h"
#include "options.h"
#include <sys/stat.h>
#include <time.h>

void optionsResponse(Request* req, ConnObj* conn_state);

#endif
