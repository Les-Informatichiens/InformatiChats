#include "ChatPanel.h"
#include "imgui.h"

ChatPanel::ChatPanel(Chat &chatClient) : chatClient(chatClient){}

void ChatPanel::Update() {
    ImGui::SameLine();

    ImGui::BeginChild("Chat", ImVec2(ImGui::GetContentRegionAvail().x, 0), true, ImGuiWindowFlags_AlwaysAutoResize);

    console.Draw("Demo chat", &consoleOpen);

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
    char InputBuf[256]{};
    if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &ExampleAppConsole::TextEditCallbackStub, (void*)&console))
    {
        char* s = InputBuf;
        Strtrim(s);
        if (s[0])
        {
            // duplicate code, pls clean up
            auto result = historyMap.insert({ selectedChat, {} });
            result.first->second.history.push_back(Strdup(std::format("[{}] {}", chatClient.GetUsername(), s).c_str()));

            console.AddLog("[%s] %s", chatClient.GetUsername().c_str(), s);
            chatClient.SendMessageToPeer(selectedChat, s);
        }
        strcpy(s, "");
        reclaim_focus = true;
    }

    // Auto-focus on window apparition
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::EndChild();
}

void ChatPanel::Draw() {
    Update();

    if (IsVisible()) {
        // Draw the chat panel
    }
}

bool ChatPanel::IsVisible() {
    return false;
}

