#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2012, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/frm/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/frm/frmAbout.h \
	$(srcdir)/include/frm/frmBackup.h \
	$(srcdir)/include/frm/frmBackupGlobals.h \
	$(srcdir)/include/frm/frmBackupServer.h \
	$(srcdir)/include/frm/frmConfig.h \
	$(srcdir)/include/frm/frmDatabaseDesigner.h \
	$(srcdir)/include/frm/frmEditGrid.h \
	$(srcdir)/include/frm/frmExport.h \
  	$(srcdir)/include/frm/frmGrantWizard.h \
  	$(srcdir)/include/frm/frmHbaConfig.h \
	$(srcdir)/include/frm/frmHint.h \
	$(srcdir)/include/frm/frmImport.h \
	$(srcdir)/include/frm/frmMain.h \
	$(srcdir)/include/frm/frmMainConfig.h \
	$(srcdir)/include/frm/frmMaintenance.h \
	$(srcdir)/include/frm/frmOptions.h \
	$(srcdir)/include/frm/frmPassword.h \
	$(srcdir)/include/frm/frmPgpassConfig.h \
	$(srcdir)/include/frm/frmQuery.h \
	$(srcdir)/include/frm/frmReport.h \
	$(srcdir)/include/frm/frmRestore.h \
	$(srcdir)/include/frm/frmSplash.h \
	$(srcdir)/include/frm/frmStatus.h \
    	$(srcdir)/include/frm/menu.h

EXTRA_DIST += \
    $(srcdir)/include/frm/module.mk

