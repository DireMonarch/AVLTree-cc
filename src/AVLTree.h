/*
 * Copyright 2024 Jim Haslett
 *
 * This file is part of the 11c.dev AVL Balanced Binary Search Tree implementation.
 *
 * AVL Balanced Binary Search Tree is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * AVL Balanced Binary Search Tree is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * the AVL Balanced Binary Search Tree. If not, see <https:// www.gnu.org/licenses/>.
 */


#ifndef SRC_AVLTREE_H_
#define SRC_AVLTREE_H_

#include <format>
#include <stack>
#include <queue>
#include <stdexcept>
#include <iterator>
#include <cstddef>
#include "AVLTreeNode.h"

namespace _11c_dev_collections {
/**
 * enum used to determine the order of Iteration traversal of an AVLTree.
 */
enum class AVLTreeTraversalMethod {
    /**
     * Iterates an AVL tree in natural order represented by the Key.
     */
    InOrder,
    /**
     * Iterates an AVL tree in reverse order represented by the Key.
     */
    ReverseOrder,
    /**
     * Iterates an AVL tree in top down order. This iterator traverses the
     * AVLTree in special way that is useful for serializing or saving the tree
     * for the purpose of reloading another tree. Inserting the elements into a
     * tree in the order they are iterated here is the fastest way to load the
     * tree without necessary and costly sorting.
     */
    TopDown
};
/**
 * AVL Balanced Binary Search Tree.
 *
 * An AVL tree is a self-balancing binary search tree. In an AVL tree, the
 * heights of the two child subtrees of any node differ by at most one; if at
 * any time they differ by more than one, rebalancing is done to restore thi
 * property. Lookup, insertion, and deletion all take O(log n) time in both the
 * average and worst cases, where n is the number of nodes in the tree prior to
 * the operation. Insertions and deletions may require the tree to be rebalanced
 * by one or more tree rotations.
 *
 * @param <TKey>
 *            Generic type representing the key used for sorting. Must
 *            implement <, =, and >.
 * @param <TValue>
 *            Generic type representing the data being stored.
 */
template <class TKey, class TValue>
class AVLTree {
 private:
    AVLTreeNode<TKey, TValue> *root_;
    int count_;
    AVLTreeTraversalMethod traversal_method_;

 public:
	/**
	 * Creates a new AVLTree that defaults to InOrder traversal.
	 */
    AVLTree() {
        root_ = nullptr;
        count_ = 0;
        traversal_method_ = AVLTreeTraversalMethod::InOrder;
    }

	/**
	 * Creates a new AVLTree specifying the traversal method of iteration.
	 * 
	 * @param TraversalMethod
	 *            Defines the traversal method used in iteration.
	 */    
    explicit AVLTree(AVLTreeTraversalMethod traversal_method) {
        root_ = nullptr;
        count_ = 0;
        traversal_method_ = traversal_method;
    }

	/**
	 * Returns the number of elements in the tree.
	 * 
	 * @return Number of elements in the tree.
	 */
    int GetCount() { return count_; }

	/**
	 * Returns the current traversal method used for iteration.
	 * 
	 * @return Current traversal method for iteration.
	 */
    AVLTreeTraversalMethod GetTraversalMethod() { return traversal_method_; }

	/**
	 * Set the traversal method for iteration.
	 * 
	 * @param traversalMethod
	 *            New traversal method.
	 */
    void SetTraversalMethod(AVLTreeTraversalMethod traversal_method) {
        traversal_method_ = traversal_method;
    }

    /**
     * Returns the current height of the tree.
     * 
     * @return int height of the tree
     */
    int GetTreeHieight() {
        if (root_ != nullptr) return root_->GetHeight();
        return 0;
    }

    /**
     * Returns the current balance factor of the root node.
     * 
     * @return int balance factor of the root node.
     */
    int GetTreeBalanceFactor() {
        if (root_ != nullptr) return root_->GetBalanceFactor();
        return 0;
    }

