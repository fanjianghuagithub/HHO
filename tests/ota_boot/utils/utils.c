/***************************************************************************
 *
 * File:
 *     $Id: utils.c 1411 2009-04-15 22:10:58Z suneelk $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 1411 $
 *
 * Description:
 *     This file contains utility function code for stacks
 *     and add-in components. Including functions for list 
 *     manipulation. These functions may be mapped to OS function 
 *     calls in some instances.
 *
 * Created:
 *     June 20, 1995
 *
 * Copyright 1995-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "osapi.h"
#include "utils.h"

#if XA_USE_ENDIAN_MACROS == XA_DISABLED
/*---------------------------------------------------------------------------
 *            BEtoHost16()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieve a 16-bit number from the given buffer. The number
 *            is in Big-Endian format.
 *
 * Return:    16-bit number.
 */
U16 _BEtoHost16(const U8* ptr)
{
    return (U16)( ((U16) *ptr << 8) | ((U16) *(ptr+1)) ); 
}

/*---------------------------------------------------------------------------
 *            BEtoHost32()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieve a 32-bit number from the given buffer. The number
 *            is in Big-Endian format.
 *
 * Return:    32-bit number.
 */
U32 _BEtoHost32(const U8* ptr) 
{
    return (U32)( ((U32) *ptr << 24)   | \
                  ((U32) *(ptr+1) << 16) | \
                  ((U32) *(ptr+2) << 8)  | \
                  ((U32) *(ptr+3)) );
}

/*---------------------------------------------------------------------------
 *            LEtoHost16()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieve a 16-bit number from the given buffer. The number
 *            is in Little-Endian format.
 *
 * Return:    16-bit number.
 */
U16 _LEtoHost16(const U8* ptr)
{
    return (U16)( ((U16) *(ptr+1) << 8) | ((U16) *ptr) ); 
}

/*---------------------------------------------------------------------------
 *            LEtoHost32()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Retrieve a 32-bit number from the given buffer. The number
 *            is in Little-Endian format.
 *
 * Return:    32-bit number.
 */
U32 _LEtoHost32(const U8* ptr) 
{
    return (U32)( ((U32) *(ptr+3) << 24)   | \
                  ((U32) *(ptr+2) << 16) | \
                  ((U32) *(ptr+1) << 8)  | \
                  ((U32) *(ptr)) );
}

/*---------------------------------------------------------------------------
 *            StoreLE16()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Store 16 bit value into a buffer in Little Endian format.
 *
 * Return:    void
 */
void _StoreLE16(U8 *buff, U16 le_value) 
{
   buff[1] = (U8)(le_value>>8);
   buff[0] = (U8)le_value;
}

/*---------------------------------------------------------------------------
 *            StoreLE32()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Store 32 bit value into a buffer in Little Endian format.
 *
 * Return:    void
 */
void _StoreLE32(U8 *buff, U32 le_value)
{
   buff[3] = (U8) (le_value>>24);
   buff[2] = (U8) (le_value>>16);
   buff[1] = (U8) (le_value>>8);
   buff[0] = (U8) le_value;
}

/*---------------------------------------------------------------------------
 *            StoreBE16()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Store 16 bit value into a buffer in Big Endian format.
 *
 * Return:    void
 */
void _StoreBE16(U8 *buff, U16 be_value) 
{
   buff[0] = (U8)(be_value>>8);
   buff[1] = (U8)be_value;
}

/*---------------------------------------------------------------------------
 *            StoreBE32()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Store 32 bit value into a buffer in Big Endian format.
 *
 * Return:    void
 */
void _StoreBE32(U8 *buff, U32 be_value)
{
   buff[0] = (U8) (be_value>>24);
   buff[1] = (U8) (be_value>>16);
   buff[2] = (U8) (be_value>>8);
   buff[3] = (U8) be_value;
}

#endif /* XA_USE_ENDIAN_MACROS == XA_DISABLED */

/*---------------------------------------------------------------------------
 *            _InsertTailList()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Insert an entry at the tail of the list specified by head.
 *
 * Return:    void
 */
