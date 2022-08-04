#include <../headers/git.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <blob.hpp>
#include <sys/dir.h>
#include <set>
#include <../headers/tree.hpp>
#include <../headers/commit.hpp>
#include <../headers/ref.hpp>
#include <../headers/utils.hpp>

std::set<std::string> ignoredDirs = {".", "..", ".git", ".gitAtHome"};

void Git::init()
{
    mkdir("./.gitAtHome", 0777);
    mkdir("./.gitAtHome/objects", 0777);
    mkdir("./.gitAtHome/refs", 0777);
}

void Git::commit()
{
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(".")) != NULL)
    {
        std::vector<TreeEntry> treeEntries;
        /* for now we commit all files in current directory */
        while ((ent = readdir(dir)) != NULL)
        {
            if (ignoredDirs.find(std::string(ent->d_name)) == ignoredDirs.end())
            {
                // just regular files for now!
                if (ent->d_type == DT_REG)
                {
                    treeEntries.push_back(TreeEntry( 
                        std::string(ent->d_name), 
                        Blob(std::string(ent->d_name))
                        ));
                }
            }
        }
        closedir(dir);

        // get commit hash at HEAD
        std::string head = Ref::getHead();
        std::vector<std::string> parents;
        if (head != "")
            parents.push_back(head);

        Commit curCommit(
            Tree(treeEntries),
            CommitMessage("lowLevelGod", "lowLevelGod", "initial commit !"),
            parents);
        
        curCommit.serialize("");
        // print root/parents
        if (parents.empty())
            std::cout << "[root - commit] ";
        else
            std::cout << "[master - " << parents[0].substr(0, 7) << "]"; 
        curCommit.print();

        // update current commit
        Ref::updateHead(curCommit.getHash());
    }
    else
    {
        /* could not open directory */
        perror("");
    }
}

void Git::run()
{
    init();
    Git::commit();
}