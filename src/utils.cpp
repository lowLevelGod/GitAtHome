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

const std::vector<char> Utils::decompressObject(const std::string &fileName, const size_t len)
{
    // read compressed blob from objects
    std::vector<char> compressed(Utils::readBinaryFile(fileName));

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

const std::string Utils::getMode(const std::string& fileName)
{
    struct stat st = {0};

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
    return "NULL";
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