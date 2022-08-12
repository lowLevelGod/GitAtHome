#include <../headers/flatToTree.hpp>
#include <algorithm>
#include <queue>
#include <iostream>

void FlatToTree::insert(const std::vector<std::string>& paths)
{
    // root node is the empty path

    for (auto path : paths)
    {
        // get all directories on path or file in case of last token
        std::vector<std::string> tokens = Utils::splitPath(path);
        std::shared_ptr<Node> currentNode = root;
        for (auto token : tokens)
        {
            bool found = false;
            for (auto node : currentNode->children)
            {   
                // node already exists in tree, so just go down through it
                if (node->name == token)
                {
                    currentNode = node;
                    found = true;
                    break;
                }
            }

            // if node isn't found, create it and link it to current node
            if (!found)
            {
                std::shared_ptr<Node> newNode = std::make_shared<Node>(std::vector<std::shared_ptr<Node>>(), token, currentNode);
                currentNode->children.push_back(newNode);
                currentNode = newNode;
            }
        }
    }

}

const Tree FlatToTree::createCommitTree(const std::shared_ptr<Node>& currentNode)
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
                absolutePath,
                Blob(absolutePath)
            ));
        }else // it's a tree
        {
            treeEntries.push_back(TreeEntry(
                absolutePath,
                Tree(createCommitTree(child))
            ));
        }
    }

    Tree tempTree(treeEntries);
    tempTree.serialize("");

    return tempTree;
}

void FlatToTree::dfs(const std::shared_ptr<Node>& currentNode)
{
    for (auto child : currentNode->children)
    {
        std::cout << child->name << std::endl;
        dfs(child);
    }
}


void FlatToTree::bfs(const std::shared_ptr<Node>& currentNode)
{
    std::queue<std::shared_ptr<Node>> q;
    q.push(currentNode);

    while (!q.empty())
    {
        std::shared_ptr<Node> tempNode = q.front();
        std::cout << "Parent: " << tempNode->name << std::endl; 
        q.pop();
        for (auto n : tempNode->children)
        {
            q.push(n);
            std::cout << n->name << " ";
        }

        std::cout << std::endl;
    }
}