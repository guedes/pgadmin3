//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2007, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// wsapp.cpp - debugger 
//
//////////////////////////////////////////////////////////////////////////

#include "debugger.h"
#include "wsMainFrame.h"
#include "wsCodeWindow.h"
#include "wsBreakPoint.h"
#include "wsDirectdbg.h"

#include <wx/aui/aui.h>
#include <wx/log.h>
#include <wx/dir.h>
#include <wx/filesys.h>
#include <wx/stdpaths.h>

#include <stdexcept>

#define I18N_DIR wxT("/i18n")

IMPLEMENT_APP( wsApp )

wsMainFrame		*glMainFrame = NULL;
wsApp			*glApp       = NULL;

// cmdLineDesc - defines the options, switches, and parameters that we 
// 		 expect to find on the command line.
//
//		 Note:  A switch is a simple toggle - it's presence or absence on the command line
//			determines behavior.  An option has a value (like -p 5432).  A parameter is
//			simply a value that appears at the end of the command line (like psql template1).

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    //                  short        long
    // Entry type       name         name               		description         		                         data type              flags
    // ---------------  ----         -------------      		--------------------------------------               -----------------      -----------
    { wxCMD_LINE_OPTION, wxT( "d" ), wxT( "database" ), 		wxT( "specify database name to connect to" ),        wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_OPTION, wxT( "h" ), wxT( "host" ),     		wxT( "database server host or socket directory" ),   wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_OPTION, wxT( "p" ), wxT( "port" ),     		wxT( "database server port" ),                       wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_OPTION, wxT( "U" ), wxT( "user" ),     		wxT( "database user name" ),                         wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_OPTION, wxT( "k" ), wxT( "debug" ),    		wxT( "attach to debugger" ),                         wxCMD_LINE_VAL_STRING, 0 },

    { wxCMD_LINE_OPTION, wxT( "f" ), wxT( "function" ), 	 	wxT( "set breakpoint on function" ),         		 wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_OPTION, wxT( "s" ), wxT( "procedure" ),		wxT( "set breakpoint on procedure" ),        		 wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_OPTION, wxT( "o" ), wxT( "oid" ),      		wxT( "set breakpoint on object (oid or oid.oid)" ),  wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_OPTION, wxT( "t" ), wxT( "trigger" ),			wxT( "set breakpoint on trigger" ),		     		 wxCMD_LINE_VAL_STRING, 0 },
    { wxCMD_LINE_SWITCH, wxT( "i" ), wxT( "invoke" ),			wxT( "invoke debugger target automatically" ),		 wxCMD_LINE_VAL_NONE,   0 },
    { wxCMD_LINE_OPTION, wxT( "P" ), wxT( "process" ),			wxT( "restrict global breakpoint to given process" ),wxCMD_LINE_VAL_STRING, 0 },

    { wxCMD_LINE_OPTION, wxT( "w" ), wxT( "password" ), 		wxT( "database user password" ),      			     wxCMD_LINE_VAL_STRING, 0 },    	
    { wxCMD_LINE_SWITCH, wxT( "H" ), wxT( "help" ),     		wxT( "help" ),                                       wxCMD_LINE_VAL_NONE,   wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_PARAM,  NULL,       NULL,              		wxT( "database name" ),                              wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

    { wxCMD_LINE_NONE }
};


////////////////////////////////////////////////////////////////////////////////
// OnInit()
//
// 	This function is the entry point for the workstation application (ie. the
// 	'main' function that you would find in a traditional C program)

