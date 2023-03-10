GTK_FLAGS=`pkg-config gtk+-2.0 --cflags`
GTK_LIBS=`pkg-config gtk+-2.0 --libs`

KRB5LIBS=
LDAP_LIBS=
LIBS=-lresolv -ldl -lnetapi -ltdb -ltalloc
DEVELOPER_CFLAGS=-DDEBUG_PASSWORD -DDEVELOPER -g -Wall -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings
CCFLAGS=-I../ -L../../../bin  -I../lib/zlib -Os -pipe -msoft-float -DMAX_DEBUG_LEVEL=-1 -D__location__=\"\" -ffunction-sections -fdata-sections $(FLAGS) -D_SAMBA_BUILD_=3 $(GTK_FLAGS)
CC=mips-unknown-linux-uclibc-gcc
PICFLAG=-fPIC
LDFLAGS= -Wl,--gc-sections -lrt -L./bin
DYNEXP=-Wl,--export-dynamic
NETAPI_LIBS=$(LIBS) $(KRB5LIBS) $(LDAP_LIBS)
CMDLINE_LIBS=$(NETAPI_LIBS) $(POPT_OBJ)

# Compile a source file.
COMPILE_CC = $(CC) -I. $(CCFLAGS) $(PICFLAG) -c $< -o $@
COMPILE = $(COMPILE_CC)

PROGS = bin/getdc \
	bin/dsgetdc \
	bin/netdomjoin \
	bin/netdomjoin-gui \
	bin/getjoinableous \
	bin/getjoininformation \
	bin/rename_machine \
	bin/user_add \
	bin/user_del \
	bin/user_enum \
	bin/user_dispinfo \
	bin/user_chgpwd \
	bin/user_getinfo \
	bin/user_setinfo \
	bin/user_modalsget \
	bin/user_modalsset \
	bin/user_getgroups \
	bin/user_setgroups \
	bin/user_getlocalgroups \
	bin/group_add \
	bin/group_del \
	bin/group_enum \
	bin/group_setinfo \
	bin/group_getinfo \
	bin/group_adduser \
	bin/group_deluser \
	bin/group_getusers \
	bin/group_setusers \
	bin/localgroup_add \
	bin/localgroup_del \
	bin/localgroup_getinfo \
	bin/localgroup_setinfo \
	bin/localgroup_enum \
	bin/localgroup_addmembers \
	bin/localgroup_delmembers \
	bin/localgroup_setmembers \
	bin/localgroup_getmembers \
	bin/remote_tod \
	bin/server_getinfo \
	bin/share_add \
	bin/share_del \
	bin/share_enum \
	bin/share_getinfo \
	bin/share_setinfo \
	bin/file_close \
	bin/file_getinfo \
	bin/file_enum \
	bin/shutdown_init \
	bin/shutdown_abort \
	bin/netlogon_control \
	bin/netlogon_control2 \
	bin/nltest

all: $(PROGS)

MAKEDIR = || exec false; \
	  if test -d "$$dir"; then :; else \
	  echo mkdir "$$dir"; \
	  mkdir -p "$$dir" >/dev/null 2>&1 || \
	  test -d "$$dir" || \
	  mkdir "$$dir" || \
	  exec false; fi || exec false

BINARY_PREREQS = bin/.dummy

bin/.dummy:
	@if (: >> $@ || : > $@) >/dev/null 2>&1; then :; else \
	  dir=bin $(MAKEDIR); fi
	@: >> $@ || : > $@ # what a fancy emoticon!

.c.o:
	@if (: >> $@ || : > $@) >/dev/null 2>&1; then rm -f $@; else \
	 dir=`echo $@ | sed 's,/[^/]*$$,,;s,^$$,.,'` $(MAKEDIR); fi
	@echo Compiling $*.c
	@$(COMPILE) && exit 0;\
		echo "The following command failed:" 1>&2;\
		echo "$(COMPILE_CC)" 1>&2;\
		$(COMPILE_CC) >/dev/null 2>&1

