#include <../headers/workspace.hpp>
#include <iostream>

const std::set<std::string> Workspace::searchUntracked(const std::string& dirName)
{
    std::set<std::string> untracked;
    for (auto x : Utils::getStatsInDir(dirName))
    {
        // std::cout << x.first << std::endl;
        if (index.isTracked(x.first))
        {
            if (x.second.st_mode & S_IFDIR)
            {
                std::set<std::string> tmp = searchUntracked(x.first);
                untracked.insert(tmp.begin(), tmp.end());
            }else
            {
                stats[x.first] = x.second;
            }
        }else if (trackableFile(x.first))
        {
            untracked.insert(x.second.st_mode & S_IFDIR ? 
                (x.first + '/') :
                x.first);
        }
    }

    return untracked;
}

bool Workspace::trackableFile(const std::string& path)
{
    if (Utils::isDir(path))
    {
       std::map<std::string, struct stat> items = Utils::getStatsInDir(path);
       std::vector<std::string> files;
       std::vector<std::string> dirs;

       for (auto x : items)
       {
            if (x.second.st_mode & S_IFDIR)
                dirs.push_back(x.first);
            else
                files.push_back(x.first);
       }

       for (auto x : files)
            if (trackableFile(x))
                return true;
        for (auto x : dirs)
            if (trackableFile(x))
                return true;
    }else
    {
        return !index.isTracked(path);
    }

    return false;
}

void Workspace::detectWorkspaceChanges()
{
    for (auto entry : index.getEntries())
        checkIndexEntry(entry);
}

void Workspace::checkIndexEntry(const IndexEntry& entry)
{
    if (stats.find(entry.getFileName()) == stats.end())
    {
        changedFiles.insert({entry.getFileName(), WORKSPACE_DELETED});
        return;
    }
    bool statCheck = entry.statMatch(stats[entry.getFileName()]);
    if (!statCheck)
    {
        changedFiles.insert({entry.getFileName(), WORKSPACE_MODIFIED});
        return;
    }

    bool timesCheck = entry.timesMatch(stats[entry.getFileName()]);
    if (timesCheck)
        return;

    Blob blob(entry.getFileName());
    if (blob.getHash() != entry.getHash())
    {
        changedFiles.insert({entry.getFileName(), WORKSPACE_MODIFIED});
        return;
    }else
    {
        index.updateEntryStat(entry.getFileName(), stats[entry.getFileName()]);
        return;
    }
}

const std::string Workspace::workspaceStatusToString(const Workspace_status& s)
{
    if (s == WORKSPACE_MODIFIED)
        return " M";
    if (s == WORKSPACE_DELETED)
        return " D";
    return "  ";    
}