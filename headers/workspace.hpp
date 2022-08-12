#pragma once
#include <index.hpp>

enum Workspace_status{
        WORKSPACE_DELETED,
        WORKSPACE_MODIFIED
    };

class Workspace
{
    private:
        Index index;
        std::map<std::string, struct stat> stats;
        std::map<std::string, Workspace_status> changedFiles;   
    public:
        const std::set<std::string> searchUntracked(const std::string&);
        bool trackableFile(const std::string&);
        void detectWorkspaceChanges();
        void checkIndexEntry(const IndexEntry&);
        const std::string workspaceStatusToString(const Workspace_status&);
        void saveIndex() { index.saveUpdates();}
        const std::map<std::string, Workspace_status>& getChangedFiles() const { return changedFiles;}
};