bool wsApp::OnInit( )
{
	glApp = this;

    // Load the Settings
#ifdef __WXMSW__
	m_settings = new wxConfig( wxT("pgAdmin III"));
#else
	m_settings = new wxConfig( wxT("pgadmin3"));
#endif
	// Initialize our locale and load the language catalog...

	initializeLocale( argv[0] );

    // Get the fonts from pgAdmin's settings
	wxString fontName;
	m_settings->Read(wxT("Font"), &fontName, wxEmptyString);

	if (fontName.IsEmpty())
		systemFont = wxSystemSettings::GetFont(wxSYS_ICONTITLE_FONT);
	else
		systemFont = wxFont(fontName);

    glApp->getSettings().Read(wxT("frmQuery/Font"), &fontName, wxEmptyString);

    if (fontName.IsEmpty())
    {
#ifdef __WXMSW__
        sqlFont = wxFont(9, wxTELETYPE, wxNORMAL, wxNORMAL);
#else
        sqlFont = wxFont(12, wxTELETYPE, wxNORMAL, wxNORMAL);
#endif
    }
    else
    	sqlFont = wxFont(fontName);

	// Disable wxWidgets logging unless ENABLE_LOGGING is defined

	if( getenv( "ENABLE_LOGGING" ) == NULL )
	{
		wxLogNull *disableLogging;
		disableLogging = new( wxLogNull );
	}

	// Disable timestamps on log messages

	wxLog::GetActiveTarget()->SetTimestamp( NULL );	
    
	// Parse the command line according to the cmdLineDesc defined above

	m_cmdLine = new wxCmdLineParser( cmdLineDesc, argc, argv );

	if( m_cmdLine->Parse( true ) != 0 )
		exit( 0 );

	// Create a new frame that manages the entire user interface
	glMainFrame = m_mainFrame = new wsMainFrame( NULL , _( "Debugger" ), wxDefaultPosition, wxSize(800, 600));

	m_mainFrame->Show( true );
	m_mainFrame->Raise();

	SetTopWindow( m_mainFrame );

	// Now interpret the command line arguments (connecting to a PostgreSQl
	// server if we found the right pieces on the command line).
	handleCmdLine( );

	return( true );

}

////////////////////////////////////////////////////////////////////////////////
// OnExit()
//
//	This is the last (application) function called when the workstation 
//  application ends.

int wsApp::OnExit( )
{
	return( 0 );
}

////////////////////////////////////////////////////////////////////////////////
// handleCmdLine()
//
// 	If the user gave us a database, host, port, or user name on the command
//  line, go ahead and connect to the requested server.

void wsApp::handleCmdLine( void )
{
    wxCmdLineParser   * p = m_cmdLine;	// Shortcut

	p->Found( wxT( "d" ), &m_connProp.m_database );
	p->Found( wxT( "p" ), &m_connProp.m_port ); 
	p->Found( wxT( "U" ), &m_connProp.m_userName );
	p->Found( wxT( "w" ), &m_connProp.m_password );

	if( ! p->Found( wxT( "h" ), &m_connProp.m_host ))
		m_connProp.m_host = wxT( "localhost" );

	try
	{

		if( p->Found( wxT( "k" ), &m_connProp.m_debugPort ))
		{
			wsCodeWindow * debugger = m_mainFrame->addDebug( m_connProp );

			debugger->startLocalDebugging();
		   
		}
		else if( p->Found( wxT( "f" )) || p->Found( wxT( "s" )) || p->Found( wxT( "o" )) || p->Found( wxT( "t" )))
		{
			wsCodeWindow *debugger       = NULL;
			wsDirectDbg  *directDebugger = NULL;
			int	targetCount    = 0;
			wxString	target;
			wxString	targetProcess( wxT( "'NULL'" ));

			p->Found( wxT( "P" ), &targetProcess );

			if( p->Found( wxT( "i" )))
				directDebugger = m_mainFrame->addDirectDbg( m_connProp );
			else
				debugger = m_mainFrame->addDebug( m_connProp );

			wsBreakpointList & breakpoints = debugger ? debugger->getBreakpointList() : directDebugger->getBreakpointList();

			if( p->Found( wxT( "t" ), &target ))
			{	
				targetCount++;
				breakpoints.Append( new wsBreakpoint( wsBreakpoint::TRIGGER, target, targetProcess ));
			}

			if( p->Found( wxT( "f" ), &target ))
			{
				targetCount++;
				breakpoints.Append( new wsBreakpoint( wsBreakpoint::FUNCTION, target, targetProcess ));
			}

			if( p->Found( wxT( "s" ), &target ))
			{
				targetCount++;
				breakpoints.Append( new wsBreakpoint( wsBreakpoint::PROCEDURE, target, targetProcess ));
			}

			if( p->Found( wxT( "o" ), &target ))
			{
				targetCount++;
				breakpoints.Append( new wsBreakpoint( wsBreakpoint::OID, target, targetProcess ));
			}

			if( p->Found( wxT( "i" )))
			{
				if( targetCount != 1 )
				{
					wxMessageBox( _( "One of the options -f, -s, -t or -o must be specified." ), _( "Error" ), wxOK | wxICON_ERROR );
					exit( 0 );
				}

				// Hide the main debugger window until we have some parameters
				glMainFrame->Show( false );
				directDebugger->startDebugging();
			}
			else
			{
				debugger->startGlobalDebugging();
			}
		}
		else
		{
			wxMessageBox( _( "-f, -s, -t and -o options are mutually exclusive - you may only use one of them at time." ), _( "Error" ), wxOK | wxICON_ERROR );
		}
	}
	catch( std::runtime_error & error )
	{	
		// Something went wrong - error.what() contains an error 
		// message.
		
		wxMessageBox( wxString(error.what(), wxConvUTF8), _( "Error" ), wxOK | wxICON_ERROR );
		exit( 0 );
	}
}

