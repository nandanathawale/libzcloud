/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/*  ***** BEGIN LICENSE BLOCK *****
 * Copyright (C) 2009 Zmanda Incorporated. All Rights Reserved.
 *
 * This file is part of libzcloud.
 *
 * libzcloud is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License (the LGPL)
 * as published by the Free Software Foundation, either version 2.1 of
 * the LGPL, or (at your option) any later version.
 *
 * libzcloud is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *  ***** END LICENSE BLOCK ***** */

#include <errno.h>
#include <poll.h>
#include <string.h>
#include "internal.h"

static void
got_result_impl(
    ZCloudListConsumer *zself,
    const gchar *key)
{
    ZCloudFDListConsumer *self = ZCLOUD_FD_LIST_CONSUMER(zself);
    size_t key_len, written;
    
    if (self->fd < 0) return;

    if (key) {
        key_len = strlen(key);
        for (written = 0; written < key_len; /*nothing*/) {
            size_t w_ret = write(self->fd, key+written, key_len-written);
            if (w_ret < 0) {
                if (EAGAIN == errno) {
                    struct pollfd fds[] = {{self->fd, POLLOUT | POLLERR | POLLHUP | POLLNVAL, 0}};
                    if (poll(fds, 1, -1) < 0) {
                        if (EINTR != errno) {
                            g_debug("an error ocurred while polling fd %d: %s",
                                self->fd, strerror(errno));
                            goto bad_fd;
                        }
                    } else if (fds[0].revents & POLLNVAL) {
                        g_debug("fd %d is not open for reading (anymore?)",
                            self->fd);
                        goto bad_fd;
                    } else if (fds[0].revents & POLLHUP) {
                        g_debug("fd %d has been closed", self->fd);
                        goto bad_fd;
                    } else if (fds[0].revents & POLLERR) {
                        g_debug("fd %d is invalid?", self->fd);
                        goto bad_fd;
                    } else if (fds[0].revents & POLLOUT) {
                        /* nothing */
                    } else {
                        g_assert_not_reached();
                    }
                } else if (EINTR != errno) {
                    g_debug("an error ocurred while writing to fd %d: %s",
                        self->fd, strerror(errno));
                    goto bad_fd;
                }
            } else {
                written += w_ret;
            }
        }
    }

    if (0) {
    bad_fd:
        self->fd = -1;
    }
}

static void
class_init(ZCloudFDListConsumerClass *klass)
{
    ZCloudListConsumerClass *up_class = ZCLOUD_LIST_CONSUMER_CLASS(klass);

    up_class->got_result = got_result_impl;
}

GType
zcloud_fd_list_consumer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudFDListConsumerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudFDListConsumer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_LIST_CONSUMER,
                                      "ZCloudFDListConsumer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

ZCloudFDListConsumer *
zcloud_fd_list_consumer(int fd, gchar suffix)
{
    ZCloudFDListConsumer *ret;

    ret = g_object_new(ZCLOUD_TYPE_FD_LIST_CONSUMER, NULL);
    ret->fd = fd;
    ret->suffix = suffix;

    return ret;
}
