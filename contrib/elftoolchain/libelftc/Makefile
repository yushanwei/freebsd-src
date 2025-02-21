# $Id: Makefile 4119 2025-01-28 23:06:08Z jkoshy $

TOP=	..

LIB=	elftc

SRCS=	elftc_bfdtarget.c			\
	elftc_copyfile.c			\
	elftc_demangle.c			\
	elftc_reloc_type_str.c			\
	elftc_set_timestamps.c			\
	elftc_string_table.c			\
	elftc_timestamp.c			\
	elftc_version.c				\
	libelftc_bfdtarget.c			\
	libelftc_dem_arm.c			\
	libelftc_dem_gnu2.c			\
	libelftc_dem_gnu3.c			\
	libelftc_hash.c				\
	libelftc_vstr.c

M4FLAGS=	-DTOP=${TOP}
M4SRCS=	elftc_get_machine_description.m4	\
	elftc_get_machine_name.m4		\
	elftc_get_relocation_type_name.m4

SRCS+=	${M4SRCS:S/.m4/.c/}
CLEANFILES+=	${M4SRCS:S/.m4/.c/}

INCS=		libelftc.h
INCSDIR=	/usr/include

RELEASE=	HEAD		# Change this on release branches.

SHLIB_MAJOR=	1

WARNS?=	6

ELFTC_VERSION_FILE=	${.OBJDIR}/elftc_version.c
CLEANFILES+=		${ELFTC_VERSION_FILE}

LDADD+=	-lelf

MAN=	elftc.3 \
	elftc_bfd_find_target.3 \
	elftc_copyfile.3 \
	elftc_demangle.3 \
	elftc_get_machine_description.3 \
	elftc_reloc_type_str.3 \
	elftc_set_timestamps.3 \
	elftc_string_table_create.3 \
	elftc_version.3

MLINKS=	elftc_bfd_find_target.3 elftc_bfd_target_byteorder.3 \
	elftc_bfd_find_target.3 elftc_bfd_target_class.3 \
	elftc_bfd_find_target.3 elftc_bfd_target_flavor.3 \
	elftc_get_machine_description.3 elftc_get_machine_name.3 \
	elftc_get_machine_description.3 elftc_get_relocation_type_name.3 \
	elftc_string_table_create.3 elftc_string_table_from_section.3 \
	elftc_string_table_create.3 elftc_string_table_destroy.3 \
	elftc_string_table_create.3 elftc_string_table_image.3 \
	elftc_string_table_create.3 elftc_string_table_insert.3 \
	elftc_string_table_create.3 elftc_string_table_lookup.3

.if	!make(clean) && !make(clobber) && !make(obj)
.BEGIN:	.SILENT
	${.CURDIR}/make-toolchain-version -t ${.CURDIR}/${TOP} \
	    -r ${RELEASE} -h ${OS_HOST} -o ${ELFTC_VERSION_FILE}
.endif

# The M4 files have a dependency on "${TOP}/common/sys/elfconstants.m4".
ELF_CONSTANTS_M4=	${TOP}/common/sys/elfconstants.m4
.for f in ${M4SRCS}
${f:S/.m4/.c/}: ${f} ${ELF_CONSTANTS_M4}
.endfor

.include "${TOP}/mk/elftoolchain.lib.mk"
