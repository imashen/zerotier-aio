#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <node/Constants.hpp>
#include <node/World.hpp>
#include <node/C25519.hpp>
#include <node/Identity.hpp>
#include <node/InetAddress.hpp>
#include <osdep/OSUtils.hpp>

using namespace ZeroTier;
using json = nlohmann::json;

void printHelp() {
    printf("Usage: mkplanet [options]\n");
    printf("Options:\n");
    printf("  -h, --help          Display this help message\n");
    printf("  -j2b, --json2bin      Convert from JSON file to planet\n");
    printf("  -b2j, --bin2json      Convert from planet to JSON format\n");
}

int jsonToBinary() {
    std::string previous, current;

    // Check if planet exists
    if (!OSUtils::fileExists("planet")) {
        // Check if origin-planet.json exists and copy it to planet.json
        if (OSUtils::fileExists("origin-planet.json")) {
            std::string originContent;
            if (OSUtils::readFile("origin-planet.json", originContent)) {
                if (!OSUtils::writeFile("planet.json", originContent)) {
                    fprintf(stderr, "FATAL: Failed to copy origin-planet.json to planet.json\n");
                    return 1;
                }
                fprintf(stderr, "INFO: Copied origin-planet.json to planet.json\n");
            } else {
                fprintf(stderr, "FATAL: Failed to read origin-planet.json\n");
                return 1;
            }
        } else {
            fprintf(stderr, "FATAL: origin-planet.json not found\n");
            return 1;
        }
    }

    if ((!OSUtils::readFile("previous.c25519", previous)) || (!OSUtils::readFile("current.c25519", current))) {
        C25519::Pair np(C25519::generate());
        previous = std::string();
        previous.append((const char*)np.pub.data, ZT_C25519_PUBLIC_KEY_LEN);
        previous.append((const char*)np.priv.data, ZT_C25519_PRIVATE_KEY_LEN);
        current = previous;
        OSUtils::writeFile("previous.c25519", previous);
        OSUtils::writeFile("current.c25519", current);
        fprintf(stderr, "INFO: created initial world keys: previous.c25519 and current.c25519 (both initially the same)\n");
    }

    if ((previous.length() != (ZT_C25519_PUBLIC_KEY_LEN + ZT_C25519_PRIVATE_KEY_LEN)) || (current.length() != (ZT_C25519_PUBLIC_KEY_LEN + ZT_C25519_PRIVATE_KEY_LEN))) {
        fprintf(stderr, "FATAL: previous.c25519 or current.c25519 empty or invalid\n");
        return 1;
    }
    C25519::Pair previousKP;
    memcpy(previousKP.pub.data, previous.data(), ZT_C25519_PUBLIC_KEY_LEN);
    memcpy(previousKP.priv.data, previous.data() + ZT_C25519_PUBLIC_KEY_LEN, ZT_C25519_PRIVATE_KEY_LEN);
    C25519::Pair currentKP;
    memcpy(currentKP.pub.data, current.data(), ZT_C25519_PUBLIC_KEY_LEN);
    memcpy(currentKP.priv.data, current.data() + ZT_C25519_PUBLIC_KEY_LEN, ZT_C25519_PRIVATE_KEY_LEN);

    std::string fileContent;
    if (!OSUtils::readFile("planet.json", fileContent)) {
        fprintf(stderr, "Failed to open planet.json file.\n");
        return 1;
    }

    json config = json::parse(fileContent);

    std::vector<World::Root> roots;
    for (auto& root : config["roots"]) {
        roots.push_back(World::Root());
        roots.back().identity = Identity(root["identity"].get<std::string>().c_str());
        for (auto& endpoint : root["stableEndpoints"]) {
            roots.back().stableEndpoints.push_back(InetAddress(endpoint.get<std::string>().c_str()));
        }
    }

    fprintf(stderr, "INFO: generating and signing world\n");

    World nw = World::make(World::TYPE_PLANET, ZT_WORLD_ID_EARTH, 1567191349589ULL, currentKP.pub, roots, previousKP);

    Buffer<ZT_WORLD_MAX_SERIALIZED_LENGTH> outtmp;
    nw.serialize(outtmp, false);
    OSUtils::writeFile("planet", std::string((const char*)outtmp.data(), outtmp.size()));
    fprintf(stderr, "INFO: planet written with %u bytes of binary world data.\n", outtmp.size());

    return 0;
}

void binaryToJson() {
    std::string binContent;
    if (!OSUtils::readFile("planet", binContent)) {
        fprintf(stderr, "Failed to open planet file.\n");
        return;
    }

    Buffer<ZT_WORLD_MAX_SERIALIZED_LENGTH> binBuffer(binContent.data(), binContent.size());
    World world;
    if (!world.deserialize(binBuffer, 0)) {
        fprintf(stderr, "Failed to deserialize planet content.\n");
        return;
    }

    json worldJson;
    json rootsJson;
    for (const auto& root : world.roots()) {
        json rootJson;
        char identityStr[ZT_IDENTITY_STRING_BUFFER_LENGTH];
        root.identity.toString(true, identityStr);
        rootJson["identity"] = identityStr;

        json stableEndpointsJson;
        for (const auto& endpoint : root.stableEndpoints) {
            char ipStr[64];
            endpoint.toString(ipStr);
            stableEndpointsJson.push_back(ipStr);
        }
        rootJson["stableEndpoints"] = stableEndpointsJson;

        rootsJson.push_back(rootJson);
    }
    worldJson["roots"] = rootsJson;
    std::string jsonStr = worldJson.dump(4);
    printf("World JSON:\n%s\n", jsonStr.c_str());
    if (!OSUtils::writeFile("planet.json", jsonStr.c_str(), jsonStr.size())) {
        fprintf(stderr, "Failed to write JSON data to planet.json.\n");
    } else {
        printf("JSON data successfully written to planet.json.\n");
    }
}

int main(int argc, char** argv) {
    // Change working directory to /var/lib/zerotier-one
    if (chdir("/var/lib/zerotier-one") != 0) {
        perror("Directory Error");
        return 1;
    }

    bool json2bin = false;
    bool bin2json = false;

    bool hasOption = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "-j2b" || arg == "--json2bin") {
            json2bin = true;
            hasOption = true;
        } else if (arg == "-b2j" || arg == "--bin2json") {
            bin2json = true;
            hasOption = true;
        }
    }

    if (!hasOption) {
        printHelp();
        return 0;
    }

    if (json2bin && bin2json) {
        printf("Error: Cannot specify both JSON to binary and binary to JSON conversion options.\n");
        printHelp();
        return 1;
    }

    if (json2bin) {
        return jsonToBinary();
    } else if (bin2json) {
        binaryToJson();
    }

    return 0;
}
