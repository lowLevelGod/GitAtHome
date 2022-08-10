#include <../headers/commit.hpp>
#include <../headers/utils.hpp>
#include <iostream>

const std::string Commit::getHeader(const size_t len) const
{
    return "commit" + Object::getHeader(len);
}

Commit::Commit(const Tree& tree, const CommitMessage& cm, const std::vector<std::string>& parents)
{
    //tree {tree_sha1}
    std::string treeString = "tree " + tree.getHash();

    //parent {parent_sha1} for each entry
    std::string parentString = "";
    for (auto p : parents)
        parentString += "parent " + p;

    //author {author_name} <{author_email}> {author_date_seconds} {author_date_timezone}
    std::string authorString = "author " + cm.author;

    //committer {committer_name} <{committer_email}> {committer_date_seconds} {committer_date_timezone}
    std::string committerString = "committer " + cm.committer;

    std::string resultString = treeString + parentString + authorString + committerString + "\n" + cm.message;
    content.assign(resultString.begin(), resultString.end());

    //prepending header to content to compute sha1
    resultString = getHeader(resultString.length()) + resultString;
    std::vector<char> result;
    result.assign(resultString.begin(), resultString.end());
    
    hash.assign(Utils::getSHA1hash(result));

    // tree.serialize("");
    tree.print();
}

void Commit::serialize(const std::string& t = "") const
{
    std::string header = getHeader(content.size());
    this->Object::serialize(header);
}

void Commit::print() const
{
  
}