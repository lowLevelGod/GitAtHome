#include <../headers/workspace.hpp>
#include <../headers/ref.hpp>
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

void Workspace::detectChanges()
{
    for (auto entry : index.getEntries())
    {
        checkIndexEntryWorkspace(entry);
        checkIndexEntryHEAD(entry);
    }
}

void Workspace::checkIndexEntryWorkspace(const IndexEntry& entry)
{
    if (stats.find(entry.getFileName()) == stats.end())
    {
        if (changedFiles.find(entry.getFileName()) == changedFiles.end())
            changedFiles.insert({entry.getFileName(), std::to_string(WORKSPACE_DELETED)});
        else
            changedFiles[entry.getFileName()] += std::to_string(WORKSPACE_DELETED);
        return;
    }
    bool statCheck = entry.statMatch(stats[entry.getFileName()]);
    if (!statCheck)
    {
        if (changedFiles.find(entry.getFileName()) == changedFiles.end())
            changedFiles.insert({entry.getFileName(), std::to_string(WORKSPACE_MODIFIED)});
        else
            changedFiles[entry.getFileName()] += std::to_string(WORKSPACE_MODIFIED);
        return;
    }

    bool timesCheck = entry.timesMatch(stats[entry.getFileName()]);
    if (timesCheck)
        return;

    Blob blob(entry.getFileName());
    if (blob.getHash() != entry.getHash())
    {
        if (changedFiles.find(entry.getFileName()) == changedFiles.end())
            changedFiles.insert({entry.getFileName(), std::to_string(WORKSPACE_MODIFIED)});
        else
            changedFiles[entry.getFileName()] += std::to_string(WORKSPACE_MODIFIED);
        return;
    }else
    {
        index.updateEntryStat(entry.getFileName(), stats[entry.getFileName()]);
        return;
    }
}

void Workspace::checkIndexEntryHEAD(const IndexEntry& entry)
{
    if (Ref::headEntries.find(entry.getFileName()) == Ref::headEntries.end())
    {
        if (changedFiles.find(entry.getFileName()) == changedFiles.end())
            changedFiles.insert({entry.getFileName(), std::to_string(INDEX_ADDED)});
        else
            changedFiles[entry.getFileName()] += std::to_string(INDEX_ADDED);
    }else
    {
        if (entry.getMode() != Ref::headEntries[entry.getFileName()].getMode() ||
        entry.getHash() != Ref::headEntries[entry.getFileName()].getHash())
        {
            if (changedFiles.find(entry.getFileName()) == changedFiles.end())
                changedFiles.insert({entry.getFileName(), std::to_string(INDEX_MODIFIED)});
            else
                changedFiles[entry.getFileName()] += std::to_string(INDEX_MODIFIED);
        }

    }
}

void Workspace::collectDeletedHeadFiles()
{
    for (auto x : Ref::headEntries)
        if (!index.isTrackedFile(x.first))
        {
            if (changedFiles.find(x.first) == changedFiles.end())
                changedFiles.insert({x.first, std::to_string(INDEX_DELETED)});
            else
                changedFiles[x.first] += std::to_string(INDEX_DELETED);
        }

}

const std::string Workspace::workspaceStatusToString(const std::string& path)
{
    std::string status = changedFiles[path];
    std::string left = " ";
    left = status.find(std::to_string(INDEX_ADDED)) != std::string::npos ? "A" : left;
    left = status.find(std::to_string(INDEX_MODIFIED)) != std::string::npos ? "M" : left;
    left = status.find(std::to_string(INDEX_DELETED)) != std::string::npos ? "D" : left;

    std::string right = " ";
    right = status.find(std::to_string(WORKSPACE_DELETED)) != std::string::npos ? "D" : right;
    right = status.find(std::to_string(WORKSPACE_MODIFIED)) != std::string::npos ? "M" : right;

    return left + right;    
}