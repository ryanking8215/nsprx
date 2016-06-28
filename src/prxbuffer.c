#include "prxbuffer.h"

#include <string.h>
#include <nspr/nspr.h>

#define PRXBUFFER_DEFAULT_CAP (1024)

struct PRXBuffer
{
    PRUint32 cap;
    PRUint32 r;
    PRUint32 w;
    PRUint8 * bytes;
    PRPackedBool auto_grown;
    PRXByteOrder byteorder;
};

static PRStatus _resize(PRXBuffer *buffer, PRUint32 append_size)
{
    PRUint32 len = PRX_GetBufferLength(buffer);
    if (len + append_size < buffer->cap) {
        memmove(buffer->bytes, buffer->bytes+buffer->r, len);
    } 
    else {
        if (!buffer->auto_grown) {
            return PR_FAILURE;
        }
        PRUint32 cap = PR_MAX(len+append_size, buffer->cap*3/2);
        PRUint8 * new_bytes = (PRUint8 *)PR_MALLOC(cap);
        if (new_bytes == NULL) {
            return PR_FAILURE;
        }
        memcpy(new_bytes, buffer->bytes+buffer->r, len);
        PR_FREEIF(buffer->bytes);
        buffer->cap = cap;
        buffer->bytes = new_bytes;
    }
    buffer->r = 0;
    buffer->w = len;
    return PR_SUCCESS;
}

static PRStatus _write_bytes(PRXBuffer *buffer, PRUint8 *bytes, PRUint32 size)
{
    if (buffer==NULL || bytes==NULL || size==0) {
        return PR_FAILURE;
    }
    PRStatus status;
    if (buffer->w+size > buffer->cap) {
        status = _resize(buffer, size);
        if (status == PR_FAILURE) {
            return status;
        }
    }

    memcpy(buffer->bytes+buffer->w, bytes, size);
    buffer->w+=size;
    status = PR_SUCCESS;

    return status;
}

static PRStatus _read_bytes(PRXBuffer *buffer, PRUint8* bytes, PRUint32 size)
{
    if (buffer==NULL || bytes==NULL) {
        return PR_FAILURE;
    }
    if (size==0) {
        return PR_SUCCESS;
    }
    if (PRX_GetBufferLength(buffer) < size) {
        return PR_FAILURE;
    }
    memcpy(bytes, buffer->bytes+buffer->r, size);
    buffer->r+=size;
    return PR_SUCCESS;
}


PR_IMPLEMENT(PRXBuffer*)
    PRX_CreateBuffer()
{
    return PRX_CreateBufferFull(PRXBUFFER_DEFAULT_CAP, PRX_ByteOrder_Host, PR_TRUE);
}

PR_IMPLEMENT(PRXBuffer*)
    PRX_CreateBufferWithCap(PRUint32 cap)
{
    return PRX_CreateBufferFull(cap, PRX_ByteOrder_Host, PR_TRUE);
}

PR_IMPLEMENT(PRXBuffer*)
    PRX_CreateBufferFull(PRUint32 cap, PRXByteOrder byteorder, PRBool auto_grown)
{
    if (cap==0) {
        cap = PRXBUFFER_DEFAULT_CAP;
    }
    PRXBuffer * b = PR_NEW(PRXBuffer);
    if (b!=NULL) {
        memset(b, 0, sizeof(*b));
        b->cap = cap;
        b->byteorder = byteorder;
        b->auto_grown = auto_grown;
        b->bytes = (PRUint8 *)PR_MALLOC(cap);
        if (b->bytes == NULL) {
            PR_FREEIF(b);
            b = NULL;
        }
    }
    return b;
}

PR_IMPLEMENT(void)
    PRX_DestoryBuffer(PRXBuffer *b)
{
    if (b!=NULL) {
        PR_FREEIF(b->bytes);
    }
    PR_FREEIF(b);
}

PR_IMPLEMENT(PRStatus) PRX_SetBufferByteOrder(PRXBuffer * buffer, PRXByteOrder byteorder)
{
    if (buffer==NULL) {
        return PR_FAILURE;
    }
    buffer->byteorder = byteorder;
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRUint32) PRX_GetBufferCapacity(PRXBuffer * buffer)
{
    if (buffer != NULL) {
        return buffer->cap;
    }
    return 0;
}

PR_IMPLEMENT(PRUint32) PRX_GetBufferLength(PRXBuffer * buffer)
{
    if (buffer != NULL) {
        return buffer->w - buffer->r;
    }
    return 0;
}

