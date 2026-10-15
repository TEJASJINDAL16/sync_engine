// #include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <queue>
#include <set>
#include <algorithm>
using namespace std;
// this are the queries ( filter )
// there can be at most 15 nodes in a single query tree 
struct query {
    int id ;
    string op ;
    query ( int i ) { this->id  = i ;} 
    virtual ~query() = default ;
};
struct logic : query {
    // string op ;
    // op is inherited from query.
    vector<int>child ;
    logic ( int i , stringstream& ss ) : query(i){
        ss >> op ;
        int n ; ss >> n ; // this is for the the next numebr of 
        // while (ss) {
        for ( int i = 0 ; i < n ; i++) {
            string t ; ss >> t ;
            child.push_back(stoi(t));
        }
    }
};
struct cond : query{
    string col ;
    // string op ;
    // op is inherited from query.
    string comparator;
    string value ;
    cond (int i , stringstream& ss) : query(i) {
        ss >> op ;
        ss >> col ;
        ss >> comparator;
        ss >> value;
    }
};
struct tree {
    // here from the queries build the tree 
    // conditional is a leaf node 
    // logic is the node
    // what i will be given is a vector of queries and i have to build tree from that 
    vector<vector<int>> adj;
    vector<query*>& queries ;
    tree( vector<query*>& q ) : queries(q) {
        adj.resize(16);
        build();
    } 
    // this is for building the hierarchy 
    // to access a particular query we can access the q vector using the query id
    void build(){
        queue<int> q;
        q.push(1);
        while (!q.empty()){
            int f = q.front();
            q.pop();
            query* ptr = queries[f];
            if ( ptr->op == "COND") continue ;
            logic* pt = dynamic_cast<logic*>(ptr);
            for ( auto it : pt->child){
                adj[f].push_back(it);
                q.push(it);
            }
        }
    }
};

// now we have to build the write 
// the thing is a row can have upto 20 colums and each write can have from 0 to 20 columns
// for each write we have to store the columns and their value in a map 
struct write {
    int id ; 
    write ( int i ) { this ->id = i ;}
    map<string , string> mp ;
    void read ( int t ) {
        for ( int i = 0 ; i < t ; i++) {
            string key ; string value ;
            cin >> key >> value ;
            mp[key] = value ;
        }
    }
};
bool operator== ( write& w1 , write&w2 ){
    return w1.mp == w2.mp ;
}

