#include "Application.h"
// Main code
int main(int, char**)
{

    const std::string stunServer = "stun.l.google.com";
    const std::string stunServerPort = "19302";
    const std::string signalingServer = "51.79.86.30";
    const std::string signalingServerPort = "51337";
    ConnectionConfig config = { stunServer, stunServerPort, signalingServer, signalingServerPort };
    Chat chatClient(config);

    Application app(chatClient);
    app.Run();

    return 0;
}