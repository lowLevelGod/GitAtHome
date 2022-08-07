#include <../headers/flatToTree.hpp>
#include <algorithm>
#include <queue>
#include <iostream>

const std::vector<std::string> FlatToTree::splitPath(std::string path)
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

void FlatToTree::flatToTree(const std::vector<std::string>& paths)
{
    // root node is the empty path
    std::shared_ptr<Node> root = std::make_shared<Node>(std::vector<std::shared_ptr<Node>>(), "", nullptr);

    for (auto path : paths)
    {
        // get all directories on path or file in case of last token
        std::vector<std::string> tokens = splitPath(path);
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

    this->root = root;
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