#include "Events.hpp"

#include "../Compositor.hpp"
#include "../helpers/WLClasses.hpp"
#include "../managers/InputManager.hpp"
#include "../render/Renderer.hpp"
#include "../helpers/TextInput.hpp"

void Events::listener_TextInput(wl_listener* listener, void* data) {
    const auto WLRTEXTINPUT = (wlr_text_input_v3*)data;

    g_pInputManager->createTextInput(WLRTEXTINPUT);
}

void Events::listener_NewIME(wl_listener* listener, void* data) {
    const auto WLRIME = (wlr_input_method_v2*)data;

    Debug::log(LOG, "Creating new IME at %x", WLRIME);

    if (g_pCompositor->m_sSeat.pCurrentIME) {
        Debug::log(ERR, "Cannot create a new IME, one is already active!");
        return;
    }

    g_pCompositor->m_sSeat.IME.hyprListener_IMECommit.initCallback(&WLRIME->events.commit, &Events::listener_IMECommit, &g_pCompositor->m_sSeat.IME, "IME");
    g_pCompositor->m_sSeat.IME.hyprListener_IMEGrab.initCallback(&WLRIME->events.grab_keyboard, &Events::listener_IMEGrab, &g_pCompositor->m_sSeat.IME, "IME");
    g_pCompositor->m_sSeat.IME.hyprListener_IMEDestroy.initCallback(&WLRIME->events.destroy, &Events::listener_IMEDestroy, &g_pCompositor->m_sSeat.IME, "IME");

    g_pCompositor->m_sSeat.pCurrentIME = WLRIME;
}

void Events::listener_IMECommit(void* owner, void* data) {
    const auto IME = g_pCompositor->m_sSeat.pCurrentIME;
    
    const auto TEXTINPUT = g_pInputManager->getFocusedTextInput();

    ASSERT(IME);

    if (IME->current.preedit.text)
        wlr_text_input_v3_send_preedit_string(TEXTINPUT->wlrInput, IME->current.preedit.text, IME->current.preedit.cursor_begin, IME->current.preedit.cursor_end);

    if (IME->current.commit_text)
        wlr_text_input_v3_send_commit_string(TEXTINPUT->wlrInput, IME->current.commit_text);

    if (IME->current.delete_.before_length || IME->current.delete_.after_length)
        wlr_text_input_v3_send_delete_surrounding_text(TEXTINPUT->wlrInput, IME->current.delete_.before_length, IME->current.delete_.after_length);

    wlr_text_input_v3_send_done(TEXTINPUT->wlrInput);
}

void Events::listener_IMEGrab(void* owner, void* data) {
    const auto WLRKBGRAB = (wlr_input_method_keyboard_grab_v2*)data;

    const auto PKEYBOARD = g_pInputManager->getActiveKeyboard();

    if (!PKEYBOARD) {
        Debug::log(ERR, "IME Cannot grab, no active keyboard!");
        return;
    }

    wlr_input_method_keyboard_grab_v2_set_keyboard(WLRKBGRAB, PKEYBOARD->keyboard->keyboard);

    Debug::log(LOG, "IME grabbed keyboard %x", PKEYBOARD);

    g_pCompositor->m_sSeat.IME.hyprListener_IMEDestroyGrab.initCallback(&WLRKBGRAB->events.destroy, Events::listener_IMEDestroyGrab, &g_pCompositor->m_sSeat.IME, "IME");
}

void Events::listener_IMEDestroyGrab(void* owner, void* data) {
    //const auto WLRKBGRAB = (wlr_input_method_keyboard_grab_v2*)data;

    Debug::log(LOG, "IME destroyed grab");

    g_pCompositor->m_sSeat.IME.hyprListener_IMEDestroyGrab.removeCallback();
}

void Events::listener_IMEDestroy(void* owner, void* data) {
    //const auto IME = (wlr_input_method_v2*)data;

    g_pCompositor->m_sSeat.pCurrentIME = nullptr;

    const auto TEXTINPUT = g_pInputManager->getFocusedTextInput();

    if (TEXTINPUT) {
        wlr_text_input_v3_send_leave(TEXTINPUT->wlrInput);
    }
}

