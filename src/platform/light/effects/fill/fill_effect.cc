#include "fill_effect.h"

const uint8_t kGlowSize = 10;
const size_t kFillTransitionDuration = 1500;

bool FillEffect::handleFrame(LightState* state, Pixels* pixels) {
    if (state->currentColor == state->targetColor) {
        return false;
    }

    if (progress_.finished()) {
        state->currentColor = state->targetColor;
    }

    uint8_t fillSize = scale8(center_, progress_.get());
    for (uint8_t i = 0; i < fillSize; i++) {
        pixels->set(state->targetColor, center_ - i);
    }
    // for (uint8_t i = 0; i < kGlowSize; i++) {
    //     CRGB glowPixelColor = blend(
    //         state->currentColor,
    //         state->previousColor,
    //         i * 255 / kGlowSize
    //     );
    //     if (fillSize + i >= center_) {
    //         continue;
    //     }

    //     pixels->set(glowPixelColor, center_ - fillSize - i);
    // }
    pixels->mirror();

    return true;
}

void FillEffect::onActivate(LightState* state, Pixels* pixels) {
    center_ = pixels->length() / 2;
    progress_.start(kFillTransitionDuration);
}

void FillEffect::onColorUpdate(LightState* state) {
    state->selectedColor = state->targetColor;
    progress_.start(kFillTransitionDuration);
}

FillEffect FillFx;
