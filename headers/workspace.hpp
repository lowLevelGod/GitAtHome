#pragma once
#include <index.hpp>

enum Workspace_status{
        WORKSPACE_DELETED,
        WORKSPACE_MODIFIED,
        INDEX_ADDED,
        INDEX_MODIFIED,
        INDEX_DELETED
    };

class Workspace
{
    private:
        Index index;
        std::map<std::string, struct stat> stats;
        std::map<std::string, std::string> changedFiles;   
    public:
        const std::set<std::string> searchUntracked(const std::string&);
        bool trackableFile(const std::string&);
        void detectChanges();
        void checkIndexEntryWorkspace(const IndexEntry&);
        void checkIndexEntryHEAD(const IndexEntry&);
        const std::string workspaceStatusToString(const std::string&);
        void collectDeletedHeadFiles();
        void saveIndex() { index.saveUpdates();}
        const std::map<std::string, std::string>& getChangedFiles() const { return changedFiles;}
};