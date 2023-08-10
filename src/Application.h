//
// Created by Jonathan Richard on 2023-08-09.
//

#ifndef INFORMATICHATS_APPLICATION_H
#define INFORMATICHATS_APPLICATION_H

#include "Chat.h"

#define GLFW_INCLUDE_NONE // without this, glfw3 includes gl.h which causes an error if included before glew.h
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

class Application
{
public:
    // TODO: add a constructor

    bool Init();
    void Update();
    void Uninit();

private:
    GLFWwindow* window;

    Chat chatClient;
    std::string selectedChat;
    bool addNewChatPrompt;

    // chat message space
    ExampleAppConsole console;
    bool consoleOpen;

    struct PeerData
    {
        ImVector<char*> history;
        size_t unreadMessageCount;
    };
    // chat histories
    std::unordered_map<std::string, PeerData> historyMap;

    bool showDemoWindow;
};


#endif //INFORMATICHATS_APPLICATION_H
