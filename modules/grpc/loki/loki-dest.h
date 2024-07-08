/*
 * Copyright (c) 2024 Axoflow
 * Copyright (c) 2024 Attila Szakacs <attila.szakacs@axoflow.com>
 * Copyright (c) 2023 László Várady
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */

#ifndef LOKI_DEST_H
#define LOKI_DEST_H

#include "syslog-ng.h"

#include "compat/cpp-start.h"
#include "driver.h"
#include "logmsg/logmsg.h"
#include "template/templates.h"
#include "credentials/grpc-credentials-builder.h"

typedef struct _LokiDestDriver LokiDestDriver;

LogDriver *loki_dd_new(GlobalConfig *cfg);

void loki_dd_set_url(LogDriver *d, const gchar *url);
void loki_dd_set_message_template_ref(LogDriver *d, LogTemplate *message);
void loki_dd_add_label(LogDriver *d, const gchar *name, LogTemplate *value);
gboolean loki_dd_set_timestamp(LogDriver *d, const gchar *t);
void loki_dd_set_tenant_id(LogDriver *d, const gchar *tid);

GrpcClientCredentialsBuilderW *loki_dd_get_credentials_builder(LogDriver *s);

void loki_dd_set_keepalive_time(LogDriver *d, gint t);
void loki_dd_set_keepalive_timeout(LogDriver *d, gint t);
void loki_dd_set_keepalive_max_pings(LogDriver *d, gint p);

void loki_dd_add_int_channel_arg(LogDriver *s, const gchar *name, glong value);
void loki_dd_add_string_channel_arg(LogDriver *s, const gchar *name, const gchar *value);

void loki_dd_add_header(LogDriver *s, const gchar *name, const gchar *value);

LogTemplateOptions *loki_dd_get_template_options(LogDriver *d);

#include "compat/cpp-end.h"

#endif
