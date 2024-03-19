#include <efi.h>
#include <efilib.h>

EFI_STATUS
efi_main(EFI_HANDLE Ximage, EFI_SYSTEM_TABLE *Xsystab)
{
	ST = Xsystab;

	ST->ConOut->OutputString(ST->ConOut, __DECONST(CHAR16 *, L"YYYYYYYYYYYYYYYYYYYYYYYYY.\r\n"));
	ST->BootServices->Exit(IH, 0, 0, NULL);
}
