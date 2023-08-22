#include "ChatPanel.h"
#include "Model/IUser.h"

#include "imgui.h"
#include <misc/cpp/imgui_stdlib.h>


std::string formatMilliseconds(std::chrono::milliseconds ms)
{
    // Convert epoch time to a time_point
    std::chrono::system_clock::time_point timePoint = std::chrono::time_point<std::chrono::system_clock>(ms);

    // Convert time_point to a time structure (tm)
    std::time_t rawTime = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* timeInfo = std::localtime(&rawTime);

    // Format the time structure to a string
    std::stringstream buffer;
    buffer << std::put_time(timeInfo, "%H:%M:%S");

    return buffer.str();
}

ChatPanel::ChatPanel(IChatController& controller_) : controller(controller_) {}

void ChatPanel::Update()
{
    ChatViewModel vm = controller.GetViewModel();

    ImGui::SameLine();

//    ImGui::BeginDisabled(vm.chatHistory == nullptr);

    ImGui::BeginChild("Chat", ImVec2(ImGui::GetContentRegionAvail().x, 0), true, ImGuiWindowFlags_AlwaysAutoResize);

    console.Draw("Demo chat", &consoleOpen);

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
                                           ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

    const ChatHistory* chatHistory = vm.chatHistory;
    if (chatHistory != nullptr && !chatHistory->empty())
    {
        //if(this->chatBuffers.get(selectedChat) != this->)
        const ChatMessage* chatMessage = &(*chatHistory)[0];

        auto addedEntryIt = this->chatBuffers.insert({selectedChat, *chatMessage});
        if(addedEntryIt.second)
        {
            console.AddLog("[%s][%s] %s", formatMilliseconds(addedEntryIt.first->second.timestamp).c_str(), addedEntryIt.first->second.senderId.c_str(), addedEntryIt.first->second.content.c_str());
        }

        ChatMessage lastChatMessage = this->chatBuffers[selectedChat];

        auto chatHistoryCurrentIt = chatHistory->begin();
        auto chatHistoryEndIt = chatHistory->end();

        chatHistoryCurrentIt = std::find_if(chatHistoryCurrentIt, chatHistoryEndIt, [&lastChatMessage](const ChatMessage& chatMessage){return chatMessage.content == lastChatMessage.content && chatMessage.timestamp == lastChatMessage.timestamp;});

        for (++chatHistoryCurrentIt; chatHistoryCurrentIt != chatHistoryEndIt; ++chatHistoryCurrentIt)
        {
            console.AddLog("[%s][%s] %s", formatMilliseconds(chatHistoryCurrentIt->timestamp).c_str(), chatHistoryCurrentIt->senderId.c_str(), chatHistoryCurrentIt->content.c_str());
            this->chatBuffers[selectedChat] = *chatHistoryCurrentIt;
        }
    }

    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), input_text_flags,
                         &ExampleAppConsole::TextEditCallbackStub, (void*) &console))
    {
        char* s = inputBuf;
        Strtrim(s);
        if (s[0])
        {
            this->controller.SendMessage(inputBuf);
            // duplicate code, pls clean up
//            auto result = historyMap.insert({selectedChat, {}});
//            result.first->second.history.push_back(Strdup(std::format("[{}] {}", vm.userName, s).c_str()));
//
//            console.AddLog("[%s] %s", vm.userName.c_str(), s);
            //chatClient.SendMessageToPeer(selectedChat, s);
        }
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1);// Auto focus previous widget

    ImGui::EndChild();

//    ImGui::EndDisabled();

}

void ChatPanel::Draw()
{
    Update();

    if (IsVisible())
    {
        // Draw the chat panel
    }
}

bool ChatPanel::IsVisible()
{
    return false;
}
