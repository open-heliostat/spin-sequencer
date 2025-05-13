#ifndef REMOTES_H
#define REMOTES_H
#include <Arduino.h>
#include <vector>

struct Remote
{
    String hostname = "";
    String ip = "";
    uint32_t rxId = 99;
};

class RemotesController
{
public:
    bool isMaster = false;
    std::vector<Remote> remotes;
    RemotesController() {}
    RemotesController(std::vector<Remote> remotes) : remotes(remotes) {}
    RemotesController(Remote remote) {
        remotes.push_back(remote);
    }

    void addRemote(Remote remote) {
        remotes.push_back(remote);
    }
    void addRemote(String hostname, String ip, uint32_t rxId) {
        Remote remote;
        remote.hostname = hostname;
        remote.ip = ip;
        remote.rxId = rxId;
        remotes.push_back(remote);
    }
    void addRemote(String hostname, uint32_t rxId) {
        Remote remote;
        remote.hostname = hostname;
        remote.rxId = rxId;
        remotes.push_back(remote);
    }
    void addRemote(String hostname) {
        Remote remote;
        remote.hostname = hostname;
        remotes.push_back(remote);
    }
    void addRemote(uint32_t rxId) {
        // check if rxId is already in the list
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->rxId == rxId) {
                return; // rxId already exists, do not add
            }
        }
        Remote remote;
        remote.rxId = rxId;
        remotes.push_back(remote);
    }
    void removeRemote(Remote remote) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->hostname == remote.hostname && it->ip == remote.ip && it->rxId == remote.rxId) {
                remotes.erase(it);
                break;
            }
        }
    }
    void removeRemote(String hostname) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->hostname == hostname) {
                remotes.erase(it);
                break;
            }
        }
    }
    void removeRemote(uint32_t rxId) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->rxId == rxId) {
                remotes.erase(it);
                break;
            }
        }
    }
    void removeRemote(int index) {
        if (index >= 0 && index < remotes.size()) {
            remotes.erase(remotes.begin() + index);
        }
    }
    void clearRemotes() {
        remotes.clear();
    }
    Remote getRemote(int index) {
        if (index >= 0 && index < remotes.size()) {
            return remotes[index];
        }
        return Remote();
    }
    Remote getRemote(String hostname) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->hostname == hostname) {
                return *it;
            }
        }
        return Remote();
    }
    Remote getRemote(uint32_t rxId) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->rxId == rxId) {
                return *it;
            }
        }
        return Remote();
    }
    bool isRemote(String hostname) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->hostname == hostname) {
                return true;
            }
        }
        return false;
    }
    bool isRemote(Remote remote) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->hostname == remote.hostname && it->ip == remote.ip && it->rxId == remote.rxId) {
                return true;
            }
        }
        return false;
    }
    bool isRemote(Remote remote, String hostname) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->hostname == hostname && it->rxId == remote.rxId) {
                return true;
            }
        }
        return false;
    }
    bool isRemote(uint32_t rxId) {
        for (auto it = remotes.begin(); it != remotes.end(); ++it) {
            if (it->rxId == rxId) {
                return true;
            }
        }
        return false;
    }
};

#endif