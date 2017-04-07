#include "csg.h"

struct CSGNode {
    void InitLeaf(int index) {
        shape_index = index;
        oper = -1;
    }
    void InitInterior(int operation, CSGNode *c0, CSGNode *c1) {
        children.push_back(c0);
        children.push_back(c1);
        oper = operation;
        shape_index = -1;
    }
    std::vector<CSGNode*> children;
    int shape_index;
    int oper;
};

void deleteNode(CSGNode* node) {
    if (node->children.size() != 0) {
        for (auto i : node->children) deleteNode(i);
    }
    delete node;
}

CSG::~CSG() {
    deleteNode(root);
}

// shape_num: index into shape array
// oper_num: index into operator array
CSGNode* CSG::recursiveBuild(int* shape_num, int* oper_num) {

    CSGNode* node = new CSGNode();
    operation oper = operators[(*oper_num)++];
    switch (oper) {
        case OBJECT :
            node->InitLeaf((*shape_num)++);
            break;
        default :
            CSGNode *left = recursiveBuild(shape_num, oper_num);
            CSGNode *right = recursiveBuild(shape_num, oper_num);
            node->InitInterior(oper, left, right);
    }
    return node;
}

CSG::CSG() {
    int shape_num = 0;
    int oper_num = 0;
    root = recursiveBuild(&shape_num, &oper_num);
}

bool CSG::Intersect(const Ray &ray, Intersection *isect) const {

}
