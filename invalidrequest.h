#ifndef __BAD_REQUEST_H__
#define __BAD_REQUEST_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "connection.h"

void send404(ConnObj* conn_state);
void send401(ConnObj* conn_state);


#endif
