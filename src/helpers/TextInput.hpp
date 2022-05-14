#pragma once

#include "../defines.hpp"

struct STextInput {
    wlr_text_input_v3* wlrInput = nullptr;

    wlr_surface* pSurface = nullptr;

    DYNLISTENER(InputEnable);
    DYNLISTENER(InputCommit);
    DYNLISTENER(InputDisable);
    DYNLISTENER(InputDestroy);

    bool operator==(const STextInput& b) const {
        return wlrInput == b.wlrInput;
    }
};