////////////////////////////////////////////////////////////////////////////////
// getStatusBar()
//
//	Returns a pointer to the status bar - note we can't inline this because it
//  calls a member function of the wsMainFrame class and we don't want to force
//  the wsMainFrame class on everyone that includes debugger.h

wxStatusBar * wsApp::getStatusBar()
{
	return( m_mainFrame->getStatusBar());
}

////////////////////////////////////////////////////////////////////////////////
// initializeLocale()
//
//	This function locates the translation catalog for this application and 
//  initializes the locale for the selected language.

void wsApp::initializeLocale( wxChar * argv0 )
{
	wxString 	loadPath = wxPathOnly( argv0 );
	wxString   	i18nPath;      	// Where i18n data is stored

    if (loadPath.IsEmpty())
        loadPath = wxT(".");

	// Figure out where the pgadmin3 language catalog is located

#ifdef __WXMSW__

    // Search for the right paths. We check the following locations:
    //
    // 1) ./xxx               - Running as a standalone install
    // 2) ../pgAdmin/xxx      - Running in a pgInstaller 8.1 installation 
    //                          (with the .exe and dlls in the main bin dir)
    // 3) ../../xxx or ../xxx - Running in a development environment
    
    if (wxDir::Exists(loadPath + I18N_DIR))
        i18nPath = loadPath + I18N_DIR;
    else if (wxDir::Exists(loadPath + wxT("/../pgAdmin III") + I18N_DIR))
        i18nPath = loadPath + wxT("/../pgAdmin III") + I18N_DIR;
    else 
        i18nPath = loadPath + wxT("/../..") + I18N_DIR;

#else

    wxString dataDir;

#ifdef __WXMAC__

    // When using wxStandardPaths on OSX, wx default to the unix,
    // not to the mac variants. Therefore, we request wxStandardPathsCF
    // directly.
    wxStandardPathsCF stdPaths ;
    dataDir = stdPaths.GetDataDir() ;

#else // other *ixes

// Data path (defined by configure under Unix).
#ifndef DATA_DIR
#define DATA_DIR "./"
#endif

    dataDir = wxString::FromAscii(DATA_DIR);
#endif

    if (wxDir::Exists(dataDir + I18N_DIR))
        i18nPath = dataDir + I18N_DIR;
#ifdef __WXMAC__ // On Mac, the catalog might be in the parent bundle.
    else if (wxDir::Exists(dataDir + wxT("/../../..") + I18N_DIR))
        i18nPath = dataDir + wxT("/../../..") + I18N_DIR;
#endif

#endif

	wxLocale * locale = new wxLocale();

	locale->AddCatalogLookupPathPrefix( i18nPath );

	wxLanguage langId = (wxLanguage)m_settings->Read(wxT("LanguageId"), wxLANGUAGE_DEFAULT);

	if( locale->Init( langId ))
	{
#ifdef __LINUX__
		locale->AddCatalog(wxT("fileutils"));
#endif
		locale->AddCatalog( wxT( "pgadmin3" ));
	}
}