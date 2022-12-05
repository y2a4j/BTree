#include<iostream>
#include<vector>
using namespace std;

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
                }
            }
        }
};

void BTreeNode::testPrint(BTreeNode *n, int keycount){
    for(int i=0; i<keycount; i++){
        cout << n->key[i] << endl;
    }
}

void BTreeNode::traverse(){
    int i;
    for(i=0; i<keycount; i++){
        if(leaf == false){
            child[i]->traverse();//木の左側
        }
        cout << " " << key[i];
    }

    if(leaf == false){
        child[i]->traverse();//木の右側
    }

}

BTreeNode *BTreeNode::search(int k){
    
    BTreeNode *c = this;
    while(c->keycount != 0){
        int i = 0;
        while(i < c->keycount && k > c->key[i]){//キーがありそうな場所を探す
            i++;
        }

        if(c->keycount > i){
            if(c->key[i] == k){//キーがあったら返す
                return c;
            }
        } 

        if(c->leaf == true){//キーがないときNULL
            //delete c;
            return NULL;
        }

        c = c->child[i];//次の子ノードを見る
    }
    //delete c;
    return NULL;
}

void BTree::insert(int k){
    if(root == NULL){
        root = new BTreeNode(true);//新しい根ノード
        root->key[0] = k;
        root->keycount++;
        height++;
    }else{
        if(root->keycount == 2*MIN_DEGREE-1){//根ノードがFULL
            //根ノードを分割
            BTreeNode *r = new BTreeNode(false);//新しい根ノード
            r->child[0] = root;
            r->split(root, 0);

            int i = 0;
            if(r->key[0] < k){
                i++;
            }
            r->child[i]->insertSub(k);
            root = r;
            height++;
        }else{
            root->insertSub(k);
        }
    }
}

void BTreeNode::insertSub(int k){
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

        if(child[i+1]->keycount == 2*MIN_DEGREE-1){//キーの入る子ノードがFULLのとき
            split(child[i+1], i+1);
            if(key[i+1] < k){
                i++;
            }
        }
        child[i+1]->insertSub(k);//キーが入る場所に着くまで子ノードをたどる
    }
}

void BTreeNode::split(BTreeNode *a, int ci){
    int mid = MIN_DEGREE-1;
    BTreeNode *r = new BTreeNode(a->leaf);//分割した右側のノード
            
    //右側の調整
    for(int i=0; i<mid; i++){
        r->key[i] = a->key[MIN_DEGREE + i];
        r->keycount++;
    }
    //右側の子の調整
    if(a->leaf == false){//分割するノードが葉でないとき
        for(int i=0; i<MIN_DEGREE; i++){
            r->child[i] = a->child[MIN_DEGREE + i];
        }
    }
    a->keycount = mid;
    //右の子を入れる
    for(int i=keycount; i >= ci + 1; i--){
        child[i+1] = child[i];
    }
    child[ci+1] = r;

    //中央値を入れる
    for(int i = keycount-1; i>=ci; i--){
        key[i+1] = key[i];
    }
    key[ci] = a->key[mid];
    keycount++;
}

BTreeNode *BTreeNode::searchParent(BTreeNode *n){
    int i = 0;
    while(i < keycount && n->key[0] > key[i]){
        i++;
    }

    if(n == child[i]){
        return this;
    }

    return child[i]->searchParent(n);

}

void BTreeNode::tempKey(BTreeNode *a, int ai, BTreeNode *b, int bi){
    int x;
    x = a->key[ai];
    a->key[ai] = b->key[bi];
    b->key[bi] = x;
}

void BTreeNode::rightShift(BTreeNode *leftn, BTreeNode *n, BTreeNode *np){
    
    int i = searchNode(np, leftn);
    cout << "rs" << endl;
    for(int j=n->keycount; j>0; j--){
        n->key[j] = n->key[j-1];
    }
    n->key[0] = np->key[i];
    n->keycount++;
    np->key[i] = leftn->key[leftn->keycount-1];
    for(int j=n->keycount; j>0; j--){
        n->child[j] = n->child[j-1];
    }
    n->child[0] = leftn->child[leftn->keycount];
    leftn->keycount--;

}

