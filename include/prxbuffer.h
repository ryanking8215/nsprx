#ifndef prxbuffer_h__
#define prxbuffer_h__

#include <nspr/prtypes.h>

PR_BEGIN_EXTERN_C

typedef enum PRXByteOrder {
    PRX_ByteOrder_Host,
    PRX_ByteOrder_Network,
} PRXByteOrder;

typedef struct PRXBuffer PRXBuffer;

NSPR_API(PRXBuffer *) PRX_CreateBuffer();
NSPR_API(PRXBuffer *) PRX_CreateBufferWithCap(PRUint32 cap);
NSPR_API(PRXBuffer *) PRX_CreateBufferFull(PRUint32 cap, PRXByteOrder byteorder, PRBool auto_grown);

NSPR_API(void) PRX_DestoryBuffer(PRXBuffer *);

NSPR_API(PRStatus) PRX_SetBufferByteOrder(PRXBuffer * buffer, PRXByteOrder byteorder);

NSPR_API(PRUint32) PRX_GetBufferCapacity(PRXBuffer * buffer);
NSPR_API(PRUint32) PRX_GetBufferLength(PRXBuffer * buffer);
NSPR_API(PRUint32) PRX_GetBufferFree(PRXBuffer * buffer);

NSPR_API(PRStatus) PRX_Buffer_WriteUInt8(PRXBuffer * buffer, PRUint8 v);
NSPR_API(PRStatus) PRX_Buffer_WriteInt8(PRXBuffer * buffer, PRInt8 v);
NSPR_API(PRStatus) PRX_Buffer_WriteUInt16(PRXBuffer * buffer, PRUint16 v);
NSPR_API(PRStatus) PRX_Buffer_WriteInt16(PRXBuffer * buffer, PRInt16 v);
NSPR_API(PRStatus) PRX_Buffer_WriteUInt32(PRXBuffer * buffer, PRUint32 v);
NSPR_API(PRStatus) PRX_Buffer_WriteInt32(PRXBuffer * buffer, PRInt32 v);
NSPR_API(PRStatus) PRX_Buffer_WriteUint64(PRXBuffer * buffer, PRUint64 v);
NSPR_API(PRStatus) PRX_Buffer_WriteInt64(PRXBuffer * buffer, PRInt64 v);
NSPR_API(PRStatus) PRX_Buffer_WriteFloat64(PRXBuffer * buffer, PRFloat64 v);
NSPR_API(PRStatus) PRX_Buffer_WriteString(PRXBuffer * buffer, const char *str);
NSPR_API(PRStatus) PRX_Buffer_WriteBytes(PRXBuffer * buffer, PRUint8* bytes, PRUint32 size);

NSPR_API(PRStatus) PRX_Buffer_Consume(PRXBuffer * buffer, PRInt32 size);
NSPR_API(PRUint8 *) PRX_Buffer_Peek(PRXBuffer * buffer, PRInt32 *size);

NSPR_API(PRStatus) PRX_Buffer_ReadUInt8(PRXBuffer * buffer, PRUint8 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadInt8(PRXBuffer * buffer, PRInt8 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadUint16(PRXBuffer * buffer, PRUint16 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadInt16(PRXBuffer * buffer, PRInt16 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadUint32(PRXBuffer * buffer, PRUint32 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadInt32(PRXBuffer * buffer, PRInt32 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadUint64(PRXBuffer * buffer, PRUint64 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadInt64(PRXBuffer * buffer, PRInt64 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadFloat64(PRXBuffer * buffer, PRFloat64 *v);
NSPR_API(PRStatus) PRX_Buffer_ReadBytes(PRXBuffer * buffer, PRUint8 *bytes, PRInt32 size);


PR_END_EXTERN_C

#endif

