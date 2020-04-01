/////////////////////////////////////////////////////////////////////////////
// Name:        wx/animate.h
// Purpose:     wxAnimation and wxAnimationCtrl
// Author:      Julian Smart and Guillermo Rodriguez Garcia
// Modified by: Francesco Montorsi
// Created:     13/8/99
// Copyright:   (c) Julian Smart and Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_ANIMATE_H_
#define _WX_ANIMATE_H_

#include "wx/defs.h"

#if wxUSE_ANIMATIONCTRL

#include "wx/animdecod.h"
#include "wx/control.h"
#include "wx/timer.h"
#include "wx/bitmap.h"

class WXDLLIMPEXP_FWD_CORE wxAnimation;

extern WXDLLIMPEXP_DATA_CORE(wxAnimation) wxNullAnimation;
extern WXDLLIMPEXP_DATA_CORE(const char) wxAnimationCtrlNameStr[];


// ----------------------------------------------------------------------------
// wxAnimationImpl
// ----------------------------------------------------------------------------

enum wxAnimationImplType
{
    wxANIMATION_IMPL_TYPE_NATIVE,
    wxANIMATION_IMPL_TYPE_GENERIC
};

class WXDLLIMPEXP_CORE wxAnimationImpl : public wxObject, public wxRefCounter
{
public:
    wxAnimationImpl() {}

    virtual wxAnimationImplType GetImplType() = 0;

    virtual bool IsOk() const = 0;

    // can be -1
    virtual int GetDelay(unsigned int frame) const = 0;

    virtual unsigned int GetFrameCount() const = 0;
    virtual wxImage GetFrame(unsigned int frame) const = 0;
    virtual wxSize GetSize() const = 0;

    virtual bool LoadFile(const wxString& name,
                          wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

    // extended interface used only by the generic implementation of wxAnimationCtrl
    virtual wxPoint GetFramePosition(unsigned int frame) const = 0;
    virtual wxSize GetFrameSize(unsigned int frame) const = 0;
    virtual wxAnimationDisposal GetDisposalMethod(unsigned int frame) const = 0;
    virtual wxColour GetTransparentColour(unsigned int frame) const = 0;
    virtual wxColour GetBackgroundColour() const = 0;

protected:
    wxDECLARE_ABSTRACT_CLASS(wxAnimationImpl);
};


// ----------------------------------------------------------------------------
// wxAnimation
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxAnimation : public wxObject
{
public:
    wxAnimation(wxAnimationImplType implType = wxANIMATION_IMPL_TYPE_NATIVE);
    wxAnimation(const wxString &name, wxAnimationType type = wxANIMATION_TYPE_ANY,
                wxAnimationImplType implType = wxANIMATION_IMPL_TYPE_NATIVE);
    wxAnimation(const wxAnimation& other);

    wxAnimationImpl* GetImpl() const
        { return static_cast<wxAnimationImpl*>(m_refData); }

    bool IsOk() const
        { return GetImpl() && GetImpl()->IsOk(); }

    int GetDelay(unsigned int frame) const;
    unsigned int GetFrameCount() const;
    wxImage GetFrame(unsigned int frame);
    wxSize GetSize() const;

    bool LoadFile(const wxString& name, wxAnimationType type = wxANIMATION_TYPE_ANY);
    bool Load(wxInputStream& stream, wxAnimationType type = wxANIMATION_TYPE_ANY);

    // extended interface used only by wxGenericAnimationCtrl
    wxPoint GetFramePosition(unsigned int frame) const;
    wxSize GetFrameSize(unsigned int frame) const;
    wxAnimationDisposal GetDisposalMethod(unsigned int frame) const;
    wxColour GetTransparentColour(unsigned int frame) const;
    wxColour GetBackgroundColour() const;

protected:
    wxDECLARE_DYNAMIC_CLASS(wxAnimation);
};


// ----------------------------------------------------------------------------
// wxAnimationCtrlBase
// ----------------------------------------------------------------------------

// do not autoresize to the animation's size when SetAnimation() is called
#define wxAC_NO_AUTORESIZE       (0x0010)

// default style does not include wxAC_NO_AUTORESIZE, that is, the control
// auto-resizes by default to fit the new animation when SetAnimation() is called
#define wxAC_DEFAULT_STYLE       (wxBORDER_NONE)

class WXDLLIMPEXP_CORE wxAnimationCtrlBase : public wxControl
{
public:
    wxAnimationCtrlBase() { }

    // public API
    virtual bool LoadFile(const wxString& filename,
                          wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;
    virtual bool Load(wxInputStream& stream,
                      wxAnimationType type = wxANIMATION_TYPE_ANY) = 0;

    virtual void SetAnimation(const wxAnimation &anim) = 0;
    virtual wxAnimation GetAnimation() const = 0;

    virtual bool Play() = 0;
    virtual void Stop() = 0;

    virtual bool IsPlaying() const = 0;

    virtual void SetInactiveBitmap(const wxBitmap &bmp);

    // always return the original bitmap set in this control
    wxBitmap GetInactiveBitmap() const
        { return m_bmpStatic; }

protected:
    // the inactive bitmap as it was set by the user
    wxBitmap m_bmpStatic;

    // the inactive bitmap currently shown in the control
    // (may differ in the size from m_bmpStatic)
    wxBitmap m_bmpStaticReal;

    // updates m_bmpStaticReal from m_bmpStatic if needed
    virtual void UpdateStaticImage();

    // called by SetInactiveBitmap
    virtual void DisplayStaticImage() = 0;

private:
    wxDECLARE_ABSTRACT_CLASS(wxAnimationCtrlBase);
};


// ----------------------------------------------------------------------------
// include the platform-specific version of the wxAnimationCtrl class
// ----------------------------------------------------------------------------

#include "wx/generic/animate.h"

#if defined(__WXGTK20__)
    #include "wx/gtk/animate.h"
#else
    class WXDLLIMPEXP_ADV wxAnimationCtrl : public wxGenericAnimationCtrl
    {
    public:
        wxAnimationCtrl()
            : wxGenericAnimationCtrl()
            {}
        wxAnimationCtrl(wxWindow *parent,
                        wxWindowID id,
                        const wxAnimation& anim = wxNullAnimation,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = wxAC_DEFAULT_STYLE,
                        const wxString& name = wxAnimationCtrlNameStr)
            : wxGenericAnimationCtrl(parent, id, anim, pos, size, style, name)
            {}

    bool Create(wxWindow *parent, wxWindowID id,
                const wxAnimation& anim = wxNullAnimation,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxAC_DEFAULT_STYLE,
                const wxString& name = wxAnimationCtrlNameStr)
        { return wxGenericAnimationCtrl::Create(parent, id, anim, pos, size, style, name); }

    static wxAnimationImpl* CreateAnimationImpl(wxAnimationImplType WXUNUSED(implType))
        {
            // For the generic widget we always use the generic impl and ignore the given type
            return new wxAnimationGenericImpl();
        }

    private:
        wxDECLARE_DYNAMIC_CLASS(wxAnimationCtrl);
    };
#endif // defined(__WXGTK20__)

#endif // wxUSE_ANIMATIONCTRL

#endif // _WX_ANIMATE_H_