void BTreeNode::leftShift(BTreeNode *rightn, BTreeNode *n, BTreeNode *np){
    
    int i = searchNode(np, rightn);
    cout << "ls" << endl;
    n->key[n->keycount] = np->key[i-1];
    n->keycount++;
    np->key[i-1] = rightn->key[0];
    for(int j=0; j<rightn->keycount-1; j++){
        rightn->key[j] = rightn->key[j+1];
    }
    n->child[n->keycount] = rightn->child[0];
    for(int j=0; j<rightn->keycount; j++){
        rightn->child[j] = rightn->child[j+1];
    }
    rightn->keycount--;

}

void BTreeNode::leftMerge(BTreeNode *leftn, BTreeNode *n, BTreeNode *np){

    int i = searchNode(np, n);
    cout << "lm" << endl;
    leftn->key[leftn->keycount] = np->key[i-1];
    leftn->keycount++;
    for(int j=0; j<n->keycount; j++){
        leftn->child[leftn->keycount] = n->child[j];
        leftn->key[leftn->keycount] = n->key[j];
        leftn->keycount++;
    }
    leftn->child[leftn->keycount] = n->child[n->keycount];

    n->keycount = 0;
    for(int j=i-1; j<np->keycount-1; j++){
        np->key[j] = np->key[j+1];
        np->child[j+1] = np->child[j+2];
    }
    np->keycount--;
}

void BTreeNode::rightMerge(BTreeNode *rightn, BTreeNode *n, BTreeNode *np){

    int i = searchNode(np, n);
    cout << "rm" << endl;
    n->key[n->keycount] = np->key[i];
    n->keycount++;
    for(int j=0; j<rightn->keycount; j++){
        n->child[n->keycount] = rightn->child[j];
        n->key[n->keycount] = rightn->key[j];
        n->keycount++;
    }
    n->child[n->keycount] = rightn->child[rightn->keycount];
    
    rightn->keycount = 0;
    for(int j=i; j<np->keycount-1; j++){
        np->key[j] = np->key[j+1];
        np->child[j+1] = np->child[j+2];
    }
    np->keycount--;
    
}

int BTreeNode::searchNode(BTreeNode *np, BTreeNode *n){

    int l=0;
    while(l < np->keycount+1){
        if(np->child[l] == n){
            break;
        }
        l++;
    }
    return l;
}

vector<BTreeNode*> BTreeNode::delSearch(int k){

    BTreeNode *c = this;
    vector<BTreeNode*> pas;
    while(c->keycount != 0){
        pas.push_back(c);
        int i = 0;
        while(i < c->keycount && k > c->key[i]){//キーがありそうな場所を探す
            i++;
        }
        if(c->keycount > i){
            if(c->key[i] == k){//キーがあったら返す
                return pas;
            }
        }
        c = c->child[i];//次の子ノードを見る
    }
    return pas;

}

BTreeNode *BTreeNode::adjustNode(BTreeNode *n, BTreeNode *np){

    BTreeNode *leftn = new BTreeNode();
    BTreeNode *rightn = new BTreeNode();
    int l = searchNode(np, n);

    if(n->keycount >= MIN_DEGREE-1){
        return this;
    }
    //a 右シフト
    if(l >= 1){
        leftn = np->child[l-1];
        if(leftn->keycount > MIN_DEGREE-1){
            rightShift(leftn, n, np);
            if(n->keycount >= MIN_DEGREE-1){
                //delete leftn;
                //delete rightn;
                return this;
            }
        }
    }

    //b 左シフト
    if(l < np->keycount){
        rightn = np->child[l+1];
        if(rightn->keycount > MIN_DEGREE-1){
            leftShift(rightn, n, np);
            if(n->keycount >= MIN_DEGREE-1){
                //delete leftn;
                //delete rightn;
                return this;
            }
        }
    }

    //c 左兄弟との併合
    if(l >= 1){
        leftn = np->child[l-1];
        if(leftn->keycount == MIN_DEGREE-1){
            leftMerge(leftn, n, np);
            if(np->keycount == 0){
                //delete rightn;
                return leftn;
            }
            if(np->keycount >= MIN_DEGREE-1){
                //delete rightn;
                //delete leftn;
                return this;
            }    
        }
    }

    //d 右兄弟との併合
    if(l < np->keycount){
        rightn = np->child[l+1];
        if(rightn->keycount == MIN_DEGREE-1){
            rightMerge(rightn, n, np);
            if(np->keycount == 0){
                //delete rightn;
                return n;
            }
            if(np->keycount >= MIN_DEGREE-1){
                //delete leftn;
                //delete rightn;
                return this;
            }
        }
    }
    return this;
}


