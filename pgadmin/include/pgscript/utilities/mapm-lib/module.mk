#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2009, The pgAdmin Development Team
# This software is released under the BSD Licence
#
# module.mk - pgadmin/include/pgscript/utilities/mapm-lib/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/utilities/mapm-lib/m_apm.h \
	$(srcdir)/include/pgscript/utilities/mapm-lib/m_apm_lc.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/utilities/mapm-lib/module.mk \
	$(srcdir)/include/pgscript/utilities/mapm-lib/license.txt