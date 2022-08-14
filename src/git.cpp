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
#include <../headers/workspace.hpp>

const std::set<std::string> Git::ignoredDirs = {".", "..", ".git", ".gitAtHome", "build", "webserver"};
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
    ftree.insert(index.getFilePaths());

    Commit curCommit(
        ftree.createCommitTree(ftree.getRoot()),
        CommitMessage("lowLevelGod", "lowLevelGod", "initial commit !"),
        parents);
    
    curCommit.serialize("");
    // print root/parents
    if (parents.empty())
        std::cout << "[root - commit] ";
    else
        std::cout << "[master - " << parents[0].substr(0, 7) << "]"; 
    Utils::parseObjectFile(Utils::getPathFromHash(curCommit.getHash()));

    // update current commit
    Ref::updateHead(curCommit.getHash());
}

void Git::status()
{
    Workspace workspace; // this also loads index


    std::shared_ptr<Commit> headCommit;
    if (Ref::getHead() != "")
        headCommit = std::dynamic_pointer_cast<Commit>(
            Utils::parseObjectFile(
                Utils::getPathFromHash(
                    Ref::getHead()
                    )
            )
            );

    // for (auto x : Ref::headEntries)
    //     std::cout << x.first << std::endl;
    std::set<std::string> untracked = workspace.searchUntracked(".");
    workspace.detectChanges();
    workspace.collectDeletedHeadFiles();
    std::map<std::string, std::string> changedFiles = workspace.getChangedFiles();
    
    workspace.saveIndex();

    for (auto x : untracked)
        std::cout << "?? " << x << std::endl;
    for (auto x : changedFiles)
        std::cout << workspace.workspaceStatusToString(x.first) + " " + x.first << std::endl;
    
}

void Git::run()
{
    init();
    // Blob blob("test.txt");
    // std::cout << blob.getHash() << std::endl;
    // std::cout << Utils::getSHA1hash(std::vector<uint8_t>({'b','l','o','b',' ', '1','4','H','e','l','l','o',',',' ', 'W','o','r','l','d','5'})) << std::endl;
    // Index index;
    // index.add(std::vector<std::string>({
    //     "headers/tree.hpp"
    //     }));
    // index.add(Utils::listAllFiles("headers/blob.hpp"));
    // index.saveUpdates();
    // Git::commit();
    Git::status();
    // Index index;
    // index.add(Utils::listAllFiles("."));
    // index.saveUpdates();
    // FlatToTree ftree;
    // ftree.insert(Utils::listAllFiles("src"));
    // Tree tree = Git::createCommitTree(ftree.getRoot());
    // for (auto path : Utils::listAllFiles(gitDir + '/' + "objects"))
    //     Utils::parseObjectFile(path);
    // Git::commit();
    // Index index;
    // index.add(std::vector<std::string>({
    //     "test.txt"
    //     }));
    // index.add(Utils::listAllFiles("."));
    // index.saveUpdates();
}