void _InsertTailList(ListEntry* head, ListEntry* entry)
{
  Assert(entry != head->Blink);
  entry->Flink = head;
  entry->Blink = head->Blink;
  head->Blink->Flink = entry;
  head->Blink = entry;
  Assert(IsNodeConnected(entry));

}

/*---------------------------------------------------------------------------
 *            _InsertHeadList()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Insert an entry at the head of the list specified by head.
 *
 * Return:    void
 */
void _InsertHeadList(ListEntry* head, ListEntry* entry)
{
  entry->Flink = head->Flink;
  entry->Blink = head;
  head->Flink->Blink = entry;
  head->Flink = entry;
  Assert(IsNodeConnected(entry));

}

/*---------------------------------------------------------------------------
 *            _RemoveHeadList()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Remove the first entry on the list specified by head.
 *
 * Return:    void
 */
ListEntry* _RemoveHeadList(ListEntry* head)
{
  ListEntry* first;

  first = head->Flink;
  first->Flink->Blink = head;
  head->Flink = first->Flink;
  Assert(IsListCircular(head));
  return(first);

}

/*---------------------------------------------------------------------------
 *           RemoveEntryList()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Remove the given entry from the list.
 *
 * Return:    void
 *
 */
void RemoveEntryList(ListEntry* entry)
{
  Assert(IsListCircular(entry));
  entry->Blink->Flink = entry->Flink;
  entry->Flink->Blink = entry->Blink;
  Assert(IsListCircular(entry->Blink));
  InitializeListEntry(entry);

}

/*---------------------------------------------------------------------------
 *            IsNodeOnList()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determine if an entry is on the list specified by head.
 *
 * Return:    TRUE - the entry is on the list.
 *            FALSE - the entry is not on the list.
 */
BOOL IsNodeOnList(ListEntry* head, ListEntry* node)
{
  ListEntry* tmpNode;

  Assert(IsListCircular(head));
  tmpNode = GetHeadList(head);

  while (tmpNode != head)
  {
    if (tmpNode == node)
      return(TRUE);

    tmpNode = tmpNode->Flink;
  }
  return(FALSE);

}

/*---------------------------------------------------------------------------
 *            MoveList()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Moves a list to a new list head (which need not be initialized)
 *            The source list is left empty. 
 *
 * Return:    none
 */
void MoveList(ListEntry* dest, ListEntry* src)
{
    Assert(IsListCircular(src));

    if (IsListEmpty(src)) {
        InitializeListHead(dest);

    } else {
        dest->Flink = src->Flink;
        dest->Blink = src->Blink;
        src->Flink->Blink = dest;
        src->Blink->Flink = dest;
        InitializeListHead(src);
    }
}

#if XA_DEBUG == XA_ENABLED
/*---------------------------------------------------------------------------
 *            IsListCircular()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determine if the list is circular. This is only used for
 *            debugging purposes.
 *
 * Return:    TRUE - the list is circular
 *            FALSE - the list is not circular.
 */
BOOL IsListCircular(ListEntry* list)
{
  ListEntry* tmp = list;

  if (!IsNodeConnected(list))
    return(FALSE);

  for (tmp = tmp->Flink; tmp != list; tmp = tmp->Flink)
    if (!IsNodeConnected(tmp))
      return(FALSE);

  return(TRUE);

}

/*---------------------------------------------------------------------------
 *            VerifyMacros()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Determine if defined macros are functioning properly.
 */
void VerifyMacros(void)
{
    struct MacroTests {
        U8      f1;
        U16     f2[3];
        U8      f3;
        U32     f4;
    } mt;
    U16 a, b;
    
    /* The following Asserts verify the ContainingRecord() macro.
     * If an assert fails, there is a problem with your compilers handling
     * of the macro. A macro compatible with your compiler should be placed
     * in the overide.h file to override the default version found in config.h.
     */
    Assert(ContainingRecord(&mt.f3, struct MacroTests, f3) == &mt);
    Assert(ContainingRecord(&mt.f4, struct MacroTests, f3) != &mt);

    /* The following code verifies that comma operators execute left-to-right
     * and return the rvalue, which is necessary for the IterateListSafe macro
     * to work properly.
     */
    a = 2;
    b = 0;
    b = a++, b = a + 1;
    Assert(b == 4);
}
#else
BOOL IsListCircular(ListEntry* list)
{
  ListEntry* tmp = list;

  if (!IsNodeConnected(list))
    return(FALSE);

  for (tmp = tmp->Flink; tmp != list; tmp = tmp->Flink)
    if (!IsNodeConnected(tmp))
      return(FALSE);

  return(TRUE);

}
#endif /* XA_DEBUG */


