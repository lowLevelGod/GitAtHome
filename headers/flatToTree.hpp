#pragma once
#include <vector>
#include <string>
#include <memory>
#include <index.hpp>

class Node
{
    public:
        std::vector<std::shared_ptr<Node>> children;
        std::string name;
        std::shared_ptr<Node> parent;
        Node() = default;
        Node(const std::vector<std::shared_ptr<Node>>& children, const std::string& name, const std::shared_ptr<Node>& parent) 
        : children(children), name(name), parent(parent) {}
};

class FlatToTree
{
    private:
        std::shared_ptr<Node> root;
    public:
        const Tree createCommitTree(const std::shared_ptr<Node>&);   
        void insert(const std::vector<std::string>&);
        void dfs(const std::shared_ptr<Node>&);
        void bfs(const std::shared_ptr<Node>&);
        const std::shared_ptr<Node>& getRoot() { return root;}
        FlatToTree() : root(std::make_shared<Node>(std::vector<std::shared_ptr<Node>>(), "", nullptr)) {}
};