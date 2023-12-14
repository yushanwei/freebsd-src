# $FreeBSD$

.PATH: ${.CURDIR:H}/common

.include <src.opts.mk>

CFLAGS+=	-DCRT_IRELOC_SUPPRESS

.if ${MACHINE_ARCH:Mloongarch64}
ELFCLASS=	64
.else
ELFCLASS=	32
.endif

# binutils requires an explicit emulation for ld -r
LDFLAGS.bfd+=	-Wl,-m -Wl,elf${ELFCLASS}tsmip_fbsd

.include <bsd.lib.mk>
