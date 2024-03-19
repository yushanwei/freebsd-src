#include <efi.h>
#include <efilib.h>
#include <efipihob.h>

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
