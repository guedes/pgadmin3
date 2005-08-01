//////////////////////////////////////////////////////////////////////////
//
// pgAdmin III - PostgreSQL Tools
// RCS-ID:      $Id$
// Copyright (C) 2002 - 2005, The pgAdmin Development Team
// This software is released under the Artistic Licence
//
// dlgServer.cpp - PostgreSQL Database Property
//
//////////////////////////////////////////////////////////////////////////

// wxWindows headers
#include <wx/wx.h>

// App headers
#include "pgAdmin3.h"
#include "misc.h"
#include "frmMain.h"
#include "dlgServer.h"
#include "pgDatabase.h"

// Images
#include "images/server.xpm"


// pointer to controls
#define txtDescription  CTRL_TEXT("txtDescription")
#define txtService      CTRL_TEXT("txtService")
#define cbDatabase      CTRL_COMBOBOX("cbDatabase")
#define txtPort         CTRL_TEXT("txtPort")
#define cbSSL           CTRL_COMBOBOX("cbSSL")
#define txtUsername     CTRL_TEXT("txtUsername")
//#define chkNeedPwd      CTRL_CHECKBOX("chkNeedPwd")
#define stTryConnect    CTRL_STATIC("stTryConnect")
#define chkTryConnect   CTRL_CHECKBOX("chkTryConnect")
#define stStorePwd      CTRL_STATIC("stStorePwd")
#define chkStorePwd     CTRL_CHECKBOX("chkStorePwd")
#define stPassword      CTRL_STATIC("stPassword")
#define txtPassword     CTRL_TEXT("txtPassword")



