//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "IPanel.h"
#include "Controller/IConnexionController.h"

class ConnexionPanel : public IPanel{
public:
    explicit ConnexionPanel(IConnexionController& controller);

    void Draw() override;

    void Update() override;

    bool IsVisible() override;

private:
    IConnexionController& controller;

};