BTreeNode *BTreeNode::bdelete(int k, int height){
    BTreeNode *newroot = this;
    BTreeNode *s = search(k);
    if(s == NULL){//該当キーがないとき終了
        return this;
    }

    BTreeNode *n = new BTreeNode(this->leaf);
    vector<BTreeNode*> pas = delSearch(k);

    //該当キーの添え字を記録
    int i;
    for(i=0; i<s->keycount; i++){
        if(s->key[i] == k){
            break;
        }
    }

    if(s->leaf != true){//該当のキーが内部ノードにあったとき

        cout << "ind" << endl;
        BTreeNode *t = s;//直前
        BTreeNode *t1 = s;//直後

        //該当キーのすぐ下の子に移動
        t = t->child[i];//直前用
        t1 = t1->child[i+1];//直後用
        
        vector<BTreeNode*> pas2{t}, pas3{t1};
        
        //該当キーの直前か直後かを判断する
        while(t->leaf != true){//直前
            t = t->child[t->keycount];
            pas2.insert(pas2.end(), t);
        }
        while(t1->leaf != true){//直後
            t1 = t1->child[0];
            pas3.insert(pas3.end(), t1);
        }        
        
        bool r = false;//直前
        if(t->keycount <= t1->keycount){
            r = true;//直後
        }
        
        if(r == false){
            for(auto itr=pas2.begin(); itr != pas2.end(); itr++){
                pas.insert(pas.end(), *itr);
            }
        }else{
            for(auto itr=pas3.begin(); itr != pas3.end(); itr++){
                pas.insert(pas.end(), *itr);
            }
        }
        
        //該当キーと葉を入れ替える
        int x;//入れ替え用
        if(r == false){//直前
            tempKey(t,t->keycount-1,s,i);
        }else{//直後
            tempKey(t1,0,s,i);
        }
        
        //該当キーの削除        
        if(r == false){
            t->keycount--;
            n = t;
        }else{
            for(int j=0; j<t1->keycount-1; j++){
                t1->key[j] = t1->key[j+1];
            }
            t1->keycount--;
            n = t1;
        }

        //delete t;
        //delete t1;
        pas.pop_back();
    }else{//該当キーが葉にあったら

        cout << "lnd" << endl;
        if(i != s->keycount-1){//直後
            for( ; i<s->keycount-1; i++){
                s->key[i] = s->key[i+1];
            }
            s->keycount--;
        }else{//直前
            s->keycount--;
        }
        n = s;
        pas.pop_back();

        if(height == 1){
            return newroot;
        }
    }

    if(n->keycount >= MIN_DEGREE-1){
        //delete s;
        //delete n;
        return newroot;
    }

    //ノードの調整
    bool flag = true;
    BTreeNode *np = new BTreeNode();
    while(flag){
        np = pas.back();
        pas.pop_back();
        newroot = adjustNode(n, np);
        n = np;
        if(n == this){
            flag = false;
        }
    }
    //delete s;
    //delete n;
    //delete np;
    return newroot;
}


void BTreeNode::print(int h){
    int i;
    for(i=keycount; i>=0; i--){
        if(leaf == false){
            child[i]->print(h+1);
        }
        if(i>0){
            for(int j=0; j<h; j++){
                cout << "\t";
            }
            cout << key[i-1] << endl;
        }
    }
}