/* <-------------------------------------------------------------------------------------->*/
// now we have to make a solve function 
bool evaluate ( query*q , write&w ) {
    cond* temp = dynamic_cast<cond*>(q);
    string coll = temp->col;
    // map < string, string> mpp = w.mp ;
    map < string, string>& mpp = w.mp ;
    if ( !mpp.count(coll)) return false ;
    string value = mpp[coll] ;
    string oprtr = temp->comparator;
    auto number = [](const string& s) {
        size_t i = (!s.empty() && (s[0] == '-' || s[0] == '+')) ? 1 : 0;
        return i < s.size() && all_of(s.begin()+i, s.end(), [](char c) { return c >= '0' && c <= '9'; });
    };
    bool a = number(value), b = number(temp->value);
    if ( a != b ) return false;
    if ( a ) {
        long long x = stoll(value), y = stoll(temp->value);
        if ( oprtr == "=" ) return x == y;
        if ( oprtr == "<=" ) return x <= y;
        if ( oprtr == ">=" ) return x >= y;
        if ( oprtr == ">" ) return x > y;
        if ( oprtr == "<" ) return x < y;
    }
    if ( oprtr == "=") return value == temp->value;
    else if( oprtr == "<=") return value <= temp->value;
    else if ( oprtr == ">=") return value >= temp->value;
    else if ( oprtr == ">") return value > temp->value;
    else if ( oprtr == "<") return value < temp->value;
    return false;
}
bool ee ( query*q , tree&tt , write&w ) {
    if ( q->op == "COND") return evaluate( q , w ) ;
    int t = q->id;
    if ( q -> op == "OR") {
        bool result = false ;
        logic* temp = dynamic_cast<logic*>(q);
        for ( int i = 0 ; i < temp->child.size() ; i++) {
            result = result || ee ( tt.queries[temp->child[i]] , tt , w ) ; 
        }
        return result ;
    }
    bool result = true ;
    logic* temp = dynamic_cast<logic*>(q);
    for ( int i = 0 ; i < temp->child.size() ; i++) {
        result = result && ee ( tt.queries[temp->child[i]] , tt , w ) ; 
    }
    return result ;
}
bool solve ( write&w , tree&t ) {
    // tree has an adj and queries list ( 1 based indexding )
    return ee( t.queries[1] , t , w ) ;

}
vector<int> sol ( write& w , vector<tree>&t ) {
    vector<int>ans ;
    for ( int i = 0 ; i < t.size() ; i++) { 
        if ( solve( w , t[i])) ans.push_back(i+1);
    }
    return ans;
}
/* <-------------------------------------------------------------------------------------->*/
int main() {
   int n , q ;
   cin >> n >> q ;
   vector<vector<query*>> queries ;
   for ( int i = 0 ; i < n ; i++) {
        int t ; cin >> t ; // this is the number of nodes in the query tree
        // vector<query*>temp; 
        vector<query*>temp(1, nullptr);
        for ( int j = 0 ; j < t ; j++) {
            string s ; getline(cin >> ws, s); stringstream ss(s);
            char ch = s[0];
            if ( ch == 'C') { query* cc = new cond(j+1,ss) ;temp.push_back(cc);}
            else { query* cc = new logic(j+1,ss) ;temp.push_back(cc);}
        }
        queries.push_back(temp);
   }
   // insert and delete have same function 
   vector<write> notup ; 
   vector<pair<write,write>> update ;
   int cnt = 0 ;
   for ( int i = 0 ; i < q ; i++) {
    string tt ; 
    cin >> tt ;
    if ( tt == "UPDATE") {
        int t1 ; cin >> t1 ;
        // write w1(cnt) ;
        write w1(cnt+1) ;
        w1.read(t1) ;
        int t2 ; cin >> t2;
        // write w2(cnt);
        write w2(cnt+1);
        w2.read(t2);
        cnt++;
        update.push_back({w1,w2});
        continue ;
    }
    cnt++;
    int t ; cin >> t ;
    write w(cnt) ;
    w.read(t);
    notup.push_back(w);
    }
    vector<tree> trees ;
    for ( int i = 0 ; i < n ; i++) {
        tree tt(queries[i]);
        trees.push_back(tt);
    }
    vector<vector<int>> ans ;
    int i = 0 ; 
    int j = 0 ;
    int c = 1 ;
    vector<tree>tr ;
    for ( int i = 0 ; i < queries.size() ; i++) { tree temp(queries[i]) ; tr.push_back(temp);}
    // while ( i + j < c ) {
    while ( c <= cnt ) {
        // if ( notup[i].id == c ) { ans.push_back( sol(notup[i] , tr)) ; i++;} 
        if ( i < notup.size() && notup[i].id == c ) { ans.push_back( sol(notup[i] , tr)) ; i++;}
        else { 
            // vector<int> v1 = sol(update[i].first , tr) ;
            vector<int> v1 = sol(update[j].first , tr) ;
            // vector<int> v2 = sol(update[i].second , tr) ;
            vector<int> v2 = sol(update[j].second , tr) ;
            set<int> st ;
            st.insert(v1.begin() , v1.end()); 
            st.insert(v2.begin() , v2.end());
            vector<int> tu ;
            for ( auto it : st ) tu.push_back(it);
            ans.push_back(tu);
            j++;
        }
        c++;
    } 
    for ( int i = 0 ; i < ans.size() ; i++) {
        // for ( int j = 0 ; j < ans[0].size() ; j++) cout << ans[i][j] ;
        if ( ans[i].empty() ) cout << -1;
        for ( int j = 0 ; j < ans[i].size() ; j++) cout << (j ? " " : "") << ans[i][j] ;
        cout << "\n";
    }
    for ( auto& v : queries ) for ( auto ptr : v ) delete ptr;
}