CMDLINE_OBJ = common.o
GETDC_OBJ = getdc/getdc.o $(CMDLINE_OBJ)
DSGETDC_OBJ = dsgetdc/dsgetdc.o $(CMDLINE_OBJ)
NETDOMJOIN_OBJ = join/netdomjoin.o $(CMDLINE_OBJ)
NETDOMJOIN_GUI_OBJ = netdomjoin-gui/netdomjoin-gui.o
GETJOINABLEOUS_OBJ = join/getjoinableous.o $(CMDLINE_OBJ)
GETJOININFORMATION_OBJ = join/getjoininformation.o $(CMDLINE_OBJ)
RENAMEMACHINE_OBJ = join/rename_machine.o $(CMDLINE_OBJ)
USERADD_OBJ = user/user_add.o $(CMDLINE_OBJ)
USERDEL_OBJ = user/user_del.o $(CMDLINE_OBJ)
USERENUM_OBJ = user/user_enum.o $(CMDLINE_OBJ)
USERDISPINFO_OBJ = user/user_dispinfo.o $(CMDLINE_OBJ)
USERCHGPWD_OBJ = user/user_chgpwd.o $(CMDLINE_OBJ)
USERGETINFO_OBJ = user/user_getinfo.o $(CMDLINE_OBJ)
USERSETINFO_OBJ = user/user_setinfo.o $(CMDLINE_OBJ)
USERMODALSGET_OBJ = user/user_modalsget.o $(CMDLINE_OBJ)
USERMODALSSET_OBJ = user/user_modalsset.o $(CMDLINE_OBJ)
USERGETGROUPS_OBJ = user/user_getgroups.o $(CMDLINE_OBJ)
USERSETGROUPS_OBJ = user/user_setgroups.o $(CMDLINE_OBJ)
USERGETLOCALGROUPS_OBJ = user/user_getlocalgroups.o $(CMDLINE_OBJ)
GROUPADD_OBJ = group/group_add.o $(CMDLINE_OBJ)
GROUPDEL_OBJ = group/group_del.o $(CMDLINE_OBJ)
GROUPENUM_OBJ = group/group_enum.o $(CMDLINE_OBJ)
GROUPSETINFO_OBJ = group/group_setinfo.o $(CMDLINE_OBJ)
GROUPGETINFO_OBJ = group/group_getinfo.o $(CMDLINE_OBJ)
GROUPADDUSER_OBJ = group/group_adduser.o $(CMDLINE_OBJ)
GROUPDELUSER_OBJ = group/group_deluser.o $(CMDLINE_OBJ)
GROUPGETUSERS_OBJ = group/group_getusers.o $(CMDLINE_OBJ)
GROUPSETUSERS_OBJ = group/group_setusers.o $(CMDLINE_OBJ)
LOCALGROUPADD_OBJ = localgroup/localgroup_add.o $(CMDLINE_OBJ)
LOCALGROUPDEL_OBJ = localgroup/localgroup_del.o $(CMDLINE_OBJ)
LOCALGROUPGETINFO_OBJ = localgroup/localgroup_getinfo.o $(CMDLINE_OBJ)
LOCALGROUPSETINFO_OBJ = localgroup/localgroup_setinfo.o $(CMDLINE_OBJ)
LOCALGROUPENUM_OBJ = localgroup/localgroup_enum.o $(CMDLINE_OBJ)
LOCALGROUPADDMEMBERS_OBJ = localgroup/localgroup_addmembers.o $(CMDLINE_OBJ)
LOCALGROUPDELMEMBERS_OBJ = localgroup/localgroup_delmembers.o $(CMDLINE_OBJ)
LOCALGROUPSETMEMBERS_OBJ = localgroup/localgroup_setmembers.o $(CMDLINE_OBJ)
LOCALGROUPGETMEMBERS_OBJ = localgroup/localgroup_getmembers.o $(CMDLINE_OBJ)
REMOTETOD_OBJ = server/remote_tod.o $(CMDLINE_OBJ)
SERVERGETINFO_OBJ = server/server_getinfo.o $(CMDLINE_OBJ)
SHAREADD_OBJ = share/share_add.o $(CMDLINE_OBJ)
SHAREDEL_OBJ = share/share_del.o $(CMDLINE_OBJ)
SHAREENUM_OBJ = share/share_enum.o $(CMDLINE_OBJ)
SHAREGETINFO_OBJ = share/share_getinfo.o $(CMDLINE_OBJ)
SHARESETINFO_OBJ = share/share_setinfo.o $(CMDLINE_OBJ)
FILECLOSE_OBJ = file/file_close.o $(CMDLINE_OBJ)
FILEGETINFO_OBJ = file/file_getinfo.o $(CMDLINE_OBJ)
FILEENUM_OBJ = file/file_enum.o $(CMDLINE_OBJ)
SHUTDOWNINIT_OBJ = shutdown/shutdown_init.o $(CMDLINE_OBJ)
SHUTDOWNABORT_OBJ = shutdown/shutdown_abort.o $(CMDLINE_OBJ)
NETLOGONCONTROL_OBJ = netlogon/netlogon_control.o $(CMDLINE_OBJ)
NETLOGONCONTROL2_OBJ = netlogon/netlogon_control2.o $(CMDLINE_OBJ)
NLTEST_OBJ = netlogon/nltest.o $(CMDLINE_OBJ)

