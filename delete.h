#ifndef __DELETE_H__
#define __DELETE_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "connection.h"

void deleteResponse(Request* req, ConnObj* conn_state);

#endif