BEGIN_EVENT_TABLE(dlgServer, dlgProperty)
    EVT_NOTEBOOK_PAGE_CHANGED(XRCID("nbNotebook"),  dlgServer::OnPageSelect)  
    EVT_TEXT(XRCID("txtDescription"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtService"),                   dlgProperty::OnChange)
    EVT_TEXT(XRCID("cbDatabase"),                   dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbDatabase"),               dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtPort")  ,                    dlgProperty::OnChange)
    EVT_TEXT(XRCID("txtUsername"),                  dlgProperty::OnChange)
    EVT_COMBOBOX(XRCID("cbSSL"),                    dlgProperty::OnChange)
    EVT_CHECKBOX(XRCID("chkTryConnect"),            dlgServer::OnChangeTryConnect)
    EVT_BUTTON(wxID_OK,                             dlgServer::OnOK)
END_EVENT_TABLE();


dlgServer::dlgServer(frmMain *frame, pgServer *node)
: dlgProperty(frame, wxT("dlgServer"))
{
    SetIcon(wxIcon(server_xpm));
    server=node;
    objectType = PG_SERVER;

    cbDatabase->Append(wxT("postgres"));
    cbDatabase->Append(wxT("template1"));
    wxString lastDB = settings->GetLastDatabase();
    if (lastDB != wxT("postgres") && lastDB != wxT("template1"))
        cbDatabase->Append(lastDB);
    cbDatabase->SetSelection(0);

    txtPort->SetValue(NumToStr((long)settings->GetLastPort()));    
    cbSSL->SetSelection(settings->GetLastSSL());
    txtUsername->SetValue(settings->GetLastUsername());
 
    chkTryConnect->SetValue(true);
    chkStorePwd->SetValue(true);
    if (node)
    {
        chkTryConnect->SetValue(false);
        chkTryConnect->Disable();
    }
}


dlgServer::~dlgServer()
{
    if (!server)
    {
        settings->SetLastDatabase(cbDatabase->GetValue());
        settings->SetLastPort(StrToLong(txtPort->GetValue()));
        settings->SetLastSSL(cbSSL->GetSelection());
        settings->SetLastUsername(txtUsername->GetValue());
    }
}


pgObject *dlgServer::GetObject()
{
    return server;
}


void dlgServer::OnOK(wxCommandEvent &ev)
{
    // notice: changes active after reconnect

    EnableOK(false);


    if (server)
    {
        server->iSetName(GetName());
        server->iSetDescription(txtDescription->GetValue());
        if (txtService->GetValue() != server->GetServiceID())
        {
            mainForm->StartMsg(_("Checking server status"));
            server->iSetServiceID(txtService->GetValue());
            mainForm->EndMsg();
        }
        server->iSetPort(StrToLong(txtPort->GetValue()));
        server->iSetSSL(cbSSL->GetSelection());
        server->iSetDatabase(cbDatabase->GetValue());
        server->iSetUsername(txtUsername->GetValue());
        server->iSetStorePwd(chkStorePwd->GetValue());
        mainForm->execSelChange(server->GetId(), true);
        mainForm->GetBrowser()->SetItemText(item, server->GetFullName());
    }

    if (IsModal())
    {
        EndModal(wxID_OK);
        return;
    }
    else
        Destroy();
}


void dlgServer::OnPageSelect(wxNotebookEvent &event)
{
    // to prevent dlgProperty from catching it
}


wxString dlgServer::GetHelpPage() const
{
    return wxT("connect");
}


int dlgServer::GoNew()
{
    if (cbSSL->IsEmpty())
        return Go(true);
    else
    {
        CheckChange();
        return ShowModal();
    }
}


int dlgServer::Go(bool modal)
{
    cbSSL->Append(wxT(" "));

#ifdef SSL
    cbSSL->Append(_("require"));
    cbSSL->Append(_("prefer"));

    if (pgConnBase::GetLibpqVersion() > 7.3)
    {
        cbSSL->Append(_("allow"));
        cbSSL->Append(_("disable"));
    }
#endif

    if (server)
    {
        if (cbDatabase->FindString(server->GetDatabaseName()) < 0)
            cbDatabase->Append(server->GetDatabaseName());
        txtDescription->SetValue(server->GetDescription());
        txtService->SetValue(server->GetServiceID());
        txtPort->SetValue(NumToStr((long)server->GetPort()));
        cbSSL->SetSelection(server->GetSSL());
        cbDatabase->SetValue(server->GetDatabaseName());
        txtUsername->SetValue(server->GetUsername());
        chkStorePwd->SetValue(server->GetStorePwd());
        stPassword->Disable();
        txtPassword->Disable();
        if (connection)
        {
            txtName->Disable();
            cbDatabase->Disable();
            txtPort->Disable();
            cbSSL->Disable();
            txtUsername->Disable();
            chkStorePwd->Disable();
        }
    }
    else
    {
        SetTitle(_("Add server"));
    }

    int rc=dlgProperty::Go(modal);

    CheckChange();

    return rc;
}


bool dlgServer::GetTryConnect()
{
    return chkTryConnect->GetValue();
}


wxString dlgServer::GetPassword()
{
    return txtPassword->GetValue();
}


pgObject *dlgServer::CreateObject(pgCollection *collection)
{
    wxString name=GetName();

    pgObject *obj=new pgServer(GetName(), txtDescription->GetValue(), cbDatabase->GetValue(), 
        txtUsername->GetValue(), StrToLong(txtPort->GetValue()), chkTryConnect->GetValue() && chkStorePwd->GetValue(), cbSSL->GetSelection());

    return obj;
}


void dlgServer::OnChangeTryConnect(wxCommandEvent &ev)
{
    chkStorePwd->Enable(chkTryConnect->GetValue());
    txtPassword->Enable(chkTryConnect->GetValue());
    OnChange(ev);
}


void dlgServer::CheckChange()
{
    wxString name=GetName();
    bool enable=true;

    if (server)
    {
        enable =  name != server->GetName()
               || txtDescription->GetValue() != server->GetDescription()
               || txtService->GetValue() != server->GetServiceID()
               || StrToLong(txtPort->GetValue()) != server->GetPort()
               || cbDatabase->GetValue() != server->GetDatabaseName()
               || txtUsername->GetValue() != server->GetUsername()
               || cbSSL->GetSelection() != server->GetSSL()
               || chkStorePwd->GetValue() != server->GetStorePwd();
    }


#ifdef __WXMSW__
    CheckValid(enable, !name.IsEmpty(), _("Please specify address."));
#else
    bool isPipe = (name.IsEmpty() || name.StartsWith(wxT("/")));
    cbSSL->Enable(!isPipe);
#endif
    CheckValid(enable, !txtDescription->GetValue().IsEmpty(), _("Please specify description."));
    CheckValid(enable, StrToLong(txtPort->GetValue()) > 0, _("Please specify port."));
    CheckValid(enable, !txtUsername->GetValue().IsEmpty(), _("Please specify user name"));

    EnableOK(enable);
}


wxString dlgServer::GetSql()
{
    return wxEmptyString;
}
