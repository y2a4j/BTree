#ifndef BTREE_H_
#define BTREE_H_

using namespace std;
#include<iostream>
#include<vector>
typedef unsigned int Key;
static constexpr unsigned int MIN_DEGREE = 3;
const char *filename = "btree.dat";

#define FNULL 0;

int loadNum(FILE *fp){
    int number;
    fseek(fp, 1, SEEK_SET);
    fread(&number, sizeof(int), 1, fp);
    return number;
}
void saveNum(FILE *fp, int number){
    fseek(fp, 1, SEEK_SET);
    fwrite(&number, sizeof(int), 1, fp);
}

struct BTreeNode{  

    Key key[2*MIN_DEGREE-1];
    int child[2*MIN_DEGREE];//子が入っているバイナリファイル上の番号
    bool leaf;
    unsigned int keycount;
    int id;//自身の番号を入れる変数
    
    public:
        BTreeNode(void){
            id = 0;
            bool bleaf = true;
            keycount = 0;
            for(int i=0; i<2*MIN_DEGREE-1; i++){
                key[i] = 0;
            }
            for(int i=0; i<2*MIN_DEGREE; i++){
                child[i] = 0;
            }
        };
        BTreeNode(bool bleaf){
            id = 0;
            keycount = 0;
            leaf = bleaf;
            for(int i=0; i<2*MIN_DEGREE-1; i++){
                key[i] = 0;
            }
            for(int i=0; i<2*MIN_DEGREE; i++){
                child[i] = 0;
            }
        };
        ~BTreeNode(){
        };
        BTreeNode load(FILE *fp, int id);
        void save(FILE *fp);
        void split(BTreeNode a, int ci, FILE *fp);//分割したい子ノードとその子ノードの添え字
        void insertSub(int k, FILE *fp);
        void traverse(FILE *fp);
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
        void treeWrite(FILE *fp);

        friend ostream & operator<<(ostream & out, const BTreeNode & node) {
            out << "ID: " << node.id << ", KEYCOUNT: " << node.keycount << ", leaf: " << node.leaf;
            return out;
        }

    friend struct BTree;        
};

struct BTree{
    int root;//nodeのバイナリファイル上での番号
    int height;
    //buffer
    BTreeNode temp;

    public:
        BTree(){
            root = 0;
            height = 0;
        }
        // ~BTree(){
        //     deleteNode(root);
        // };
        void insert(int k);
        void traverse(){
            if(root != 0){
                FILE *fp = fopen(filename, "rb");
                temp.load(fp, root);
                temp.traverse(fp);
                fclose(fp);
            }
        }
        // void print(){
        //     if(root != NULL){
        //         root->print(1);
        //     }
        // }
        // void deleteNode(BTreeNode *root);
        // BTreeNode *search(int k){
        //     if(root == NULL){
        //         return NULL;
        //     }else{
        //         root->search(k);
        //     }
        // }
        // vector<BTreeNode*> delSearch(int k){
        //     root->delSearch(k);
        // }
        // BTreeNode *searchParent(BTreeNode *n){
        //     if(height == 1 || height == 0){
        //         return NULL;
        //     }else{
        //         root->searchParent(n);
        //     }
        // }
        // BTreeNode *bdelete(int k){
        //     if(root == NULL){
        //         return NULL;
        //     }else{
        //         BTreeNode *newroot = root->bdelete(k, height);
        //         if(newroot != root){
        //             delete root;
        //             root = newroot;
        //             height--;
        //             cout << "height changes...height: " << height << endl;
        //         }
        //     }
        // }

        // void fNew();
        // void nodeFinsert(BTreeNode temp);
        // BTreeNode nodeFread(){
        //     root->nodeFread(temp);
        // }

};

//渡されたノードのポインタでファイル操作する
// struct CachedBTreeNode{

//     BTreeNode *nodead;
//     BTreeNode nodecache;
//     uintptr_t ad;

//     //コンストラクタ
//     //局所的に確保する場合
//     //新しくノードを生成する場合

//     CachedBTreeNode(BTreeNode *temp){
//         nodead = temp;
//         nodecache = *nodead;
//         ad = uintptr_t(nodead);

//         cout << nodead << endl;
//         cout << ad << endl;
        
//         FILE *fp = fopen(filename, "wb");
//         //if(SEEK_CUR == 0)
//         fseek(fp, ad, SEEK_CUR);
//         fwrite(&nodecache, sizeof(BTreeNode), 1, fp);
//         fclose(fp);
//     };

//     CachedBTreeNode(){
//         FILE *fp = fopen(filename, "wb");
//         if(SEEK_CUR != 0){
//             ad = SEEK_CUR;
//             //ポインタアドレスに変換 nodead = 
//             nodecache = *nodead;
//         }

//     }

//     //デストラクタ
//     //指されない　で作ってみる

//     //欲しくなったら削除されたノードの番号を保持しておく、ゴミ箱
// };

#endif