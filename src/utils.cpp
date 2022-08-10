#include <../headers/utils.hpp>
#include <../headers/git.hpp>
#include <sha1.hpp>
#include <iostream>
#include <fstream>
#include <iterator>
#include <zlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sstream>
#include <sys/dir.h>
#include <dirent.h>
#include <algorithm>

const std::string Utils::getSHA1hash(const std::vector<char> &content)
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

const std::vector<char> Utils::readBinaryFile(const std::string &fileName)
{
    if (!fileExists(fileName))
    {
        std::cout << "File: " << fileName << " does not exist!" << std::endl;
        return std::vector<char>();
    }
    std::vector<char> result;
    std::ifstream file(fileName, std::ios::binary);

    file.unsetf(std::ios::skipws); // for newlines
    // get file size
    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // std::cout << "File: " << fileName << " has size " << fileSize << std::endl;

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

const std::vector<char> Utils::decompressBytes(const std::vector<char>& compressed, const size_t len)
{
    uLong ucompSize = len;
    uLong compSize = compressBound(compressed.size());
    std::vector<char> decompressedResult(ucompSize); // allocate enough size for output buffer
    int result = 0;
    // Inflate
    try
    {
        result = uncompress((Bytef *)decompressedResult.data(), &ucompSize, (Bytef *)compressed.data(), compSize);
    }
    catch (const std::exception &e)
    {
        // std::cerr << e.what() << '\n';
        std::cout << "Decompression failed!" << std::endl;
    }
   
    //     std::cout << result;
    // std::cout << std::endl;
    return decompressedResult;
}

const std::string Utils::getMode(const std::string& fileName)
{
    struct stat st = {0};

    std::string result;
    if (stat(fileName.c_str(), &st) != -1) { // check if file exists
        
        std::ostringstream str;
        str << std::oct << st.st_mode;
        std::string result = str.str();

        if (result[0] == '4')
            return "40000";
        else if (result.find('7') != std::string::npos)
            return "100755";
        else
            return "100644";
    }
    // std::cout << fileName << " " << Utils::fileExists(fileName) << std::endl;
    return result;
}

const std::vector<char> Utils::int32ToBytes(const int x)
{
    return std::vector<char>({
        static_cast<char>((x >> 24) & 0xff),
        static_cast<char>((x >> 16) & 0xff),
        static_cast<char>((x >> 8) & 0xff),
        static_cast<char>((x >> 0) & 0xff)
    });
}

const std::vector<char> Utils::packStringToPackedBytes(const std::string& s)
{
    std::vector<char> result;
    for (size_t i = 0, len = s.length(); i < len - 1; i += 2)
    {
        uint8_t b1 = 0, b2 = 0;
        if (s[i] >= '0' && s[i] <= '9')
            b1 = s[i] - '0';
        else
            b1 = s[i] - 'a' + 10;
        if (s[i + 1] >= '0' && s[i + 1] <= '9')
            b2 = s[i + 1] - '0';
        else
            b2 = s[i + 1] - 'a' + 10;
        
        result.push_back(
            ((b1  & 0xf) << 4)
            | (b2  & 0xf) 
        );          
    }

    return result;
}

void Utils::writeBinaryFile(const std::string& fileName, const std::vector<char>& v)
{
    std::ofstream file(fileName, std::ios::binary);
   
    file.write(v.data(), v.size());
    // std::cerr << fileName << " " << "Error: " << strerror(errno);
    file.close();
    if (!file)
        std::cout << "Write failed!" << std::endl;
}

const std::vector<std::string> Utils::listAllFiles(const std::string& dirName)
{
    std::vector<std::string> files;
    // std::cout << "Traversing : " << dirName << std::endl;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirName.c_str())) != NULL)
    {
        /* recurse in directory given by dirName */
        while ((ent = readdir(dir)) != NULL)
        {
            if (Git::ignoredDirs.find(std::string(ent->d_name)) == Git::ignoredDirs.end())
            {
                if (ent->d_type == DT_REG) // regular file = blob
                {
                    // std::cout << "File : " << dirName + "/" + ent->d_name << std::endl;
                    std::string pushedFile = std::string(dirName + "/" + ent->d_name);
                    if (pushedFile.length() > 1 && 
                    pushedFile[0] == '.' && 
                    pushedFile [1] == '/')
                        pushedFile = pushedFile.substr(2);

                    files.push_back(pushedFile);
                }
                else if (ent->d_type == DT_DIR) // directory = tree
                {
                    std::vector<std::string> tempFiles = Utils::listAllFiles(dirName + "/" + std::string(ent->d_name));
                    files.insert(files.end(), tempFiles.begin(), tempFiles.end());
                }
            }
        }
        closedir(dir);
    }
    else
    {
        /* could not open directory */
        perror("");
    }

    return files;
}

const uint32_t Utils::bytesToInt32(const std::vector<char>& v)
{
    return (v[0] << 24) | (v[1] << 16)
             | (v[2] << 8) | (v[3] << 0);
}

const std::string Utils::unpackBytesToString(const std::vector<char>& v)
{
    const std::string hexChars = "0123456789abcdef";
    std::string result = "";

    for (auto b : v)
    {
        result += hexChars[static_cast<size_t>((b >> 4) & 0xf)];
        result += hexChars[static_cast<size_t>((b >> 0) & 0xf)];
    }

    return result;
}

const bool Utils::fileExists(const std::string& fileName)
{
    struct stat st = {0};

    return (stat(fileName.c_str(), &st) != -1);
}

const std::vector<std::string> Utils::splitPath(std::string path)
{
    std::vector<std::string> result;
    size_t pos = 0;
    while ((pos = path.find('/')) != std::string::npos)
    {
        result.push_back(path.substr(0, pos));
        path = path.substr(pos + 1);
    }
    
    result.push_back(path);

    return result;
}

const std::shared_ptr<Object> Utils::parseObjectFile(const std::string& path)
{
    std::vector<char> file = Utils::readBinaryFile(path);
    // 30 characters should be enough to get actual size of object
    std::vector<char> header = Utils::decompressBytes(file, 30);

    std::vector<char>::iterator firstSpace = std::find(header.begin(), header.end(), ' ');

    std::string objectType(header.begin(), firstSpace);
    std::vector<char>::iterator firstNull = std::find(firstSpace, header.end(), '\0');
    // std::cout << "File : " << path << std::endl;
    // for (auto x : header)
    //     std::cout << x;
    // std::cout << std::endl;
    std::string objectSize = std::string(firstSpace + 1, firstNull);
    // std::cout << "OBJ SIZE " << objectSize << std::endl;

    // save decompressed content
    file = Utils::decompressBytes(file, std::stoi(objectSize, 0, 10) + objectType.length() + objectSize.length() + 2); // not sure why + 5
    if (objectType == "tree")
    {
        return Utils::parseTree(std::vector(
            std::find(file.begin(), file.end(), '\0') + 1, file.end()
            ));
    }else if (objectType == "commit")
    {
        return Utils::parseCommit(std::vector(
            std::find(file.begin(), file.end(), '\0') + 1, file.end()
            ));
    }else if (objectType == "blob")
    {
        return Utils::parseBlob(std::vector(
            std::find(file.begin(), file.end(), '\0') + 1, file.end()
            ));
    }

    return nullptr;
}

const std::shared_ptr<Blob> Utils::parseBlob(const std::vector<char>& file)
{
    // for (auto x : file)
    //     std::cout << x;
    // std::cout << std::endl;
    
    return std::make_shared<Blob>(file);
}

const std::shared_ptr<Tree> Utils::parseTree(const std::vector<char>& file)
{
    // std::cout << "Size: " << file.size() << std::endl;
    std::vector<TreeEntry> entries;
    for (size_t i = 0, len = file.size(); i < len; ++i) // not sure why len - 1
    {
        std::string fileMode = "";

        size_t fmodemax = 0;
        if (file[i] == '4') // directory mode
            fmodemax = 5;
        else // file mode
            fmodemax = 6;
        

        for (size_t j = 0; j < fmodemax; ++j)
        {
            fileMode += file[i + j];
        }
        i += fmodemax;

        std::cout << fileMode << " ";

        std::string fileName = "";
        for (; file[i] != '\0'; ++i)
            fileName += file[i];

        std::cout << (Utils::splitPath(fileName).end() - 1)[0] << " ";
        ++i;

        std::string fileHash = "";
        for (auto x : unpackBytesToString(std::vector<char>(
            file.begin() + i, 
            file.begin() + i + 20
            )))
                fileHash += x;
        std::cout << fileHash << std::endl;
        i += 19;

        entries.push_back(TreeEntry(fileMode, fileName, fileHash));
        // if (fileMode[0] == '4')
        // {
            parseObjectFile(Utils::getPathFromHash(fileHash));
        // }
        // std::cout << i << std::endl;
    }
    
    return std::make_shared<Tree>(entries);
}

const std::shared_ptr<Commit> Utils::parseCommit(const std::vector<char>& file)
{
    // for (auto x : file)
    //     std::cout << x;
    // std::cout << std::endl;

    std::string treeHash(file.begin() + 5, file.begin() + 5 + 40);
    // std::cout << treeHash << std::endl;

    parseObjectFile(Utils::getPathFromHash(treeHash));

    return nullptr;
}

const std::string Utils::getPathFromHash(const std::string& hash) 
{
    std::string pathDir = Git::gitDir + "/" + "objects" + "/" + hash.substr(0, 2);

    if (!Utils::fileExists(pathDir)) { // check if directory exists
        mkdir(pathDir.c_str(), 0777); // testing purposes
    }

    return Git::gitDir + "/" + "objects" + "/" + hash.substr(0, 2) + "/" + hash.substr(2);
}