bin/getdc: $(BINARY_PREREQS) $(GETDC_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GETDC_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/dsgetdc: $(BINARY_PREREQS) $(DSGETDC_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(DSGETDC_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/getjoinableous: $(BINARY_PREREQS) $(GETJOINABLEOUS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GETJOINABLEOUS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/getjoininformation: $(BINARY_PREREQS) $(GETJOININFORMATION_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GETJOININFORMATION_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/rename_machine: $(BINARY_PREREQS) $(RENAMEMACHINE_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(RENAMEMACHINE_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/netdomjoin: $(BINARY_PREREQS) $(NETDOMJOIN_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(NETDOMJOIN_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/netdomjoin-gui: $(BINARY_PREREQS) $(NETDOMJOIN_GUI_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) $(GTK_FLAGS) -o $@ $(NETDOMJOIN_GUI_OBJ) $(LDFLAGS) $(DYNEXP) $(LIBS) $(KRB5LIBS) $(LDAP_LIBS) $(GTK_LIBS)

bin/user_add: $(BINARY_PREREQS) $(USERADD_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERADD_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_del: $(BINARY_PREREQS) $(USERDEL_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERDEL_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_enum: $(BINARY_PREREQS) $(USERENUM_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERENUM_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_dispinfo: $(BINARY_PREREQS) $(USERDISPINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERDISPINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_chgpwd: $(BINARY_PREREQS) $(USERCHGPWD_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERCHGPWD_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_getinfo: $(BINARY_PREREQS) $(USERGETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERGETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_setinfo: $(BINARY_PREREQS) $(USERSETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERSETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_modalsget: $(BINARY_PREREQS) $(USERMODALSGET_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERMODALSGET_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_modalsset: $(BINARY_PREREQS) $(USERMODALSSET_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERMODALSSET_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_getgroups: $(BINARY_PREREQS) $(USERGETGROUPS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERGETGROUPS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_setgroups: $(BINARY_PREREQS) $(USERSETGROUPS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERSETGROUPS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/user_getlocalgroups: $(BINARY_PREREQS) $(USERGETLOCALGROUPS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(USERGETLOCALGROUPS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_add: $(BINARY_PREREQS) $(GROUPADD_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPADD_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_del: $(BINARY_PREREQS) $(GROUPDEL_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPDEL_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_enum: $(BINARY_PREREQS) $(GROUPENUM_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPENUM_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_setinfo: $(BINARY_PREREQS) $(GROUPSETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPSETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_getinfo: $(BINARY_PREREQS) $(GROUPGETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPGETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_adduser: $(BINARY_PREREQS) $(GROUPADDUSER_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPADDUSER_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_deluser: $(BINARY_PREREQS) $(GROUPDELUSER_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPDELUSER_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_getusers: $(BINARY_PREREQS) $(GROUPGETUSERS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPGETUSERS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/group_setusers: $(BINARY_PREREQS) $(GROUPSETUSERS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(GROUPSETUSERS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_add: $(BINARY_PREREQS) $(LOCALGROUPADD_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPADD_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_del: $(BINARY_PREREQS) $(LOCALGROUPDEL_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPDEL_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_getinfo: $(BINARY_PREREQS) $(LOCALGROUPGETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPGETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_setinfo: $(BINARY_PREREQS) $(LOCALGROUPSETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPSETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_enum: $(BINARY_PREREQS) $(LOCALGROUPENUM_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPENUM_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_addmembers: $(BINARY_PREREQS) $(LOCALGROUPADDMEMBERS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPADDMEMBERS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_delmembers: $(BINARY_PREREQS) $(LOCALGROUPDELMEMBERS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPDELMEMBERS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_setmembers: $(BINARY_PREREQS) $(LOCALGROUPSETMEMBERS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPSETMEMBERS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/localgroup_getmembers: $(BINARY_PREREQS) $(LOCALGROUPGETMEMBERS_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(LOCALGROUPGETMEMBERS_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/remote_tod: $(BINARY_PREREQS) $(REMOTETOD_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(REMOTETOD_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/server_getinfo: $(BINARY_PREREQS) $(SERVERGETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SERVERGETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/share_add: $(BINARY_PREREQS) $(SHAREADD_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SHAREADD_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/share_del: $(BINARY_PREREQS) $(SHAREDEL_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SHAREDEL_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/share_enum: $(BINARY_PREREQS) $(SHAREENUM_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SHAREENUM_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/share_getinfo: $(BINARY_PREREQS) $(SHAREGETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SHAREGETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/share_setinfo: $(BINARY_PREREQS) $(SHARESETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SHARESETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/file_close: $(BINARY_PREREQS) $(FILECLOSE_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(FILECLOSE_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/file_getinfo: $(BINARY_PREREQS) $(FILEGETINFO_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(FILEGETINFO_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/file_enum: $(BINARY_PREREQS) $(FILEENUM_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(FILEENUM_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/shutdown_init: $(BINARY_PREREQS) $(SHUTDOWNINIT_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SHUTDOWNINIT_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/shutdown_abort: $(BINARY_PREREQS) $(SHUTDOWNABORT_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(SHUTDOWNABORT_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/netlogon_control: $(BINARY_PREREQS) $(NETLOGONCONTROL_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(NETLOGONCONTROL_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/netlogon_control2: $(BINARY_PREREQS) $(NETLOGONCONTROL2_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(NETLOGONCONTROL2_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

bin/nltest: $(BINARY_PREREQS) $(NLTEST_OBJ)
	@echo Linking $@
	@$(CC) $(CCFLAGS) -o $@ $(NLTEST_OBJ) $(LDFLAGS) $(DYNEXP) $(CMDLINE_LIBS)

clean:
	-rm -f $(PROGS)
	-rm -f core */*~ *~ \
		*/*.o */*/*.o */*/*/*.o
