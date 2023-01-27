#include<iostream>
#include "btree2.h"
static constexpr unsigned int BSIZE = sizeof(BTreeNode);

// void BTree::deleteNode(BTreeNode *n){
//     if(n->child[0]->keycount != 0){
//         deleteNode(n->child[0]);
//     }
//     for(int i=0; i<n->keycount; i++){
//         if(n->child[i+1]->keycount != 0){
//             deleteNode(n->child[i+1]);
//         }
//     }
//     delete n;
// }

// void BTreeNode::testPrint(BTreeNode *n, int keycount){
//     for(int i=0; i<keycount; i++){
//         cout << n->key[i] << endl;
//     }
// }

BTreeNode BTreeNode::load(FILE *fp, int id){
    fseek(fp, id*BSIZE, SEEK_SET);
    //cout << *this << endl;
    fread(this, BSIZE, 1, fp);
    //cout << *this << endl;
    // for(int i = 0; i < BSIZE; ++i) {
    //     cout << (int) *((char*)this+i) << " ";
    // }
    // cout << endl;
    return this;
}

void BTreeNode::save(FILE *fp){
    fseek(fp, id*BSIZE, SEEK_SET);
    fwrite(this, BSIZE, 1, fp);
    // for(int i = 0; i < BSIZE; ++i) {
    //     cout << (int)*((char*)this+i) << " ";
    // }
    // cout << endl;
}

void BTreeNode::traverse(FILE *fp){
    int i;
    BTreeNode temp;
    for(i=0; i<keycount; i++){
        if(leaf == false){
            temp.load(fp, child[i]);
            temp.traverse(fp);//木の左側
        }
        cout << " " << key[i];
    }

    if(leaf == false){
        temp.load(fp, child[i]);
        temp.traverse(fp);//木の右側
    }
}

BTreeNode BTreeNode::search(int k, FILE *fp){
    
    BTreeNode temp = this;
    while(temp.keycount != 0){
        int i = 0;
        while(i < temp.keycount && k > temp.key[i]){//キーがありそうな場所を探す
            i++;
        }

        if(temp.keycount > i){
            if(temp.key[i] == k){//キーがあったら返す
                return temp;
            }
        } 

        if(temp.leaf == true){//キーがないときNULL
            return NULL;
        }

        temp.load(fp, temp.child[i]);//次の子ノードを見る
    }
    return NULL;
}

void BTree::insert(int k){
    if(root == 0){
        BTreeNode temp(true);
        int number = 1;
        temp.key[0] = k;
        temp.keycount++;
        temp.id = number;
        height++;
        //tempをファイルに書き込む
        FILE *fp = fopen(filename, "rb+");
        temp.save(fp);
        root = temp.id;
        saveNum(fp, number);
        fclose(fp);
    }else{
        BTreeNode temp;
        FILE *fp = fopen(filename, "rb+");
        temp.load(fp, root);
        if(temp.keycount == 2*MIN_DEGREE-1){//根ノードがFULL
            int number = loadNum(fp);
            
            //根ノードを分割
            BTreeNode newroot(false);//root
            number++;
            saveNum(fp, number);
            newroot.id = number;
            root = newroot.id;

            newroot.child[0] = temp.id;
            newroot.split(temp, 0, fp);
            newroot.save(fp);
            int i=0;
            if(newroot.key[0] < k){
                i++;
            }
            temp.load(fp, newroot.child[i]);
            temp.insertSub(k, fp);
            temp.save(fp);

            height++;
        }else{
            temp.insertSub(k, fp);
            temp.save(fp);
        }
        fclose(fp);
    }
}

void BTreeNode::insertSub(int k, FILE *fp){
    int i = keycount - 1;
    if(leaf == true){//葉のとき
        while(i >= 0 && key[i] > k){//上からキーの入るべき場所が見つかるまで
            key[i+1] = key[i];//キーをずらす
            i--;
        }
        key[i+1] = k;
        keycount++;
    }else{//葉でないとき
        while(i >= 0 && key[i] > k){
            i--;
        }

        BTreeNode temp2;
        temp2.load(fp, child[i+1]);
        if(temp2.keycount == 2*MIN_DEGREE-1){//キーの入る子ノードがFULLのとき
            split(temp2, i+1, fp);
            if(key[i+1] < k){
                i++;
            }
        }

        temp2.load(fp, child[i+1]);
        temp2.insertSub(k, fp);//キーが入る場所に着くまで子ノードをたどる
        temp2.save(fp);
    }
}

