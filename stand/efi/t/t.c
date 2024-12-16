#include <sys/param.h>
#include <machine/elf.h>
#include <machine/stdarg.h>
#include <stand.h>

#include <efi.h>
#include <efilib.h>
#include <eficonsctl.h>
#include <efichar.h>

static EFI_GUID ConsoleControlGUID = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;

static EFI_PHYSICAL_ADDRESS heap;
static UINTN heapsize;

EFI_STATUS
efi_main(EFI_HANDLE Ximage, EFI_SYSTEM_TABLE *Xsystab)
{
	EFI_STATUS status;
	EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl = NULL;
	SIMPLE_TEXT_OUTPUT_INTERFACE *conout = NULL;
	UINTN i, hsize, nhandles;
	CHAR16 *text;


	/* Basic initialization*/
	ST = Xsystab;
	IH = Ximage;
	BS = ST->BootServices;
	RS = ST->RuntimeServices;

	heapsize = 64 * 1024 * 1024;
	status = BS->AllocatePages(AllocateAnyPages, EfiLoaderData,
	    EFI_SIZE_TO_PAGES(heapsize), &heap);
	if (status != EFI_SUCCESS) {
		ST->ConOut->OutputString(ST->ConOut,
		    __DECONST(CHAR16 *,
		    L"Failed to allocate memory for heap.\r\n"));
		BS->Exit(IH, status, 0, NULL);
	}

	setheap((void *)(uintptr_t)heap, (void *)(uintptr_t)(heap + heapsize));

	/* Set up the console, so printf works. */
	status = BS->LocateProtocol(&ConsoleControlGUID, NULL,
	    (VOID **)&ConsoleControl);
	if (status == EFI_SUCCESS)
		(void)ConsoleControl->SetMode(ConsoleControl,
		    EfiConsoleControlScreenText);
	/*
	 * Reset the console enable the cursor. Later we'll choose a better
	 * console size through GOP/UGA.
	 */
	conout = ST->ConOut;
	conout->Reset(conout, TRUE);
	/* Explicitly set conout to mode 0, 80x25 */
	conout->SetMode(conout, 0);
	conout->EnableCursor(conout, TRUE);
	conout->ClearScreen(conout);

	printf("hello world!");
	efi_exit(status);
}

void
efi_exit(EFI_STATUS s)
{

	BS->FreePages(heap, EFI_SIZE_TO_PAGES(heapsize));
	BS->Exit(IH, s, 0, NULL);
}

void
exit(int error __unused)
{
	efi_exit(EFI_LOAD_ERROR);
}

int getchar(void)
{
        return (-1);
}
 
void
putchar(int c)
{
        CHAR16 buf[2];
 
        if (c == '\n') {
                buf[0] = '\r';
                buf[1] = 0;
                ST->ConOut->OutputString(ST->ConOut, buf);
        }
        buf[0] = c;
        buf[1] = 0;
        ST->ConOut->OutputString(ST->ConOut, buf);
}