    /**
     * Gets an AVLTreeNode indexed by key. This is the equivalent of an array
     * indexer.
     *
     * @param Key Key to locate in the tree.
     *
     * @return AVLTreeNode at key.
     * 
     * @throws range_error if no node exists at key
     */
    AVLTreeNode<TKey, TValue> GetNode(TKey key) {
        AVLTreeNode<TKey, TValue> *current = root_;

        while (current != nullptr) {
            if (current->GetKey() == key) {
                return *current;
            }
            if (current->GetKey() < key) {
                current = current->GetRight();
            } else  {
                current = current->GetLeft();
            }
        }

        throw std::range_error
            (std::format("! Key {} not present in Tree !", key));
    }

    /**
     * Gets a MapEntry representing they key/value pair indexed by key. This is
     * the equivalent of an array indexer.
     *
     * @param Key Key to locate in the tree.
     *
     * @return MapEntry representing the key/value pair found.
     * 
     * @throws range_error if no node exists at key
     */
    MapEntry<TKey, TValue> Get(TKey key) { return GetNode(key).GetMapEntry(); }

    /**
     * Returns the key with the minimum value.
     *
     * @return Minimum valued key in the tree.
     */
    TKey GetMinKey() {
        if (root_ == nullptr)
            return nullptr;

        AVLTreeNode<TKey, TValue> *current = root_;
        while (current->GetLeft() != nullptr)
            current = current->GetLeft();
        return current->GetKey();
    }

    /**
     * Returns the key with the maximum value.
     *
     * @return Maximum valued key in the tree.
     */
    TKey GetMaxKey() {
        if (root_ == nullptr)
            return nullptr;

        AVLTreeNode<TKey, TValue> *current = root_;
        while (current->GetRight() != nullptr)
            current = current->GetRight();
        return current->GetKey();
    }


    /**
     * Clear the contents of the tree.
     */
    void Clear() {
        root_ = nullptr;
        count_ = 0;
    }

    /**
     * Add a key/value pair to the tree.
     *
     * @param Key
     *            Key used for ordering the tree entries.
     * @param Value
     *            Value to be stored.
     * @throws std::range_error
     */
    void Add(TKey Key, TValue Value) {
        std::stack<AVLTreeNode<TKey, TValue>*> my_stack =
            std::stack<AVLTreeNode<TKey, TValue>*>();
        AVLTreeNode<TKey, TValue> *node =
            new AVLTreeNode<TKey, TValue>(Key, Value);

        AVLTreeNode<TKey, TValue> *current = root_;
        AVLTreeNode<TKey, TValue> *parent = nullptr;

        my_stack.push(nullptr);

        while (current != nullptr) {
            my_stack.push(current);

            if (node->GetKey() == current->GetKey()) {
                // Duplicate Value, throw exception
                throw std::range_error("! Key already exists in Tree !");
            } else if (node->GetKey() > current->GetKey()) {
                    // node.key > current.key --> Go Right
                parent = current;
                current = current->GetRight();
            } else {  // node.key < current.key --> Go Left
                parent = current;
                current = current->GetLeft();
            }
        }

        count_++;

        if (parent == nullptr) {  // Empty Tree
            root_ = node;
        } else {
            if (node->GetKey() > parent->GetKey()) {
                    // node.key > parent.key add to right
                parent->SetRight(node);
            } else {  // parent.key > node.key add to left
                parent->SetLeft(node);
            }
        }

        // Go back up the tree and reset height
        current = my_stack.top(); my_stack.pop();
        while (current != nullptr) {
            current->CalculateHeight();
            if (current->GetBalanceFactor() > 1) {
                if (current->GetLeft()->GetBalanceFactor() < 0)
                    RotateLeft(current->GetLeft(), current);
                RotateRight(current, my_stack.top());
            } else if (current->GetBalanceFactor() < -1) {
                if (current->GetRight()->GetBalanceFactor() > 0)
                    RotateRight(current->GetRight(), current);
                RotateLeft(current, my_stack.top());
            }
            current = my_stack.top(); my_stack.pop();
        }
    }