int main(){
    BTree t;

    t.insert(1);
    t.insert(99);
    t.insert(55);
    t.insert(77);
    t.insert(22);
    t.insert(33);
    t.insert(76);
    t.insert(4);
    t.insert(15);
    t.insert(17);
    t.insert(92);
    t.insert(84);
    t.insert(47);
    t.insert(25);
    t.insert(9);
    
    t.insert(11);
    t.insert(62);
    t.insert(46);
    t.insert(19);
    t.insert(87);
    t.insert(79);
    t.insert(59);
    t.insert(3);
    t.insert(40);
    t.insert(17);
    t.insert(43);
    t.insert(57);
    t.insert(87);
    t.insert(7);
    t.insert(27);
    t.insert(28);
    t.insert(29);
    t.insert(23);
    t.insert(16);
    t.insert(35);
    t.insert(36);
    t.insert(39);
    t.insert(30);
    t.insert(50);
    t.insert(51);
    t.insert(52);
    t.insert(53);
    t.insert(49);
    t.insert(44);
    t.insert(45);
    t.insert(49);
    
    int k = 17, l = 101;
    if(t.search(k) == NULL){
        cout << k <<" is not found" << endl;
    }else{
        cout << k <<" is found" << endl;
    }
    if(t.search(l) == NULL){
        cout << l << " is not found" << endl;
    }else{
        cout << l << " is found" << endl;
    }

    t.traverse();
    cout << endl;
    cout << " heights = " << t.height << endl;
    cout << endl;
    t.print();
    cout << "--------------------------------------" << endl;
    
    t.bdelete(59);
    t.bdelete(87);
    t.bdelete(92);
    t.print();
    
    cout << "--------------------------------------" << endl;
    t.bdelete(57);
    t.print();
    cout << "--------------------------------------" << endl;
    
    t.bdelete(51);
    t.bdelete(49);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(45);

    t.bdelete(44);
    t.bdelete(79);
    t.bdelete(43);
    t.bdelete(30);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(77);

    t.bdelete(9);
    t.bdelete(17);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(16);
    t.bdelete(15);
    t.bdelete(11);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(22);
    t.print();
    cout << "--------------------------------------" << endl;

    
    t.bdelete(53);
    t.print();
    cout << "--------------------------------------" << endl;
    
    t.bdelete(19);
    t.bdelete(23);
    t.bdelete(52);
    t.bdelete(62);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(47);
    t.bdelete(40);
    t.bdelete(49);
    t.bdelete(35);
    t.bdelete(33);
    t.bdelete(39);
    cout << t.height << endl;
    t.print();
    cout << "--------------------------------------" << endl;

    t.bdelete(55);
    t.print();
    cout << "--------------------------------------" << endl;
    t.traverse();
    cout << endl;
    t.bdelete(46);
    t.traverse();
    cout << endl;
    t.print();
    cout << "--------------------------------------" << endl;

    
    t.bdelete(50);
    cout << "height " << t.height << endl;
    t.print();
    cout << "--------------------------------------" << endl;
    
    t.bdelete(29);
    t.print();
    cout << "--------------------------------------" << endl;
    
    t.bdelete(27);
    t.bdelete(25);
    t.bdelete(28);
    t.print();
    cout << "--------------------------------------" << endl;

    t.bdelete(87);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(84);
    t.print();
    cout << "--------------------------------------" << endl;
    
    t.bdelete(76);
    t.print();
    cout << "--------------------------------------" << endl;

    
    t.bdelete(4);
    t.print();
    cout << "--------------------------------------" << endl;

    t.bdelete(3);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(7);
    cout << "height " << t.height << endl;
    t.print();

    cout << "--------------------------------------" << endl;
    t.bdelete(99);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(36);
    t.print();
    cout << "--------------------------------------" << endl;

    
    t.bdelete(17);
    t.print();
    cout << "--------------------------------------" << endl;
    t.bdelete(1);
    t.print();
    cout << "--------------------------------------" << endl;

    t.insert(1);
    t.insert(99);
    t.insert(55);
    t.insert(77);
    t.insert(22);
    t.insert(33);
    t.insert(76);
    t.insert(4);
    t.insert(15);
    t.insert(17);
    t.insert(92);
    t.insert(84);
    t.insert(47);
    t.insert(25);
    t.insert(9);
    
    t.insert(11);
    t.insert(62);
    t.insert(46);
    t.insert(19);
    t.insert(87);
    t.insert(79);
    t.insert(59);
    t.insert(3);
    t.insert(40);
    t.insert(17);
    t.insert(43);
    t.insert(57);
    t.insert(87);
    t.insert(7);
    t.insert(27);
    t.insert(28);
    t.insert(29);
    t.insert(23);
    t.insert(16);
    t.insert(35);
    t.insert(36);
    t.insert(39);
    t.insert(30);
    t.insert(50);
    t.insert(51);
    t.insert(52);
    t.insert(53);
    t.insert(49);
    t.insert(44);
    t.insert(45);
    t.insert(49);

    t.print();
    
    return 0;
}