void BTreeNode::split(BTreeNode temp, int ci, FILE *fp){
    int mid = MIN_DEGREE-1;
    int number = loadNum(fp);
    BTreeNode r(temp.leaf);//分割した右側のノード
    number++;
    r.id = number;
    saveNum(fp, number);
            
    //右側の調整
    for(int i=0; i<mid; i++){
        r.key[i] = temp.key[MIN_DEGREE + i];
        r.keycount++;
    }
    //右側の子の調整
    if(temp.leaf == false){//分割するノードが葉でないとき
        for(int i=0; i<MIN_DEGREE; i++){
            r.child[i] = temp.child[MIN_DEGREE + i];
        }
    }

    temp.keycount = mid;

    //右の子を入れる
    for(int i=keycount; i >= ci + 1; i--){
        child[i+1] = child[i];
    }
    child[ci+1] = r.id;

    //中央値を入れる
    for(int i = keycount-1; i>=ci; i--){
        key[i+1] = key[i];
    }
    key[ci] = temp.key[mid];
    keycount++;

    r.save(fp);
    temp.save(fp);
}

// BTreeNode *BTreeNode::searchParent(BTreeNode *n){
//     int i = 0;
//     while(i < keycount && n->key[0] > key[i]){
//         i++;
//     }

//     if(n == child[i]){
//         return this;
//     }

//     return child[i]->searchParent(n);

// }

// void BTreeNode::tempKey(BTreeNode *a, int ai, BTreeNode *b, int bi){
//     int x;
//     x = a->key[ai];
//     a->key[ai] = b->key[bi];
//     b->key[bi] = x;
// }

// void BTreeNode::rightShift(BTreeNode *leftn, BTreeNode *n, BTreeNode *np){
    
//     int i = searchNode(np, leftn);
//     cout << "rs" << endl;
//     for(int j=n->keycount; j>0; j--){
//         n->key[j] = n->key[j-1];
//     }
//     n->key[0] = np->key[i];
//     n->keycount++;
//     np->key[i] = leftn->key[leftn->keycount-1];
//     for(int j=n->keycount; j>0; j--){
//         n->child[j] = n->child[j-1];
//     }
//     n->child[0] = leftn->child[leftn->keycount];
//     leftn->keycount--;

// }

// void BTreeNode::leftShift(BTreeNode *rightn, BTreeNode *n, BTreeNode *np){
    
//     int i = searchNode(np, rightn);
//     cout << "ls" << endl;
//     n->key[n->keycount] = np->key[i-1];
//     n->keycount++;
//     np->key[i-1] = rightn->key[0];
//     for(int j=0; j<rightn->keycount-1; j++){
//         rightn->key[j] = rightn->key[j+1];
//     }
//     n->child[n->keycount] = rightn->child[0];
//     for(int j=0; j<rightn->keycount; j++){
//         rightn->child[j] = rightn->child[j+1];
//     }
//     rightn->keycount--;

// }

// void BTreeNode::leftMerge(BTreeNode *leftn, BTreeNode *n, BTreeNode *np){

//     int i = searchNode(np, n);
//     cout << "lm" << endl;
//     leftn->key[leftn->keycount] = np->key[i-1];
//     leftn->keycount++;
//     for(int j=0; j<n->keycount; j++){
//         leftn->child[leftn->keycount] = n->child[j];
//         leftn->key[leftn->keycount] = n->key[j];
//         leftn->keycount++;
//     }
//     leftn->child[leftn->keycount] = n->child[n->keycount];

//     n->keycount = 0;
//     for(int j=i-1; j<np->keycount-1; j++){
//         np->key[j] = np->key[j+1];
//         np->child[j+1] = np->child[j+2];
//     }
//     np->keycount--;
//     delete n;
// }

// void BTreeNode::rightMerge(BTreeNode *rightn, BTreeNode *n, BTreeNode *np){

//     int i = searchNode(np, n);
//     cout << "rm" << endl;
//     n->key[n->keycount] = np->key[i];
//     n->keycount++;
//     for(int j=0; j<rightn->keycount; j++){
//         n->child[n->keycount] = rightn->child[j];
//         n->key[n->keycount] = rightn->key[j];
//         n->keycount++;
//     }
//     n->child[n->keycount] = rightn->child[rightn->keycount];
    