    /**
     * Remove an entry from the tree.
     *
     * @param Key
     *            Key of entry to remove.
     * @return MapEntry representing the key/value pair that was removed.
     */
    MapEntry<TKey, TValue> Remove(TKey key) {
        std::stack<AVLTreeNode<TKey, TValue>*> my_stack =
            std::stack<AVLTreeNode<TKey, TValue>*>();

        AVLTreeNode<TKey, TValue> * removed = nullptr;

        AVLTreeNode<TKey, TValue> * current = root_;
        AVLTreeNode<TKey, TValue> * parent = nullptr;

        my_stack.push(nullptr);

        while (current != nullptr && current->GetKey() != key) {
            my_stack.push(current);

            if (key > current->GetKey()) {  // key > current.key  --> Go Right
                parent = current;
                current = current->GetRight();
            } else {  // key < current.key --> Go Left
                parent = current;
                current = current->GetLeft();
            }
        }

        if (current == nullptr) {  // Key not found, throw exception??
            return nullptr;
        } else {
            count_--;
            removed = current;

            /*
            * Case 1: If the node being deleted has no right child, then the
            * node's left child can be used as the replacement. The binary
            * search tree property is maintained because we know that the
            * deleted node's left subtree itself maintains the binary search
            * tree property, and that the values in the left subtree are all
            * less than or all greater than the deleted node's parent,
            * depending on whether the deleted node is a left or right child.
            * Therefore, replacing the deleted node with its left subtree
            * maintains the binary search tree property.
            */
            if (current->GetRight() == nullptr) {
                if (current->GetLeft() != nullptr)
                    my_stack.push(current->GetLeft());
                if (parent == nullptr) {  // deleting the root
                    root_ = current->GetLeft();
                } else {
                    if (parent->GetKey() < current->GetKey()) {
                        parent->SetRight(current->GetLeft());
                    } else {
                        parent->SetLeft(current->GetLeft());
                    }
                }

            /*
            * Case 2: If the deleted node's right child has no left child, then
            * the deleted node's right child can replace the deleted node. The
            * binary search tree property is maintained because the deleted
            * node's right child is greater than all nodes in the deleted
            * node's left subtree and is either greater than or less than the
            * deleted node's parent, depending on whether the deleted node wa
            * a right or left child. Therefore, replacing the deleted node with
            * its right child maintains the binary search tree property.
            */
            } else if (current->GetRight()->GetLeft() == nullptr) {
                my_stack.push(current->GetRight());
                current->GetRight()->SetLeft(current->GetLeft());
                if (parent == nullptr) {  // deleting the root
                    root_ = current->GetRight();
                } else {
                    if (parent->GetKey() < current->GetKey()) {
                        parent->SetRight(current->GetRight());
                    } else {
                        parent->SetLeft(current->GetRight());
                    }
                }

            /*
            * Case 3: Finally, if the deleted node's right child does have a
            * left child, then the deleted node needs to be replaced by the
            * deleted node's right child's left-most descendant. That is, we
            * replace the deleted node with the right subtree's smallest value.
            */
            } else {
                AVLTreeNode<TKey, TValue> * lmparent = current->GetRight();
                AVLTreeNode<TKey, TValue> * leftmost = lmparent->GetLeft();

                std::queue<AVLTreeNode<TKey, TValue>*> lmqueue =
                    std::queue<AVLTreeNode<TKey, TValue>*>();

                lmqueue.push(lmparent);

                // Find the leftmost node of current's right node, and it'
                // parent.
                while (leftmost->GetLeft() != nullptr) {
                    lmqueue.push(leftmost);
                    lmparent = leftmost;
                    leftmost = lmparent->GetLeft();
                }

                // Set the leftmost's parent's left node to the leftmosts right
                // node
                lmparent->SetLeft(leftmost->GetRight());

                // Set leftmost's left and right equal to current's left and
                // right
                leftmost->SetRight(current->GetRight());
                leftmost->SetLeft(current->GetLeft());

                if (parent == nullptr) {  // deleting the root
                    root_ = leftmost;
                } else {
                    if (parent->GetKey() < current->GetKey()) {
                        parent->SetRight(leftmost);
                    } else {
                        parent->SetLeft(leftmost);
                    }
                }
                my_stack.push(leftmost);
                while (!lmqueue.empty()) {
                    my_stack.push(lmqueue.front()); lmqueue.pop();
                }
            }

            current = my_stack.top(); my_stack.pop();
            while (current != nullptr) {
                current->CalculateHeight();
                if (current->GetBalanceFactor() > 1) {
                    if (current->GetLeft()->GetBalanceFactor() < 0)
                        RotateLeft(current->GetLeft(), current);
                    RotateRight(current, my_stack.top());
                } else if (current->GetBalanceFactor() < -1) {
                    if (current->GetRight()->GetBalanceFactor() > 0)
                        RotateRight(current->GetRight(), current);
                    RotateLeft(current, my_stack.top());
                }
                current = my_stack.top(); my_stack.pop();
            }

            MapEntry<TKey, TValue> map_entry = removed->GetMapEntry();
            removed->SetLeft(nullptr);
            removed->SetRight(nullptr);
            delete removed;
            return map_entry;
        }
    }

