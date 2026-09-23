#pragma once
#include <string>

class VehicleState {
public:
    std::string handle_command(const std::string& command, const std::string& /*payload*/) {
        if (command == "lock") {
            locked_ = true;
            return "{\"command\":\"lock\",\"status\":\"ok\",\"locked\":true}";
        }
        if (command == "unlock") {
            locked_ = false;
            return "{\"command\":\"unlock\",\"status\":\"ok\",\"locked\":false}";
        }
        return "{\"command\":\"" + command + "\",\"status\":\"unknown_command\"}";
    }

private:
    bool locked_ = true;
};