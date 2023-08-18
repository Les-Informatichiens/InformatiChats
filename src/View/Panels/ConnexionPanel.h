//
// Created by Jean on 8/18/2023.
//

#pragma once

#include "Controller/IConnexionController.h"
#include "IPanel.h"

class ConnexionPanel : public IPanel
{
public:
    explicit ConnexionPanel(IConnexionController &controller);

    void Draw() override;

    void Update() override;

    bool IsVisible() override;

private:
    IConnexionController &controller;
};