    /**
     * AVL Function to to rotate right at a given node, with a given parent.
     *      used in the balancing algorithm.
     * 
     * @param *node pointer to AVLTreeNode to rotate
     * @param *parent pointer to AVLTreeNode of the parent to *node
     *          if parent is nullptr, the parent is root_
     */
    void RotateRight(AVLTreeNode<TKey, TValue> *node,
            AVLTreeNode<TKey, TValue> *parent) {
        AVLTreeNode<TKey, TValue> * left_node = node->GetLeft();
        node->SetLeft(left_node->GetRight());
        left_node->SetRight(node);

        node->CalculateHeight();
        left_node->CalculateHeight();

        if (parent == nullptr) {
            root_ = left_node;
        } else {
            if (left_node->GetKey() < parent->GetKey())
                parent->SetLeft(left_node);
            else
                parent->SetRight(left_node);
        }
    }

    /**
     * AVL Function to to rotate left at a given node, with a given parent.
     *      used in the balancing algorithm.
     * 
     * @param *node pointer to AVLTreeNode to rotate
     * @param *parent pointer to AVLTreeNode of the parent to *node
     *          if parent is nullptr, the parent is root_
     */
    void RotateLeft(AVLTreeNode<TKey, TValue> *node,
            AVLTreeNode<TKey, TValue> *parent) {
        AVLTreeNode<TKey, TValue> * right_node = node->GetRight();
        node->SetRight(right_node->GetLeft());
        right_node->SetLeft(node);

        node->CalculateHeight();
        right_node->CalculateHeight();

        if (parent == nullptr) {
            root_ = right_node;
        } else {
            if (right_node->GetKey() < parent->GetKey())
                parent->SetLeft(right_node);
            else
                parent->SetRight(right_node);
        }
    }


    // ITERATOR

    /**
     * Iterator for AVLTree.  Used by builtin C++ for ( A : B ) loops.
     */
    struct Iterator{
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = AVLTreeNode<TKey, TValue>;
        using pointer = AVLTreeNode<TKey, TValue>*;
        using reference = AVLTreeNode<TKey, TValue>&;

        /**
         * Constructor.
         * 
         * @param start
         *          AVLTreeNode where the iteration will start.
         * 
         * @param traversal_method
         *          AVLTreeTraversalMethod that dictates the order of
         *              returned nodes.
         */
        Iterator(pointer start, AVLTreeTraversalMethod traversal_method) {
            // current_ = start;
            traversal_method_ = traversal_method;
            SetInitalLocation(start);
        }

        /**
         * * operator overload required for standard iterator
         */
        reference operator*() const { return *current_; }

        /**
         * -> operator overload required for standard iterator
         */
        pointer operator->() { return current_; }

        /**
         * Post increment ++ operator overload required for standard Iterator.
         */
        Iterator& operator++() { MoveNext(); return *this; }

        /**
         * == operator overload required for standard iterator
         */
        friend bool operator== (const Iterator &a, const Iterator &b) {
            return a.current_ == b.current_;
        }

        /**
         * += operator overload required for standard iterator
         */
        friend bool operator!= (const Iterator &a, const Iterator &b) {
            return a.current_ != b.current_;
        }

     private:
        pointer current_;
        std::stack<pointer> stack_;
        std::queue<pointer> queue_;

        AVLTreeTraversalMethod traversal_method_;

