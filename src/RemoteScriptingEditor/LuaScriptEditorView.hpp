//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file LuaScriptEditorView.hpp View for editing Lua scripts
//
#pragma once

// includes
#include <atlfind.h>
#include <algorithm>

// adapter class to use CScintillaWindow in WTL classes
class ScintillaWindowAdapter : public CScintillaWindow
{
public:
   /// sets selection
   void SetSel(int nStart, int nEnd, BOOL /*bRedraw*/ = TRUE)
   {
      CScintillaWindow::SetSel(static_cast<unsigned int>(nStart), static_cast<unsigned int>(nEnd));
   }

   /// returns selected text
   int GetSelText(CString& cszText) const
   {
      unsigned int uiBufferLen = CScintillaWindow::GetSelText(nullptr);

      CStringA cszaText;
      int iRet = CScintillaWindow::GetSelText(cszaText.GetBuffer(uiBufferLen + 1));
      cszaText.ReleaseBuffer();

      cszText = cszaText;

      return iRet;
   }

   /// replaces selection with text
   void ReplaceSel(const CString& cszText)
   {
      CScintillaWindow::ReplaceSel(CStringA(cszText));
   }

   /// returns position from character
   POINT PosFromChar(UINT nChar) const
   {
      POINT point =
      {
         CScintillaWindow::PointXFromPosition(nChar),
         CScintillaWindow::PointYFromPosition(nChar)
      };

      return point;
   }

   /// hides selection
   void HideSelection(BOOL bHide = TRUE, BOOL /*bChangeStyle*/ = FALSE)
   {
      CScintillaWindow::HideSelection(bHide != FALSE);
   }
};

/// \brief edit commands mixin for scintilla windows
/// \details adds handler for "Edit | Redo" command
template <typename T>
class ScintillaEditCommands : public CEditCommands<T>
{
   // this class type
   typedef ScintillaEditCommands<T> ThisClass;

   /// base class type
   typedef CEditCommands<T> BaseClass;

public:
   BEGIN_MSG_MAP(ThisClass)
   ALT_MSG_MAP(1)
      COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditRedo)
      CHAIN_MSG_MAP_ALT(BaseClass, 1)
   END_MSG_MAP()

   /// called for command "Edit | Redo"
   LRESULT OnEditRedo(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
   {
      T* pT = static_cast<T*>(this);

      pT->Redo();
      return 0;
   }
};

