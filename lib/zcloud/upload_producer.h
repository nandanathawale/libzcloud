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


#ifndef ZCLOUD_UPLOAD_PRODUCER_H
#define ZCLOUD_UPLOAD_PRODUCER_H

G_BEGIN_DECLS

GType zcloud_upload_producer_get_type(void);
#define ZCLOUD_TYPE_UPLOAD_PRODUCER (zcloud_upload_producer_get_type())
#define ZCLOUD_UPLOAD_PRODUCER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), ZCLOUD_TYPE_UPLOAD_PRODUCER, ZCloudUploadProducer))
#define ZCLOUD_UPLOAD_PRODUCER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), ZCLOUD_TYPE_UPLOAD_PRODUCER, ZCloudUploadProducerClass))
#define ZCLOUD_IS_UPLOAD_PRODUCER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), ZCLOUD_TYPE_UPLOAD_PRODUCER))
#define ZCLOUD_IS_UPLOAD_PRODUCER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), ZCLOUD_TYPE_UPLOAD_PRODUCER))
#define ZCLOUD_UPLOAD_PRODUCER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZCLOUD_TYPE_UPLOAD_PRODUCER, ZCloudUploadProducerClass))

#define ZCLOUD_MD5_HASH_BYTE_LEN 16

typedef struct ZCloudUploadProducer_s {
    GObject parent;

} ZCloudUploadProducer;

typedef struct ZCloudUploadProducerClass_s {
    GObjectClass parent_class;

    gsize (*read)(ZCloudUploadProducer *self, gpointer buffer, gsize bytes, GError **error);
    gsize (*get_size)(ZCloudUploadProducer *self, GError **error);
    GByteArray* (*calculate_md5)(ZCloudUploadProducer *self, GError **error);
    gboolean (*reset)(ZCloudUploadProducer *self, GError **error);
} ZCloudUploadProducerClass;

gsize zcloud_upload_producer_read(
    ZCloudUploadProducer *self,
    gpointer buffer,
    gsize bytes,
    GError **error);

gsize zcloud_upload_producer_get_size(ZCloudUploadProducer *self, GError **error);

GByteArray* zcloud_upload_producer_calculate_md5(
    ZCloudUploadProducer *self,
    GError **error);

gboolean zcloud_upload_producer_reset(ZCloudUploadProducer *self, GError **error);

G_END_DECLS

#endif
