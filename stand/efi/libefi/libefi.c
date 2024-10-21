/*-
 * Copyright (c) 2000 Doug Rabson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <efi.h>
#include <eficonsctl.h>
#include <efilib.h>
#include <stand.h>

EFI_HANDLE		IH;
EFI_SYSTEM_TABLE	*ST;
EFI_BOOT_SERVICES	*BS;
EFI_RUNTIME_SERVICES	*RS;

typedef struct {
    UINT16   HobType;
    UINT16   HobLength;
    UINT32   Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct {
  ///
  /// The HOB generic header. Header.HobType = EFI_HOB_TYPE_GUID_EXTENSION.
  ///
  EFI_HOB_GENERIC_HEADER    Header;
  ///
  /// A GUID that defines the contents of this HOB.
  ///
  EFI_GUID                  Name;
  //
  // Guid specific data goes here
  //
} EFI_HOB_GUID_TYPE;


//
// HobType of EFI_HOB_GENERIC_HEADER.
//
#define EFI_HOB_TYPE_HANDOFF              0x0001
#define EFI_HOB_TYPE_MEMORY_ALLOCATION    0x0002
#define EFI_HOB_TYPE_RESOURCE_DESCRIPTOR  0x0003
#define EFI_HOB_TYPE_GUID_EXTENSION       0x0004
#define EFI_HOB_TYPE_FV                   0x0005
#define EFI_HOB_TYPE_CPU                  0x0006
#define EFI_HOB_TYPE_MEMORY_POOL          0x0007
#define EFI_HOB_TYPE_FV2                  0x0009
#define EFI_HOB_TYPE_LOAD_PEIM_UNUSED     0x000A
#define EFI_HOB_TYPE_UEFI_CAPSULE         0x000B
#define EFI_HOB_TYPE_FV3                  0x000C
#define EFI_HOB_TYPE_UNUSED               0xFFFE
#define EFI_HOB_TYPE_END_OF_HOB_LIST      0xFFFF

#define HOB_LIST_GUID \
  { \
    0x7739f24c, 0x93d7, 0x11d4, {0x9a, 0x3a, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d } \
  }

#define FDT_HOB_GUID  {\
          0x16958446, 0x19B7, 0x480B, \
          { 0xB0, 0x47, 0x74, 0x85, 0xAD, 0x3F, 0x71, 0x6D } \
        }


void *GetFdtHob(EFI_HOB_GENERIC_HEADER *HobStart);

void *
efi_get_table(EFI_GUID *tbl)
{
	EFI_GUID *id;
	EFI_GUID hobList = HOB_LIST_GUID;
	void *ret;
	int i;

	for (i = 0; i < ST->NumberOfTableEntries; i++) {
		id = &ST->ConfigurationTable[i].VendorGuid;
		if (!memcmp(id, tbl, sizeof(EFI_GUID)))
			return (ST->ConfigurationTable[i].VendorTable);

		if (!memcmp(id, &hobList, sizeof(EFI_GUID))) {
			if ((ret = GetFdtHob((EFI_HOB_GENERIC_HEADER*)ST->ConfigurationTable[i].VendorTable)))
				return ret;
		}
	}
	return (NULL);
}

EFI_STATUS
OpenProtocolByHandle(EFI_HANDLE handle, EFI_GUID *protocol, void **interface)
{
	return (BS->OpenProtocol(handle, protocol, interface, IH, NULL,
	    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL));
}

void *GetFdtHob(EFI_HOB_GENERIC_HEADER *HobStart) {
	EFI_HOB_GENERIC_HEADER *CurrentHob = HobStart;
	EFI_HOB_GUID_TYPE *guid;
	EFI_GUID *id;
	EFI_GUID fdt = FDT_HOB_GUID;
	UINT64 FdtBase;

	while (CurrentHob->HobType != EFI_HOB_TYPE_END_OF_HOB_LIST) {
		switch (CurrentHob->HobType) {
		case EFI_HOB_TYPE_GUID_EXTENSION:
			guid = (EFI_HOB_GUID_TYPE *)CurrentHob;
			id = &guid->Name;
			if (!memcmp(id, &fdt, sizeof(EFI_GUID))) {
				FdtBase = *(UINT64 *)((UINT8 *)CurrentHob + sizeof (EFI_HOB_GUID_TYPE));
				return (void*)FdtBase;
			}
			break;
		default:
			break;
		}

		// Move to the next HOB in the list
		CurrentHob = (EFI_HOB_GENERIC_HEADER *)((char *)CurrentHob + CurrentHob->HobLength);
	}

	return NULL;
}
