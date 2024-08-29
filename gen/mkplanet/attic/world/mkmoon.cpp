#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

void printHelp() {
    std::cout << "Usage: mkmoon [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -4 <IPv4 address>   Set IPv4 address\n";
    std::cout << "  -6 <IPv6 address>   Set IPv6 address\n";
    std::cout << "  -p <port>            Set Moon port (default: 9993)\n";
    std::cout << "  --help, -h           Show this help message\n";
}

int main(int argc, char *argv[]) {
    // Display help if no arguments are passed
    if (argc == 1) {
        printHelp();
        return 0;
    }

    std::string ipv4_address;
    std::string ipv6_address;
    int moon_port = 9993;

    // Parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "4:6:p:h")) != -1) {
        switch (opt) {
            case '4':
                ipv4_address = optarg;
                std::cout << "IPv4 address: " << ipv4_address << std::endl;
                break;
            case '6':
                ipv6_address = optarg;
                std::cout << "IPv6 address: " << ipv6_address << std::endl;
                break;
            case 'p':
                moon_port = std::stoi(optarg);
                std::cout << "Moon port: " << moon_port << std::endl;
                break;
            case 'h':
                printHelp();
                return 0;
            default:
                printHelp();
                return 1;
        }
    }

    // Check if at least one address is provided
    if (ipv4_address.empty() && ipv6_address.empty()) {
        std::cerr << "Please set IPv4 address or IPv6 address." << std::endl;
        return 1;
    }

    // Construct the stableEndpoints field for moon.json
    json stableEndpointsJson;
    if (ipv4_address.empty()) {
        stableEndpointsJson = json::array({ipv6_address + "/" + std::to_string(moon_port)});
    } else if (ipv6_address.empty()) {
        stableEndpointsJson = json::array({ipv4_address + "/" + std::to_string(moon_port)});
    } else {
        stableEndpointsJson = json::array({
            ipv4_address + "/" + std::to_string(moon_port),
            ipv6_address + "/" + std::to_string(moon_port)
        });
    }

    // Read the identity.public file
    std::ifstream identityFile("/var/lib/zerotier-one/identity.public");
    if (!identityFile) {
        std::cerr << "Failed to open /var/lib/zerotier-one/identity.public" << std::endl;
        return 1;
    }
    std::stringstream identityStream;
    identityStream << identityFile.rdbuf();
    std::string identityContent = identityStream.str();

    // Generate moon.json using identity.public
    std::ofstream moonFile("/var/lib/zerotier-one/moon.json");
    if (!moonFile) {
        std::cerr << "Failed to open /var/lib/zerotier-one/moon.json" << std::endl;
        return 1;
    }
    moonFile << identityContent;
    moonFile.close();

    // Update moon.json with stableEndpoints
    std::ifstream jsonFile("/var/lib/zerotier-one/moon.json");
    json root;
    jsonFile >> root;

    root["stableEndpoints"] = stableEndpointsJson;

    std::ofstream updatedMoonFile("/var/lib/zerotier-one/moon.json");
    if (!updatedMoonFile) {
        std::cerr << "Failed to open /var/lib/zerotier-one/moon.json for writing" << std::endl;
        return 1;
    }
    updatedMoonFile << root.dump(4);
    updatedMoonFile.close();

    // Generate the .moon file
    std::system("/usr/sbin/zerotier-idtool genmoon /var/lib/zerotier-one/moon.json > /dev/null");

    // Create moons.d directory if it doesn't exist
    std::system("mkdir -p /var/lib/zerotier-one/moons.d");

    // Move the generated .moon file to moons.d
    std::system("mv *.moon /var/lib/zerotier-one/moons.d/");

    // Output the Moon ID
    std::ifstream moonJsonFile("/var/lib/zerotier-one/moon.json");
    json moonRoot;
    moonJsonFile >> moonRoot;
    std::string moon_id = moonRoot["id"];
    std::cout << "Your ZeroTier moon ID is \033[0;31m" << moon_id << "\033[0m" << std::endl;

    return 0;
}
