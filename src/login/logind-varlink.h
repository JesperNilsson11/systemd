/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "sd-bus.h"

typedef struct Manager Manager;
typedef struct Session Session;

int manager_varlink_init(Manager *m);
void manager_varlink_done(Manager *m);

int session_send_create_reply_varlink(Session *s, const sd_bus_error *error);
