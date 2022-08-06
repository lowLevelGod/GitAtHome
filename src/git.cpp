#include <../headers/git.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <blob.hpp>
#include <sys/dir.h>
#include <../headers/tree.hpp>
#include <../headers/commit.hpp>
#include <../headers/ref.hpp>
#include <../headers/utils.hpp>
#include <../headers/index.hpp>
#include <fcntl.h>

const std::set<std::string> Git::ignoredDirs = {".", "..", ".git", ".gitAtHome", "webserver"};
const std::string Git::gitDir = "./.gitAtHome";

void Git::init()
{
    mkdir(Git::gitDir.c_str(), 0777);
    mkdir((Git::gitDir + "/" + "objects").c_str(), 0777);
    mkdir((Git::gitDir + "/" + "refs").c_str(), 0777);

    // create empty HEAD file for now
    int fd = open((Git::gitDir + "/" + "HEAD").c_str(), O_CREAT | O_EXCL, 0644);
    close(fd);
    // create empty index file for now
    fd = open((Git::gitDir + "/" + "index").c_str(), O_CREAT | O_EXCL, 0644);
    close(fd);
}

void Git::commit()
{
    // get commit hash at HEAD
    std::string head = Ref::getHead();
    std::vector<std::string> parents;
    if (head != "")
        parents.push_back(head);

    Commit curCommit(
        Git::createCommitTree("."),
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

const Tree Git::createCommitTree(const std::string& dirName)
{
    // std::cout << "Traversing : " << dirName << std::endl;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirName.c_str())) != NULL)
    {
        std::vector<TreeEntry> treeEntries;
        /* recurse in directory given by dirName */
        while ((ent = readdir(dir)) != NULL)
        {
            if (ignoredDirs.find(std::string(ent->d_name)) == ignoredDirs.end())
            {
                if (ent->d_type == DT_REG) // regular file = blob
                {
                    // std::cout << "File : " << dirName + "/" + ent->d_name << std::endl;
                    treeEntries.push_back(TreeEntry( 
                        std::string(ent->d_name), 
                        Blob(std::string(dirName + "/" + ent->d_name))
                        ));
                }
                else if (ent->d_type == DT_DIR) // directory = tree
                {
                    treeEntries.push_back(TreeEntry(
                        std::string(ent->d_name),
                        Tree(Git::createCommitTree(dirName + "/" + std::string(ent->d_name)))
                    ));
                }
            }
        }
        closedir(dir);
        Tree tempTree(treeEntries);
        tempTree.serialize("");

        return tempTree;
    }
    else
    {
        /* could not open directory */
        perror("");
        return Tree();
    }
}

void Git::run()
{
    init();
    // Git::commit();
    Index index;
    // index.add(std::vector<std::string>({
    //     "test.txt"
    //     }));
    index.add(Utils::listAllFiles("."));
    index.prepareSerialize();
    index.serialize("");
}