/*---------------------------------------------------------------------------
 *            XA_StageInput()
 *---------------------------------------------------------------------------
 *
 * Synopsis: Stages data into previously specified staging buffer.
 * 
 * Return: BT_STATUS_PENDING or _SUCCESS.
 */
XaStatus XA_StageInput(XaStager *stager, U8 **inputBuff, U16 *inputLen)
{
    U16 len;

    /* If the target is zero we're in auto-success mode. This allows
     * state machines to call SimStageInput repeatedly with no data
     * and get SUCCESS.
     */
    if (stager->target == 0) return XA_STATUS_SUCCESS;

    Assert(stager->target >= stager->cur);

    len = min(*inputLen, stager->target - stager->cur);

    /* Only copy input data when not in skip mode. */
    if (!stager->skip) {
        OS_MemCopy(stager->buff + stager->cur, *inputBuff, len);
    }

    /* Increment everything */
    stager->cur += len;
    *inputBuff += len;
    *inputLen -= len;

    /* If we have reached our staging target, return success */
    if (stager->cur == stager->target) {
        stager->target = 0;
        return XA_STATUS_SUCCESS;
    }
    return XA_STATUS_PENDING;
}

/****************************************************************************
 *
 *            Decoder functions
 *
 ***************************************************************************/

#if XA_DECODER == XA_ENABLED

#ifndef XADC
XadContext xadContext = { 0 };
#define XADC(c) (xadContext.c)
#endif

/* Prefixes for values or types that are not recognized */
#define XAD_UNKNOWN_STR "Unknown: "
#define XAD_UNKNOWN_STR_LEN 9
#define XAD_UNKNOWN_TYPE_STR "Unknown type: "
#define XAD_UNKNOWN_TYPE_LEN 14

/*---------------------------------------------------------------------------
 * XadOutputHex()
 *
 *     Writes a value into a string as an uppercase hexadecimal, including 
 *     the 0x prefix.
 *
 * Parameters:
 *     value - Integer value to decode
 *
 *     string - String in which to write the value. Must contain enough
 *         room for entire string including null terminator.
 *
 *     width - Width in bits for 0-padding (e.g. 32 for 0x00000000)
 *
 */
static void XadOutputHex(U32 value, char *string, U8 width)
{
    U8 n;

    /* Convert width to # of chars required */
    width = width / 4;
    if (value > 0xFF) width = 4;
    if (value > 0xFFFF) width = 8;

    string = string + width + 2;
    *string-- = 0;

    /* Write chars from lowest precision to highest */
    while (width > 0)
    {
        n = (U8) (value & 0xF);
        if (n < 10)
            *string-- = (U8)('0' + n);
        else *string-- = (U8)('A' + n - 10);

        value >>= 4;
        width--;
    }
    *string-- = 'x';
    *string = '0';
    return;
}

/*---------------------------------------------------------------------------
 *            getOutputString()
 *---------------------------------------------------------------------------
 *
 * Returns an output string which can be used for a short time. Not
 * particularly thread-safe.
 */
static char *getOutputString()
{
    char *result;

    result = XADC(decodeStrings)[XADC(curDecodeString)];
    XADC(curDecodeString) = (XADC(curDecodeString)+1) % XAD_MAX_STRINGS;
    result[0] = 0;

    return result;
}


/*---------------------------------------------------------------------------
 *            xadDecodeByType()
 *---------------------------------------------------------------------------
 *
 * Pass in an array of XadEntry items and a value, xadDecodeByType will
 * return a match or a constructed string containing the value in text form.
 */