        /**
         * Sets the initial position of the Iterator, give start, based on
         *  the traversal_method_
         */
        void SetInitalLocation(pointer start) {
            switch (traversal_method_) {
            case AVLTreeTraversalMethod::InOrder:
                stack_ = std::stack<pointer>();
                stack_.push(nullptr);
                stack_.push(start);
                StackLeftToNull();
                current_ = stack_.top(); stack_.pop();
                break;  // AVLTreeTraversalMethod::InOrder

            case AVLTreeTraversalMethod::ReverseOrder:
                stack_ = std::stack<pointer>();
                stack_.push(nullptr);
                stack_.push(start);
                StackRightToNull();
                current_ = stack_.top(); stack_.pop();
                break;  // AVLTreeTraversalMethod::InOReverseOrderrder

            case AVLTreeTraversalMethod::TopDown:
                queue_ = std::queue<pointer>();
                current_ = start;
                if (current_ != nullptr) {
                    if (current_->GetLeft() != nullptr) {
                        queue_.push(current_->GetLeft());
                    }
                    if (current_->GetRight() != nullptr) {
                        queue_.push(current_->GetRight());
                    }
                }
                break;  // AVLTreeTraversalMethod::TopDown

            default:
                break;
            }  // switch
        }

        /**
         * Moves to the next Node, based on the traversal_method_
         */
        void MoveNext() {
            switch (traversal_method_) {
            case AVLTreeTraversalMethod::InOrder:
                if (current_->GetRight() != nullptr) {
                    stack_.push(current_->GetRight());
                    StackLeftToNull();
                }
                // std::stack doesn't return on .pop.
                // .top used to get value, then .pop to remove that value.
                current_ = stack_.top(); stack_.pop();
                break;  // AVLTreeTraversalMethod::InOrder

            case AVLTreeTraversalMethod::ReverseOrder:
                if (current_->GetLeft() != nullptr) {
                    stack_.push(current_->GetLeft());
                    StackRightToNull();
                }
                // std::stack doesn't return on .pop.
                // .top used to get value, then .pop to remove that value.
                current_ = stack_.top(); stack_.pop();
                break;  // AVLTreeTraversalMethod::ReverseOrder

            case AVLTreeTraversalMethod::TopDown:
                if (!queue_.empty()) {
                    // std::queue doesn't return on .pop.
                    // .front used to get value, then .pop to remove that value.
                    current_ = queue_.front();  queue_.pop();
                    if (current_->GetLeft() != nullptr) {
                        queue_.push(current_->GetLeft());
                    }
                    if (current_->GetRight() != nullptr) {
                        queue_.push(current_->GetRight());
                    }
                } else {
                    current_ = nullptr;
                }
                break;  // AVLTreeTraversalMethod::TopDown

            default:
                break;
            }  // switch
        }

        /**
         * Helper function that adds all left nodes, from the node on the top
         *  of the stack, to the stack.
         */
        void StackLeftToNull() {
            // equiv to peek, std::stack doesn't return on .pop.
            // .top used to get value
            pointer current = stack_.top();
            if (current != nullptr) {
                current = current->GetLeft();
                while (current != nullptr) {
                    stack_.push(current);
                    current = current->GetLeft();
                }
            }
        }


        /**
         * Helper function that adds all right nodes, from the node on the top
         *  of the stack, to the stack.
         */
        void StackRightToNull() {
            // equiv to peek, std::stack doesn't return on .pop.
            // .top used to get value
            pointer current = stack_.top();
            if (current != nullptr) {
                current = current->GetRight();
                while (current != nullptr) {
                    stack_.push(current);
                    current = current->GetRight();
                }
            }
        }
    };

    /**
     * Returns an Iterator at the begining of the tree, given node root_.
     *  The Iterator constructor will move to the proper first node, given
     *  traversal_method_.
     * 
     * This function is needed for the C++ build it for (a : b) syntax.
     * 
     * @returns Iterator
     */
    Iterator begin() { return Iterator(root_, traversal_method_); }

    /**
     * Returns an Iterator past the end of the tree.
     * 
     * This function is needed for the C++ build it for (a : b) syntax.
     * 
     * @returns Iterator
     */
    Iterator end() { return Iterator(nullptr, traversal_method_); }
};

}  // namespace _11c_dev_collections
#endif  // SRC_AVLTREE_H_
