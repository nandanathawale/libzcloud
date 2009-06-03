/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* vi: set tabstop=4 shiftwidth=4 expandtab: */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: LGPL 2.1/GPL 2.0
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
 *
 * The Original Code is Zmanda Incorporated code.
 *
 * The Initial Developer of the Original Code is
 *  Zmanda Incorporated
 * Portions created by the Initial Developer are Copyright (C) 2009
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *   Nikolas Coukouma <atrus@zmanda.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU General Public License Version 2 or later (the "GPL"),
 * in which case the provisions of the GPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL and not to allow others to
 * use your version of this file under the terms of the LGPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of either the the GPL or the LGPL.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * and GNU General Public License along with libzcloud. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * ***** END LICENSE BLOCK ***** */

#include "internal.h"
#include <openssl/md5.h>

/* class mechanics */
static void
class_init(ZCloudMemoryDownloadConsumerClass *klass);

/* prototypes for method implementations */
static gsize
write_impl(
    ZCloudDownloadConsumer *self,
    gpointer buffer,
    gsize bytes,
    GError **error);

static gboolean
reset_impl(
    ZCloudDownloadConsumer *self,
    GError **error);


static guint8 *
get_contents_impl(
    ZCloudMemoryDownloadConsumer *self,
    gsize *length,
    gboolean copy);

GType
zcloud_memory_download_consumer_get_type(void)
{
    static GType type = 0;

    if (G_UNLIKELY(type == 0)) {
        static const GTypeInfo info = {
            sizeof(ZCloudMemoryDownloadConsumerClass), /* class_size */
            (GBaseInitFunc) NULL, /* base_init */
            (GBaseFinalizeFunc) NULL, /* base_finalize */
            (GClassInitFunc) class_init, /* class_init */
            (GClassFinalizeFunc) NULL, /* class_finalize */
            NULL, /*class_data */
            sizeof(ZCloudMemoryDownloadConsumer), /* instance_size */
            0, /* n_preallocs */
            (GInstanceInitFunc) NULL, /* instance_init */
            NULL /* value_table */
        };

        type = g_type_register_static(ZCLOUD_TYPE_DOWNLOAD_CONSUMER,
                                      "ZCloudMemoryDownloadConsumer",
                                      &info, (GTypeFlags) 0);
    }

    return type;
}

static void
class_init(ZCloudMemoryDownloadConsumerClass *klass)
{
    ZCloudDownloadConsumerClass *up_class = ZCLOUD_DOWNLOAD_CONSUMER_CLASS(klass);

    up_class->write = write_impl;
    up_class->reset = reset_impl;
    klass->get_contents = get_contents_impl;
}


ZCloudMemoryDownloadConsumer *
zcloud_memory_download_consumer(
    guint max_buffer_length)
{
    ZCloudMemoryDownloadConsumer *ret;

    ret = g_object_new(ZCLOUD_TYPE_MEMORY_DOWNLOAD_CONSUMER, NULL);
    ret->buffer = NULL;
    ret->buffer_length = 0;
    ret->max_buffer_length = max_buffer_length;
    ret->buffer_position = 0;

    return ret;
}

guint8 *
zcloud_memory_download_consumer_get_contents(
    ZCloudMemoryDownloadConsumer *self,
    gsize *length,
    gboolean copy)
{
    ZCloudMemoryDownloadConsumerClass *c = ZCLOUD_MEMORY_DOWNLOAD_CONSUMER_GET_CLASS(self);
    g_assert(c->get_contents != NULL);
    return (c->get_contents)(self, length, copy);
}

static gsize
write_impl(
    ZCloudDownloadConsumer *o,
    gpointer buffer,
    gsize bytes,
    GError **error)
{
    ZCloudMemoryDownloadConsumer *self = ZCLOUD_MEMORY_DOWNLOAD_CONSUMER(o);
    guint length_needed = self->buffer_position + bytes;

    if (self->max_buffer_length && length_needed > self->max_buffer_length)
        return 0;

    /* reallocate if necessary. We use exponential sizing to make this
     * happen less often. */
    if (length_needed > self->buffer_length) {
        guint new_length = MAX(length_needed, self->buffer_length * 2);
        if (self->max_buffer_length) {
            new_length = MIN(new_length, self->max_buffer_length);
        }
        self->buffer = g_realloc(self->buffer, new_length);
        self->buffer_length = new_length;
    }

    /* actually copy the data to the buffer */
    memcpy(self->buffer + self->buffer_position, buffer, bytes);
    self->buffer_position += bytes;

    return length_needed;
}

static gboolean reset_impl(
    ZCloudDownloadConsumer *o,
    GError **error)
{
    ZCloudMemoryDownloadConsumer *self = ZCLOUD_MEMORY_DOWNLOAD_CONSUMER(o);

    self->buffer_position = 0;

    return TRUE;
}

static guint8 *
get_contents_impl(
    ZCloudMemoryDownloadConsumer *self,
    gsize *length,
    gboolean copy)
{
    guint8 *ret;

    if (length)
        *length = self->buffer_length;

    if (copy) {
        ret = g_malloc(self->buffer_length);
        memcpy(ret, self->buffer, self->buffer_length);
    } else {
        ret = self->buffer;
    }

    return ret;
}