static const char *xadDecodeByType(const XadEntry *arr, U16 arrLen, U32 val)
{
    const char *result = 0;
    char *output;
    U16 pos;

    /* Keep output at 0 until we've done some string building */
    output = 0;

    /* Go through entire array */
    for (pos = 0; pos < arrLen; pos++)
    {
        if (arr[pos].str == XAD_OPCODE_HEXVALUE) {
            /* For hexvalue, build a string based on precision */
            if (!output) {
                result = output = getOutputString();
            }
            XadOutputHex(val, output, (U8)arr[pos].value);
            output += OS_StrLen(output);
            Assert(output + OS_StrLen(output) - result < XAD_MAX_STRING_SIZE);
            break;
            
        } if (arr[pos].str == XAD_OPCODE_DROPBITS) {
            /* For dropbits, zero out specified bits; no other action */
            val &= ~arr[pos].value;

        } else if (arr[pos].value == val) {

            /* If possible, immediately return the ROM string itself */
            if (!output) return (char *)arr[pos].str;

            /* Otherwise add it to output and quit searching */
            OS_MemCopy((U8 *)output, (U8 *)arr[pos].str, OS_StrLen((char *)arr[pos].str) + 1);
            output += OS_StrLen(output);
            Assert(output + OS_StrLen(output) - result < XAD_MAX_STRING_SIZE);
            break;
        }
    }

    /* If nobody wrote anything, write in our own HEX text */
    if (result == output) {
        if (!output) {
            result = output = getOutputString();
        }
        OS_MemCopy((U8 *)output, (U8 *)XAD_UNKNOWN_STR, XAD_UNKNOWN_STR_LEN);
        output += 9;
        XadOutputHex(val, output, 8);
        output += OS_StrLen(output);
        Assert(output - result < XAD_MAX_STRING_SIZE);
    }

    return result;
}

/*---------------------------------------------------------------------------
 *            XAD_RegisterDecoder()
 *---------------------------------------------------------------------------
 *
 * Registers a type for use later by XadDecode_int
 */
void XAD_RegisterDecoder(const XadEntry *decodeArray, U16 decodeArrayLen,
                         const char *name)
{
    U16 i;

    /* Search for an emtpy slot or one with a matching name. */
    for(i = 0; i < XAD_MAX_TYPES; i++) {
        if ((XADC(typeNames)[i] != 0) && 
            (OS_StrCmp(name, XADC(typeNames)[i]) == 0)) {
            break;
        }

        if (XADC(typeNames)[i] == 0) {
            break;
        }
    }

    if (i < XAD_MAX_TYPES) {
        /* If a slot was found, register there */
        XADC(typeNames)[i] = name;
        XADC(entries)[i] = decodeArray;
        XADC(entryLens)[i] = decodeArrayLen;
    } else {
        /* Exceeded XAD_MAX_TYPES */
        Assert(0);
    }
}


/*---------------------------------------------------------------------------
 *            XAD_Decode()
 *---------------------------------------------------------------------------
 *
 * Converts integer to string
 */
const char *XAD_Decode(U32 value, const char *name)
{
    const char *result;
    char *output;
    U16 i;

    /* Search for a matching type */
    for(i = 0; i < XAD_MAX_TYPES; i++) {
        /* Out of names? Quit */
        if (XADC(typeNames)[i] == 0) break;

        /* Found a match? Either pointers match, or contents might */
        if ((name == XADC(typeNames)[i]) ||            
            (OS_StrCmp(name, XADC(typeNames)[i]) == 0)) {
            /* Return decoded value */
            result = xadDecodeByType(XADC(entries)[i], XADC(entryLens)[i], value);
            goto done;
        }
    }

    /* Unregistered type name; build it. */
    result = output = getOutputString();
    OS_MemCopy((U8 *)result, (U8 *)XAD_UNKNOWN_TYPE_STR, XAD_UNKNOWN_TYPE_LEN);
    output += XAD_UNKNOWN_TYPE_LEN;
    XadOutputHex(value, output, 8);
done:
    return result;
}

#endif /* XA_DECODER */

