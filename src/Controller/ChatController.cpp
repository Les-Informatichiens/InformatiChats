//
// Created by Jean on 8/18/2023.
//

#include "ChatController.h"


ChatViewModel ChatController::GetViewModel()
{
    return {userName};
}
ChatController::ChatController(Model &model_)
: model(model_)
{
}
