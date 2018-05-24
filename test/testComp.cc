#include <vector>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <netbuilder/Util.h>

// std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> compositionsChanges(int n, int nb_parts){
//     assert(nb_parts>= 2 && n>= nb_parts);
//     if (nb_parts == 2){
//         std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> v;
//         if (n == 2){
//             return {};
//         }
//         for (int k=n-1; k>1; k--){
//             v.push_back({{1, k}, {2, n+1-k}});
//         }
//         return v;
//     }
//     else{
//         std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> V_grand;
//         for (int k=n-nb_parts+1; k>=1; k--){
//             auto V_petit = compositionsChanges(n-k, nb_parts-1);

//             if (! (k & 1)){
//                 std::reverse(V_petit.begin(), V_petit.end());
//                 if (k < n-nb_parts+1){
//                     V_grand.push_back({{1, k+1}, {nb_parts, n-k-nb_parts+2}});
//                 }
//             }
//             else{
//                 if (k < n-nb_parts+1){
//                     V_grand.push_back({{1, k+1}, {2, n-k-nb_parts+2}});
//                 }
//             }
            
//             for (std::pair<std::pair<int, int>, std::pair<int, int>> v: V_petit){
//                 if (! (k & 1)){   // k is even
//                     auto temp = v.first;
//                     v.first = v.second;
//                     v.second = temp;
//                 }
//                 v.first.first++;
//                 v.second.first++;
                
//                 V_grand.push_back(v);
//             }    
//         }
//         return V_grand;
//     }
// }

int main(){
    auto v = NetBuilder::compositionsChanges(12, 4);
    int i = 0;
    for (auto p: v){
        std::cout << i <<": ((" << p.first.first << ", " << p.first.second << "), (" << p.second.first << ", " << p.second.second << "))" << std::endl;
        i++;
    }

    // std::vector<std::vector<int>> v2 = NetBuilder::compositions(7, 4);
    // for (std::vector<int> vec:v2){
    //     for (int i:vec){
    //         std::cout << i;
    //     }
    //     std::cout << std::endl;
    // }

    int lim = (int) v.size()/5;
    std::cout << lim << std::endl;
    std::vector<int> state (4, 1);
    state[0] = 9;
    for (int i=0; i<lim; i++){
        state[v[i].first.first-1]--;
        state[v[i].second.first-1]++;
    }
    for (int i:state){
        std::cout << i;
    }
    std::cout << std::endl;
    return 0;
}