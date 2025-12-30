// template <typename T>
// class TreeNode:Iterable<T>{
//     T val;
//     unique_ptr<TreeNode<T>> left;
//     unique_ptr<TreeNode<T>> right;
//     public:
//     TreeNode(T val):val(val),left(nullptr),right(nullptr){}
//     T getVal(){
//         return val;
//     }
//     void setLeft(unique_ptr<TreeNode<T>> l){
//         left=move(l);
//     }
//     void setRight(unique_ptr<TreeNode<T>> r){
//         right=move(r);
//     }
//     unique_ptr<TreeNode<T>> getLeft(){
//         return left;
//     }
//     unique_ptr<TreeNode<T>> getRight(){
//         return right;
//     }

//     unique_ptr<Iterator<T>> getIterator();
// };