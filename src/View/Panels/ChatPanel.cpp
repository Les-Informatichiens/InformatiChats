#include "ChatPanel.h"
#include "imgui.h"


ChatPanel::ChatPanel(IChatController &controller_) : controller(controller_) {}

void ChatPanel::Update()
{

    ChatViewModel vm = controller.GetViewModel();

    ImGui::SameLine();

    ImGui::BeginChild("Chat", ImVec2(ImGui::GetContentRegionAvail().x, 0), true, ImGuiWindowFlags_AlwaysAutoResize);

    console.Draw("Demo chat", &consoleOpen);

    // Command-line
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll |
                                           ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;

    if (ImGui::InputText("Input", inputBuf, IM_ARRAYSIZE(inputBuf), input_text_flags,
                         &ExampleAppConsole::TextEditCallbackStub, (void *) &console))
    {
        char *s = inputBuf;
        Strtrim(s);
        if (s[0])
        {
            // duplicate code, pls clean up
            auto result = historyMap.insert({selectedChat, {}});
            result.first->second.history.push_back(Strdup(std::format("[{}] {}", vm.userName, s).c_str()));

            console.AddLog("[%s] %s", vm.userName.c_str(), s);
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