/// \brief find/replace impl for scintilla windows
/// \details implement FindTextSimple() called by CFindReplaceDialog class.
/// Also adds message filter, so that tab, escape and other keys work in the dialog.
template <typename T>
class ScintillaFindReplaceImpl :
   public CEditFindReplaceImplBase<T, CFindReplaceDialog>
{
   /// this class type
   typedef ScintillaFindReplaceImpl<T> ThisClass;

   /// base class type
   typedef CEditFindReplaceImplBase<T, CFindReplaceDialog> BaseClass;

public:
   /// pre-translates message
   BOOL PreTranslateMessage(MSG* pMsg)
   {
      if (m_pFindReplaceDialog == nullptr)
         return FALSE;

      T* pT = static_cast<T*>(this);

      HWND hWndFocus = ::GetFocus();
      if ((pT->m_hWnd == hWndFocus) || m_pFindReplaceDialog->IsChild(hWndFocus))
         return m_pFindReplaceDialog->IsDialogMessage(pMsg);

      return FALSE;
   }

   /// finds text; used by CEditFindReplaceImplBase
   BOOL FindTextSimple(LPCTSTR lpszFind, BOOL bMatchCase, BOOL bWholeWord, BOOL bFindDown = TRUE)
   {
      T* pT = static_cast<T*>(this);

      TextToFind tf = { 0 };

      CStringA cszaTextToFind = lpszFind;
      tf.lpstrText = cszaTextToFind;

      tf.chrg.cpMin = pT->GetSelectionStart();
      tf.chrg.cpMax = pT->GetSelectionEnd();

      if (m_bFirstSearch)
      {
         if (bFindDown)
            m_nInitialSearchPos = tf.chrg.cpMin;
         else
            m_nInitialSearchPos = tf.chrg.cpMax;

         m_bFirstSearch = FALSE;
      }

      if (tf.chrg.cpMin != tf.chrg.cpMax) // when there's a selection
      {
         if (bFindDown)
            tf.chrg.cpMin++;
         else
            // won't wraparound backwards
            tf.chrg.cpMin = std::max<long>(tf.chrg.cpMin, 0);
      }

      tf.chrg.cpMax = pT->GetTextLength() + m_nInitialSearchPos;

      if (bFindDown)
      {
         if (m_nInitialSearchPos >= 0)
            tf.chrg.cpMax = pT->GetTextLength();

         ATLASSERT(tf.chrg.cpMax >= tf.chrg.cpMin);
      }
      else
      {
         if (m_nInitialSearchPos >= 0)
            tf.chrg.cpMax = 0;

         ATLASSERT(tf.chrg.cpMax <= tf.chrg.cpMin);
      }

      int flags = bMatchCase ? SCFIND_MATCHCASE : 0;
      flags |= bWholeWord ? SCFIND_WHOLEWORD : 0;

      bool bRet = FindAndSelect(flags, tf);

      if (!bRet && m_nInitialSearchPos > 0)
      {
         // if the original starting point was not the beginning
         // of the buffer and we haven't already been here
         if (bFindDown)
         {
            tf.chrg.cpMin = 0;
            tf.chrg.cpMax = m_nInitialSearchPos;
         }
         else
         {
            tf.chrg.cpMin = pT->GetTextLength();
            tf.chrg.cpMax = m_nInitialSearchPos;
         }
         m_nInitialSearchPos = m_nInitialSearchPos - pT->GetTextLength();

         bRet = FindAndSelect(flags, tf);
      }

      return bRet ? TRUE : FALSE;
   }

   /// finds and selects next text
   bool FindAndSelect(int flags, TextToFind & tf)
   {
      T* pT = static_cast<T*>(this);

      unsigned int uiRet = pT->FindText(flags, tf);
      bool bRet = uiRet != (unsigned int)-1;

      if (bRet)
         pT->SetSel(tf.chrgText.cpMin, tf.chrgText.cpMax);

      return bRet;
   }

private:
   BEGIN_MSG_MAP(ThisClass)
   ALT_MSG_MAP(1)
      COMMAND_ID_HANDLER(ID_EDIT_CUT, OnEditCommand)
      COMMAND_ID_HANDLER(ID_EDIT_COPY, OnEditCommand)
      COMMAND_ID_HANDLER(ID_EDIT_PASTE, OnEditCommand)
      COMMAND_ID_HANDLER(ID_EDIT_SELECT_ALL, OnEditCommand)
      COMMAND_ID_HANDLER(ID_EDIT_UNDO, OnEditCommand)
      COMMAND_ID_HANDLER(ID_EDIT_REDO, OnEditCommand)
      CHAIN_MSG_MAP_ALT(BaseClass, 1)
   END_MSG_MAP()

   /// called for edit commands
   LRESULT OnEditCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& bHandled)
   {
      if (m_pFindReplaceDialog == nullptr)
      {
         bHandled = false;
         return 0;
      }

      HWND hWndFocus = ::GetFocus();
      if (!m_pFindReplaceDialog->IsChild(hWndFocus))
      {
         bHandled = false;
         return 0;
      }

      CWindow wnd(hWndFocus);
      switch (wID)
      {
      case ID_EDIT_CUT:    wnd.SendMessage(WM_CUT); break;
      case ID_EDIT_COPY:   wnd.SendMessage(WM_COPY); break;
      case ID_EDIT_PASTE:  wnd.SendMessage(WM_PASTE); break;
      case ID_EDIT_SELECT_ALL:   wnd.SendMessage(EM_SETSEL, 0, -1); break;
      case ID_EDIT_UNDO:   wnd.SendMessage(EM_UNDO); break;
      case ID_EDIT_REDO:   wnd.SendMessage(EM_REDO); break;
      }

      return 0;
   }

};

/// Lua script editor view
class LuaScriptEditorView :
   public CWindowImpl<LuaScriptEditorView, ScintillaWindowAdapter>,
   public ScintillaEditCommands<LuaScriptEditorView>,
   public ScintillaFindReplaceImpl<LuaScriptEditorView>
{
   /// base class type
   typedef ScintillaWindowAdapter BaseClass;

   // edit commands mixin type
   typedef ScintillaEditCommands<LuaScriptEditorView> EditCommandsClass;

   /// find/replace impl type
   typedef ScintillaFindReplaceImpl<LuaScriptEditorView> FindReplaceClass;

public:
   /// ctor
   LuaScriptEditorView() throw() {}

   /// pre-translates messages
   BOOL PreTranslateMessage(MSG* pMsg);

   /// window class infos
   DECLARE_WND_SUPERCLASS(NULL, CScintillaWindow::GetWndClassName())

   /// sets up source editor
   void SetupSourceEditor();

   /// returns file path of edited file
   CString GetFilePath() const { return m_cszFilePath; }

   /// inits editor with empty file
   void Init(LPCTSTR lpstrFilePath, LPCTSTR lpstrFileTitle);

   /// executes "File | Save as" command
   bool DoFileSaveAs();

   /// queries user to save the current file, when modified
   bool QueryClose();

   /// returns if there's a text selection in the editor
   bool IsTextSelected() const
   {
      return GetSelectionStart() < GetSelectionEnd();
   }

private:
   BEGIN_MSG_MAP(LuaScriptEditorView)
      CHAIN_MSG_MAP_ALT(FindReplaceClass, 1)
      CHAIN_MSG_MAP_ALT(EditCommandsClass, 1)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
// LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
   // model

   /// full file path and file name of the currently open file
   CString m_cszFilePath;

   /// file title (e.g. file name only)
   CString m_cszFileTitle;
};
