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
#include <../headers/flatToTree.hpp>

const std::set<std::string> Git::ignoredDirs = {".", "..", ".git", ".gitAtHome", "webserver", "build"};
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
    Index index;
    // get commit hash at HEAD
    std::string head = Ref::getHead();
    std::vector<std::string> parents;
    if (head != "")
        parents.push_back(head);

    FlatToTree ftree;
    ftree.flatToTree(index.getFilePaths());

    Commit curCommit(
        Git::createCommitTree(ftree.getRoot()),
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

const Tree Git::createCommitTree(const std::shared_ptr<Node>& currentNode)
{
    std::vector<TreeEntry> treeEntries;
    for (auto child : currentNode->children)
    {
        std::string absolutePath = "";
        if (currentNode->name != "")
            absolutePath = currentNode->name + "/";
        absolutePath += child->name;
        // std::cout << child->name << std::endl;
        if (child->children.empty()) // if it has no children, then it's a blob
        {
            treeEntries.push_back(TreeEntry(
                child->name,
                Blob(absolutePath)
            ));
        }else // it's a tree
        {
            treeEntries.push_back(TreeEntry(
                child->name,
                Tree(createCommitTree(child))
            ));
        }
    }

    Tree tempTree(treeEntries);
    tempTree.serialize("");

    return tempTree;
}

void Git::run()
{
    init();
    Git::commit();
    // Index index;
    // index.add(std::vector<std::string>({
    //     "test.txt"
    //     }));
    // index.add(Utils::listAllFiles("."));
    // index.saveUpdates();
}