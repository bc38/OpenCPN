/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:
 * Author:   David Register, Alec Leamas
 *
 ***************************************************************************
 *   Copyright (C) 2022 by David Register, Alec Leamas                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/

#ifndef _RESTSERVER_H
#define _RESTSERVER_H

#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>

#include <wx/event.h>

typedef enum RESTServerResult {
  RESULT_NO_ERROR = 0,
  RESULT_GENERIC_ERROR,
  RESULT_OBJECT_REJECTED,
  RESULT_DUPLICATE_REJECTED,
  RESULT_ROUTE_INSERT_ERROR,
  RESULT_NEW_PIN_REQUESTED
} _RESTServerResult;

enum { ORS_START_OF_SESSION, ORS_CHUNK_N, ORS_CHUNK_LAST };

enum { ID_STG_CANCEL = 10000, ID_STG_OK, ID_STG_CHECK1, ID_STG_CHOICE_COMM };

class RESTServerThread;  // Internal
class RESTServerEvent;  // Internal
class PINCreateDialog;

/** Abstract base class visible in callbacks. */
class PinDialog {
public:
  /** Create and show the dialog */
  virtual PinDialog* Initiate(const std::string& msg,
                              const std::string& text1) = 0;

  /** Close and destroy */
  virtual void DeInit() = 0;
};


/** Callbacks invoked from PinDialog implementations. */
class RestServerDlgCtx {
public:
  std::function<PinDialog*(const std::string& msg,
                           const std::string& text1)> show_dialog;
  std::function<void(PinDialog*)> close_dialog;
  std::function<void(void)> update_route_mgr;

  /** Run the "Accept Object" dialog, returns value from ShowModal(). */
  std::function<int (const std::string& msg,
                     const std::string& check1msg)> run_accept_object_dlg;

  RestServerDlgCtx()
      : show_dialog([](
           const std::string&, const std::string&)->PinDialog* { return 0; } ),
        close_dialog([](PinDialog*) {}),
        update_route_mgr([](){ }),
        run_accept_object_dlg(
          [](const std::string&, const std::string&) { return 0; }) {}
};


class RESTServer : public wxEvtHandler {
public:
  RESTServer(RestServerDlgCtx ctx);

  virtual ~RESTServer();

  bool StartServer(std::string certificate_location);
  void StopServer();

  void HandleServerMessage(RESTServerEvent& event);

  //    Secondary thread life toggle
  //    Used to inform launching object (this) to determine if the thread can
  //    be safely called or polled, e.g. wxThread->Destroy();
  void SetSecThreadActive(void) { m_bsec_thread_active = true; }
  void SetSecThreadInActive(void) { m_bsec_thread_active = false; }
  bool IsSecThreadActive() const { return m_bsec_thread_active; }

  void SetSecondaryThread(RESTServerThread* secondary_Thread) {
    m_pSecondary_Thread = secondary_Thread;
  }
  RESTServerThread* GetSecondaryThread() {
    return m_pSecondary_Thread;
  }
  void SetThreadRunFlag(int run) { m_Thread_run_flag = run; }
  void UpdateRouteMgr() { m_dlg_ctx.update_route_mgr(); }

  std::string GetCertificateDirectory(){ return m_certificate_directory; }
  int m_Thread_run_flag;

  std::string m_cert_file;
  std::string m_key_file;

private:
  bool LoadConfig( void );
  bool SaveConfig( void );

  RestServerDlgCtx m_dlg_ctx;
  RESTServerThread* m_pSecondary_Thread;
  bool m_bsec_thread_active;
  std::string m_certificate_directory;
  std::unordered_map<std::string, std::string> m_key_map;
  PINCreateDialog *m_PINCreateDialog;
  wxString m_sPIN;
  int m_dPIN;
  bool m_b_overwrite;
  std::string m_tempUploadFilePath;
  std::ofstream m_ul_stream;


};

#endif  // guard
