#include "csg.h"

struct CSGNode {
    void InitLeaf(int index) {
        shape_index = index;
        oper = 0;
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

bool CSG::recursiveIntersect(CSGNode* node, const Ray &ray, Intersection *isect) const {
    bool hit = false;
    Intersection inter;
    while (true) {
        Intersection inter0, inter1;
        bool hit0, hit1;
        switch (node->oper) {
            case OBJECT:
                if (shapes[node->shape_index]->Intersect(ray, &inter)) {
                    hit = true;
                    if (inter.t < t) {
                        t = inter.t;
                        *isect = inter;
                    }
                }
                break;
            case UNION:
                hit0 = recursiveIntersect(node->children[0], ray, &inter0);
                hit1 = recursiveIntersect(node->children[1], ray, &inter1);
                if (hit0 || hit1) {
                    hit = true;
                    if (inter0.t < isect->t || inter1.t < isect->t) {
                        if (inter0.t < inter1.t) {
                            *isect = inter0;
                        } else if (inter1.t < inter0.t) {
                            *isect = inter1;
                        }
                    }
                }
                break;
            case DIFFER:
                break;
            case INTER:
                hit0 = recursiveIntersect(node->children[0], ray, &inter0);
                hit1 = recursiveIntersect(node->children[1], ray, &inter1);
                if (hit0 && hit1) {
                    hit = true;
                    if (inter0.t < isect->t || inter1.t < isect->t) {
                        if (inter0.t < inter1.t) {
                            *isect = inter0;
                        } else if (inter1.t < inter0.t) {
                            *isect = inter1;
                        }
                    }
                }

        }
    }
    return hit;
}

bool CSG::Intersect(const Ray &ray, Intersection *isect) const {
    isect->t = INFINITY;
    return recursiveBuild(root, ray, isect);
}
