#include "VideoPanel.h"

void VideoPanel::Update() {

}

void VideoPanel::Draw() {
    Update();

    if (IsVisible()) {
        // Draw the video panel
    }
}

bool VideoPanel::IsVisible() {
    return false;
}
