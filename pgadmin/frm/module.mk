#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2012, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/frm/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/frm/events.cpp \
	$(srcdir)/frm/frmAbout.cpp \
	$(srcdir)/frm/frmBackup.cpp \
	$(srcdir)/frm/frmBackupGlobals.cpp \
	$(srcdir)/frm/frmBackupServer.cpp \
	$(srcdir)/frm/frmConfig.cpp \
	$(srcdir)/frm/frmDatabaseDesigner.cpp \
	$(srcdir)/frm/frmEditGrid.cpp \
	$(srcdir)/frm/frmExport.cpp \
	$(srcdir)/frm/frmGrantWizard.cpp \
	$(srcdir)/frm/frmHbaConfig.cpp \
	$(srcdir)/frm/frmHint.cpp \
	$(srcdir)/frm/frmImport.cpp \
	$(srcdir)/frm/frmMain.cpp \
	$(srcdir)/frm/frmMainConfig.cpp \
	$(srcdir)/frm/frmMaintenance.cpp \
	$(srcdir)/frm/frmOptions.cpp \
	$(srcdir)/frm/frmPassword.cpp \
	$(srcdir)/frm/frmPgpassConfig.cpp \
	$(srcdir)/frm/frmQuery.cpp \
	$(srcdir)/frm/frmReport.cpp \
	$(srcdir)/frm/frmRestore.cpp \
	$(srcdir)/frm/frmSplash.cpp \
	$(srcdir)/frm/frmStatus.cpp \
	$(srcdir)/frm/plugins.cpp

EXTRA_DIST += \
    $(srcdir)/frm/module.mk
