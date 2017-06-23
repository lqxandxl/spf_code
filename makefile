UNAME_I=${shell uname -i}
ifeq ($(UNAME_I), x86_64)
X86_64DEF=-fPIC -DX86_64
endif

LIBSMMID = ./libservicetask.a
DLLSMMID = ./servicetask.so

SRC_LIBSMMID = \
	ServiceTask.C\
	NTFMng.C\
	SerMessage.C\
	PublishMng.C\
	UtilService.C\
	SerTreeNode.C\
	SubInfoMng.C

SRC_DLLSMMID = \
	${SRC_LIBSMMID}

SRC_ALL = \
	${SRC_COM}\
	${SRC_DLLSMMID}

OBJ_LIBSMMID = ${SRC_LIBSMMID:.C=.o}
OBJ_DLLSMMID = ${SRC_DLLSMMID:.C=.o}

UNIFRAMEINC = \
	-I.\
	-I../../_lib/xframe/include/comserv\
	-I../../_lib/xframe/include/msg\
	-I../../_lib/xframe/include/kernel\
	-I../../_lib/xframe/include/task\
	-I../../_lib/xframe/include/mysql\
	-I../../_lib/xframe/include/_compat\
	-I../../xframe/include/transport\
	-I../../_lib/xframe/include\
	-I../../_lib/application_test/include

CFLAGS = -g ${UNIFRAMEINC} -Wno-deprecated ${X86_64DEF}

BASEVERSION="\"`cat ../../version| cut -b13-`\""
SVNVERSION="\"`LANG=;svn info | grep Revision | cut -b11-`/`LANG=;svn info | grep 'Last Changed Rev' | cut -b19-`\""
BUILDBY="\"`cat ../../version| grep buildby | cut -b9-`\""
BUILDTIME="\"`date`\""

all: ${LIBSMMID} ${DLLSMMID}

lib: ${LIBSMMID}

dll: ${DLLSMMID}

${LIBSMMID}: ${OBJ_LIBSMMID}
	ar crv ${LIBSMMID} ${OBJ_LIBSMMID}
	@echo

${DLLSMMID}: ${OBJ_DLLSMMID}
	g++ -shared -o ${DLLSMMID} ${OBJ_DLLSMMID} -DDLL_EXPORTS
#	refcheck .

.C.o:   ${SRC_ALL}
	g++ -o $@ -c ${CFLAGS}  $<

clean:
	rm -f ${LIBSMMID} ${DLLSMMID} ${OBJ_DLLSMMID} version.h

LIBDIR = ../../_lib/application_test/lib
DLLDIR = ../../_bin
install:
	mkdir -p ${LIBDIR}/
	cp ${LIBSMMID} ${LIBDIR}/
	cp ${DLLSMMID} ${LIBDIR}/
	mkdir -p ${DLLDIR}/
	mkdir -p ${DLLDIR}/app/
	cp ${DLLSMMID} ${DLLDIR}/app/