//     rightn->keycount = 0;
//     for(int j=i; j<np->keycount-1; j++){
//         np->key[j] = np->key[j+1];
//         np->child[j+1] = np->child[j+2];
//     }
//     np->keycount--;
//     delete rightn;
    
// }

// int BTreeNode::searchNode(BTreeNode *np, BTreeNode *n){

//     int l=0;
//     while(l < np->keycount+1){
//         if(np->child[l] == n){
//             break;
//         }
//         l++;
//     }
//     return l;
// }

// vector<BTreeNode*> BTreeNode::delSearch(int k){

//     BTreeNode *c = this;
//     vector<BTreeNode*> pas;
//     while(c->keycount != 0){
//         pas.push_back(c);
//         int i = 0;
//         while(i < c->keycount && k > c->key[i]){//キーがありそうな場所を探す
//             i++;
//         }
//         if(c->keycount > i){
//             if(c->key[i] == k){//キーがあったら返す
//                 return pas;
//             }
//         }
//         c = c->child[i];//次の子ノードを見る
//     }
//     return pas;

// }

// BTreeNode *BTreeNode::adjustNode(BTreeNode *n, BTreeNode *np){

//     int l = searchNode(np, n);

//     if(n->keycount >= MIN_DEGREE-1){
//         return this;
//     }
//     //a 右シフト
//     if(l >= 1){
//         BTreeNode *leftn = np->child[l-1];
//         if(leftn->keycount > MIN_DEGREE-1){
//             rightShift(leftn, n, np);
//             if(n->keycount >= MIN_DEGREE-1){
//                 return this;
//             }
//         }
//     }

//     //b 左シフト
//     if(l < np->keycount){
//         BTreeNode *rightn = np->child[l+1];
//         if(rightn->keycount > MIN_DEGREE-1){
//             leftShift(rightn, n, np);
//             if(n->keycount >= MIN_DEGREE-1){
//                 return this;
//             }
//         }
//     }

//     //c 左兄弟との併合
//     if(l >= 1){
//         BTreeNode *leftn = np->child[l-1];
//         if(leftn->keycount == MIN_DEGREE-1){
//             leftMerge(leftn, n, np);
//             if(np->keycount == 0){
//                 return leftn;
//             }
//             if(np->keycount >= MIN_DEGREE-1){
//                 return this;
//             }    
//         }
//     }

//     //d 右兄弟との併合
//     if(l < np->keycount){
//         BTreeNode *rightn = np->child[l+1];
//         if(rightn->keycount == MIN_DEGREE-1){
//             rightMerge(rightn, n, np);
//             if(np->keycount == 0){
//                 return n;
//             }
//             if(np->keycount >= MIN_DEGREE-1){
//                 return this;
//             }
//         }
//     }
//     return this;
// }


// BTreeNode *BTreeNode::bdelete(int k, int height){
//     BTreeNode *s = search(k);
//     if(s == NULL){//該当キーがないとき終了
//         return this;
//     }

//     BTreeNode *newroot = this;
//     BTreeNode *n;
//     vector<BTreeNode*> pas = delSearch(k);

//     //該当キーの添え字を記録
//     int i;
//     for(i=0; i<s->keycount; i++){
//         if(s->key[i] == k){
//             break;
//         }
//     }

//     if(s->leaf != true){//該当のキーが内部ノードにあったとき

//         cout << "ind" << endl;
//         BTreeNode *t = s;//直前
//         BTreeNode *t1 = s;//直後

//         //該当キーのすぐ下の子に移動
//         t = t->child[i];//直前用
//         t1 = t1->child[i+1];//直後用
        
//         vector<BTreeNode*> pas2{t}, pas3{t1};
        
//         //該当キーの直前か直後かを判断する
//         while(t->leaf != true){//直前
//             t = t->child[t->keycount];
//             pas2.insert(pas2.end(), t);
//         }
//         while(t1->leaf != true){//直後
//             t1 = t1->child[0];
//             pas3.insert(pas3.end(), t1);
//         }        
        
//         bool r = false;//直前
//         if(t->keycount <= t1->keycount){
//             r = true;//直後
//         }
        
//         if(r == false){
//             for(auto itr=pas2.begin(); itr != pas2.end(); itr++){
//                 pas.insert(pas.end(), *itr);
//             }
//         }else{
//             for(auto itr=pas3.begin(); itr != pas3.end(); itr++){
//                 pas.insert(pas.end(), *itr);
//             }
//         }
        
