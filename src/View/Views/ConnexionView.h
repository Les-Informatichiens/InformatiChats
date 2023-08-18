//
// Created by Jean on 8/18/2023.
//

#ifndef INFORMATICHATS_CONNEXION_H
#define INFORMATICHATS_CONNEXION_H


#include "Controller/IConnexionController.h"
#include "IView.h"
#include <vector>

class ConnexionView : public IView
{
public:
    explicit ConnexionView(IConnexionController &controller);
    ConnexionView(std::vector<IPanel> &panels, IConnexionController &controller);
    void Draw() const override;

    void AddPanel(IPanel &panel) override;

private:
    static constexpr int maxNameLength{32};
    std::vector<std::reference_wrapper<IPanel>> panels;

    IConnexionController &controller;
};


#endif//INFORMATICHATS_CONNEXION_H
