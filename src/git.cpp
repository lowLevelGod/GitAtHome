#include <../headers/git.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <iterator>
#include <iostream>
#include <fstream>
#include <sha1.hpp>
#include <blob.hpp>
#include <sys/dir.h>
#include <set>
#include <../headers/tree.hpp>
#include <../headers/commit.hpp>

std::set<std::string> ignoredDirs = {".", "..", ".git", ".gitAtHome"};

void Git::init()
{
    mkdir("./.gitAtHome", 0777);
    mkdir("./.gitAtHome/objects", 0777);
    mkdir("./.gitAtHome/refs", 0777);
}

const std::string Git::getSHA1hash(const std::vector<char> &content)
{
    std::string tmp;
    tmp.assign(content.begin(), content.end());

    char hex[SHA1_HEX_SIZE]; // temporary buffer for sha1 hash
    sha1(tmp.c_str())
        // finalize must be called, otherwise the hash is not valid
        // after that, no more bytes should be added
        .finalize()
        // print the hash in hexadecimal, 0-terminated
        .print_hex(hex);

    return std::string(hex);
}

const std::vector<char> Git::readBinaryFile(const std::string &fileName)
{
    std::vector<char> result;
    std::ifstream file(fileName, std::ios::binary);

    file.unsetf(std::ios::skipws); // for newlines
    // get file size
    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    result.reserve(fileSize);

    result.insert(result.begin(),
                  std::istream_iterator<char>(file),
                  std::istream_iterator<char>());

    // std::cerr << "Error: " << strerror(errno);
    file.close();
    // if (!file)
    //     std::cout << "Read failed!" << std::endl; // Prints it every time for some reason

    return result;
}

const std::vector<char> Git::decompressObject(const std::string &fileName, const size_t len)
{
    // read compressed blob from objects
    std::vector<char> compressed(Git::readBinaryFile(fileName));

    uLong ucompSize = len;
    uLong compSize = compressBound(ucompSize);
    std::vector<char> decompressedResult(ucompSize); // allocate enough size for output buffer
    // Inflate
    try
    {
        uncompress((Bytef *)decompressedResult.data(), &ucompSize, (Bytef *)compressed.data(), compSize);
    }
    catch (const std::exception &e)
    {
        // std::cerr << e.what() << '\n';
        std::cout << "Decompression failed!" << std::endl;
    }

    return decompressedResult;
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
                        100644, 
                        std::string(ent->d_name), 
                        Blob(std::string(ent->d_name))
                        ));
                }
            }
        }
        closedir(dir);

        Commit curCommit(
            Tree(treeEntries),
            CommitMessage("lowLevelGod", "lowLevelGod", "initial commit !"),
            std::vector<std::shared_ptr<Commit>>());
        
        curCommit.serialize("");
        curCommit.print();
        updateHead(curCommit.getHash());
    }
    else
    {
        /* could not open directory */
        perror("");
    }
}

void Git::updateHead(const std::string& commitHash)
{
    std::ofstream file(".gitAtHome/HEAD", std::ios::binary);
    file.write(commitHash.c_str(), commitHash.length());
    file.close();
    if (!file)
        std::cout << "Write failed!" << std::endl;
}

void Git::run()
{
    init();
    Git::commit();
}