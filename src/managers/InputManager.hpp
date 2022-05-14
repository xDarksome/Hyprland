#pragma once

#include "../defines.hpp"
#include <list>
#include "../helpers/WLClasses.hpp"
#include "../helpers/TextInput.hpp"
#include "../Window.hpp"

class CInputManager {
public:

    void            onMouseMoved(wlr_pointer_motion_event*);
    void            onMouseWarp(wlr_pointer_motion_absolute_event*);
    void            onMouseButton(wlr_pointer_button_event*);
    void            onKeyboardKey(wlr_keyboard_key_event*, SKeyboard*);
    void            onKeyboardMod(void*, SKeyboard*);

    void            newKeyboard(wlr_input_device*);
    void            newMouse(wlr_input_device*);
    void            destroyKeyboard(SKeyboard*);
    void            destroyMouse(wlr_input_device*);
    SKeyboard*      getActiveKeyboard();

    void            constrainMouse(SMouse*, wlr_pointer_constraint_v1*);
    void            recheckConstraint(SMouse*);

    Vector2D        getMouseCoordsInternal();
    void            refocus();

    void            setKeyboardLayout();

    void            updateDragIcon();

    // IME and text input
    void            createTextInput(wlr_text_input_v3*);
    void            destroyTextInput(STextInput*);
    STextInput*     getFocusedTextInput();
    void            textInputSetFocus(STextInput*, wlr_surface*);
    void            textInputSetFocusAll(wlr_surface*);
    void            sendIMState(wlr_text_input_v3*);


    // for dragging floating windows
    CWindow*        currentlyDraggedWindow = nullptr;
    int             dragButton = -1;

    SDrag           m_sDrag;

    std::list<SConstraint> m_lConstraints;

private:

    std::list<SKeyboard>    m_lKeyboards;
    std::list<SMouse>       m_lMice;
    std::list<STextInput>   m_lTextInputs;

    SKeyboard*              m_pActiveKeyboard = nullptr;

    void            mouseMoveUnified(uint32_t, bool refocus = false);
    
};

inline std::unique_ptr<CInputManager> g_pInputManager;