//         //該当キーと葉を入れ替える
//         int x;//入れ替え用
//         if(r == false){//直前
//             tempKey(t,t->keycount-1,s,i);
//         }else{//直後
//             tempKey(t1,0,s,i);
//         }
        
//         //該当キーの削除        
//         if(r == false){
//             t->keycount--;
//             n = t;
//         }else{
//             for(int j=0; j<t1->keycount-1; j++){
//                 t1->key[j] = t1->key[j+1];
//             }
//             t1->keycount--;
//             n = t1;
//         }

//         pas.pop_back();
//     }else{//該当キーが葉にあったら

//         cout << "lnd" << endl;
//         if(i != s->keycount-1){//直後
//             for( ; i<s->keycount-1; i++){
//                 s->key[i] = s->key[i+1];
//             }
//             s->keycount--;
//         }else{//直前
//             s->keycount--;
//         }
//         n = s;
//         pas.pop_back();

//         if(height == 1){
//             return newroot;
//         }
//     }

//     if(n->keycount >= MIN_DEGREE-1){
//         return newroot;
//     }

//     //ノードの調整
//     bool flag = true;
//     BTreeNode *np;
//     while(flag){
//         np = pas.back();
//         pas.pop_back();
//         newroot = adjustNode(n, np);
//         n = np;
//         if(n == this){
//             flag = false;
//         }
//     }
//     return newroot;
// }

void BTreeNode::print(int h, FILE *fp){
    int i;
    for(i=keycount; i>=0; i--){
        if(leaf == false){
            BTreeNode temp;
            temp.load(fp, child[i]);
            temp.print(h+1, fp);
        }
        if(i>0){
            for(int j=0; j<h; j++){
                cout << "\t";
            }
            cout << key[i-1] << endl;
        }
    }
}

// void BTreeNode::treeWrite(FILE *fp){
//     fwrite(this, sizeof(BTreeNode), 1, fp);
//     cout << "fwrite: " << this->key[0] << " ..." << endl;
//     if(leaf != true){
//         for(int i=0; i<keycount+1; i++){
//             child[i]->treeWrite(fp);
//         }
//     }
// }

// int getFileSize(const char* fileName)
// {
//     FILE* fp = fopen(fileName, "rb");
//     if (fp == NULL) {
//         return -1LL;
//     }

//     if (fseek(fp, 0L, SEEK_END) == 0) {
//         fpos_t pos;

//         if (fgetpos(fp, &pos) == 0) {
//             fclose(fp);
//             return pos;
//         }
//     }

//     fclose(fp);
//     return -1LL;
// }

int main(){

    cout << "sizeof(BTreeNode): " << sizeof(BTreeNode) << endl;
    BTree t;
    FILE *fp = fopen(filename, "rb");

    t.insert(3);
    t.insert(42);
    t.insert(78);
    t.insert(99);
    t.insert(29);

    t.insert(55);
    t.insert(62);
    t.insert(59);
    t.insert(32);
    t.insert(82);

    t.insert(44);
    t.insert(47);
    t.insert(49);
    t.insert(52);

    t.insert(17);
    t.insert(10);
    t.insert(20);

    t.insert(63);
    t.insert(65);
    t.insert(70);

    t.insert(73);

    t.insert(22);
    t.insert(24);
    t.insert(27);

    t.insert(75);
    t.insert(69);

    t.insert(35);
    t.insert(36);
    t.insert(39);

    t.insert(1);
    t.insert(5);
    t.insert(9);
    t.insert(4);

    t.insert(11);
    t.insert(13);
    t.insert(14);
    t.insert(7);

    int number = loadNum(fp);
    cout << "number: " << number << endl;
    t.traverse(fp);
    cout << endl;
    t.print(fp);

    // BTreeNode temp = t.search(55, fp);
    // if(temp.id == 0){
    //     cout << " not found" << endl;
    // }else{
    //     cout << " found" << endl;
    //     cout << "id: " << temp.id << endl;
    //     for(int i=0; i<temp.keycount; i++){
    //         cout << "key: " << temp.key[i] << ", ";
    //     }
    //     cout << endl;
    // }

    fclose(fp);

    cout << "sizeof(BTreeNode): " << sizeof(BTreeNode) << endl;
    return 0;
}

