#ifndef PERSON_H_
#define PERSON_H_

using namespace std;
#include<iostream>
#include<vector>
typedef unsigned int Key;
static constexpr unsigned int MIN_DEGREE = 3;

struct BTreeNode{  

    Key key[2*MIN_DEGREE-1];
    BTreeNode *child[2*MIN_DEGREE];
    bool leaf;
    unsigned int keycount;
    
    public:
        BTreeNode(void){
            bool bleaf = true;
            keycount = 0;
            for(int i=0; i<2*MIN_DEGREE-1; i++){
                key[i] = 0;
            }
            for(int i=0; i<2*MIN_DEGREE; i++){
                child[i] = NULL;
            }
        };
        BTreeNode(bool bleaf){
            keycount = 0;
            leaf = bleaf;
            for(int i=0; i<2*MIN_DEGREE-1; i++){
                key[i] = 0;
            }
            for(int i=0; i<2*MIN_DEGREE; i++){
                child[i] = NULL;
            }
        };
        ~BTreeNode(){
        };
        void split(BTreeNode *a, int ci);//分割したい子ノードとその子ノードの添え字
        void insertSub(int k);
        void traverse();
        void print(int h);
        BTreeNode *search(int k);
        vector<BTreeNode*> delSearch(int k);
        BTreeNode *searchParent(BTreeNode *n);
        BTreeNode *bdelete(int k, int height);
        void tempKey(BTreeNode *a, int ai, BTreeNode *b, int bi);
        void rightShift(BTreeNode *leftn, BTreeNode *n, BTreeNode *np);
        void leftShift(BTreeNode *rightn, BTreeNode *n, BTreeNode *np);
        void rightMerge(BTreeNode *rightn, BTreeNode *n, BTreeNode *np);
        void leftMerge(BTreeNode *leftn, BTreeNode *n, BTreeNode *np);
        BTreeNode *adjustNode(BTreeNode *n, BTreeNode *np);
        int searchNode(BTreeNode *np, BTreeNode *n);
        void testPrint(BTreeNode *n, int keycount);//確認用

    friend struct BTree;        
};

struct BTree{
    BTreeNode *root;
    int height;

    public:
        BTree(){
            root = NULL;
            height = 0;
        }
        ~BTree(){
            deleteNode(root);
        };
        void insert(int k);
        void traverse(){
            if(root != NULL){
                root->traverse();
            }
        }
        void print(){
            if(root != NULL){
                root->print(1);
            }
        }
        void deleteNode(BTreeNode *root);
        BTreeNode *search(int k){
            if(root == NULL){
                return NULL;
            }else{
                root->search(k);
            }
        }
        vector<BTreeNode*> delSearch(int k){
            root->delSearch(k);
        }
        BTreeNode *searchParent(BTreeNode *n){
            if(height == 1 || height == 0){
                return NULL;
            }else{
                root->searchParent(n);
            }
        }
        BTreeNode *bdelete(int k){
            if(root == NULL){
                return NULL;
            }else{
                BTreeNode *newroot = root->bdelete(k, height);
                if(newroot != root){
                    delete root;
                    root = newroot;
                    height--;
                    cout << "height changes...height: " << height << endl;
                }
            }
        }
};

#endif