PR_IMPLEMENT(PRUint32) PRX_GetBufferFree(PRXBuffer * buffer)
{
    if (buffer != NULL) {
        return buffer->cap - PRX_GetBufferLength(buffer);
    }
    return 0;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteUint8(PRXBuffer * buffer, PRUint8 v)
{
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteInt8(PRXBuffer * buffer, PRInt8 v)
{
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteUint16(PRXBuffer * buffer, PRUint16 v)
{
    if (buffer != NULL) {
        if (buffer->byteorder == PRX_ByteOrder_Network) {
            v = PR_htons(v);
        }
    }
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteInt16(PRXBuffer * buffer, PRInt16 v)
{
    if (buffer != NULL) {
        if (buffer->byteorder == PRX_ByteOrder_Network) {
            v = PR_htons((PRUint16)v);
        }
    }
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteUint32(PRXBuffer * buffer, PRUint32 v)
{
    if (buffer != NULL) {
        if (buffer->byteorder == PRX_ByteOrder_Network) {
            v = PR_htonl(v);
        }
    }
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteInt32(PRXBuffer * buffer, PRInt32 v)
{
    if (buffer != NULL) {
        if (buffer->byteorder == PRX_ByteOrder_Network) {
            v = PR_htonl((PRUint32)v);
        }
    }
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteUint64(PRXBuffer * buffer, PRUint64 v)
{
    if (buffer != NULL) {
        if (buffer->byteorder == PRX_ByteOrder_Network) {
            v = PR_htonll((PRUint64)v);
        }
    }
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteFloat64(PRXBuffer * buffer, PRFloat64 v)
{
    if (buffer != NULL) {
        if (buffer->byteorder == PRX_ByteOrder_Network) {
            v = PR_htonll((PRUint64)v);
        }
    }
    return _write_bytes(buffer, (PRUint8 *)&v, sizeof(v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteString(PRXBuffer * buffer, const char *str)
{
    PRUint32 len = 0;
    if (str!=NULL) {
        len = (PRUint32)strlen(str);
    }
    return _write_bytes(buffer, (PRUint8 *)str, len);
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_WriteBytes(PRXBuffer * buffer, PRUint8* bytes, PRUint32 size)
{
    return _write_bytes(buffer, (PRUint8 *)bytes, size);
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_Consume(PRXBuffer * buffer, PRInt32 size)
{
    if (buffer==NULL) {
        return PR_FAILURE;
    }
    if (size<0 || size>PRX_GetBufferLength(buffer)) {
        size = PRX_GetBufferLength(buffer);
    }
    buffer->r+=size;
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRUint8 *) PRX_Buffer_Peek(PRXBuffer * buffer, PRInt32 *size)
{
    if (buffer==NULL || size==NULL) {
        return NULL;
    }
    if (*size<0 || *size>PRX_GetBufferLength(buffer)) {
        *size = PRX_GetBufferLength(buffer);
    }
    return buffer->bytes+buffer->r;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadUInt8(PRXBuffer * buffer, PRUint8 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    return _read_bytes(buffer, v, sizeof(*v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadInt8(PRXBuffer * buffer, PRInt8 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    return _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadUint16(PRXBuffer * buffer, PRUint16 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    PRStatus status = _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
    if (status==PR_FAILURE) {
        return status;
    }
    if (buffer->byteorder == PRX_ByteOrder_Network) {
        *v = PR_ntohs(*v);
    }
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadInt16(PRXBuffer * buffer, PRInt16 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    PRStatus status = _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
    if (status==PR_FAILURE) {
        return status;
    }
    if (buffer->byteorder == PRX_ByteOrder_Network) {
        *v = PR_ntohs((PRUint16)(*v));
    }
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadUint32(PRXBuffer * buffer, PRUint32 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    PRStatus status = _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
    if (status==PR_FAILURE) {
        return status;
    }
    if (buffer->byteorder == PRX_ByteOrder_Network) {
        *v = PR_ntohl((PRUint32)(*v));
    }
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadInt32(PRXBuffer * buffer, PRInt32 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    PRStatus status = _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
    if (status==PR_FAILURE) {
        return status;
    }
    if (buffer->byteorder == PRX_ByteOrder_Network) {
        *v = (PRInt32)PR_ntohl((PRUint32)(*v));
    }
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadUint64(PRXBuffer * buffer, PRUint64 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    PRStatus status = _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
    if (status==PR_FAILURE) {
        return status;
    }
    if (buffer->byteorder == PRX_ByteOrder_Network) {
        *v = (PRInt64)PR_ntohll((PRUint64)(*v));
    }
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadInt64(PRXBuffer * buffer, PRInt64 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    PRStatus status = _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
    if (status==PR_FAILURE) {
        return status;
    }
    if (buffer->byteorder == PRX_ByteOrder_Network) {
        *v = (PRInt64)PR_ntohll((PRUint64)(*v));
    }
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadFloat64(PRXBuffer * buffer, PRFloat64 *v)
{
    if (v==NULL) {
        return PR_FAILURE;
    }
    PRStatus status = _read_bytes(buffer, (PRUint8 *)v, sizeof(*v));
    if (status==PR_FAILURE) {
        return status;
    }
    if (buffer->byteorder == PRX_ByteOrder_Network) {
        *v = (PRFloat64)PR_ntohll((PRUint64)(*v));
    }
    return PR_SUCCESS;
}

PR_IMPLEMENT(PRStatus) PRX_Buffer_ReadBytes(PRXBuffer * buffer, PRUint8 *bytes, PRInt32 size)
{
    if (size<0) {
        size = PRX_GetBufferLength(buffer);
    }
    return _read_bytes(buffer, bytes, size);
}

