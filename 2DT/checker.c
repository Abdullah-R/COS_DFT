/*--------------------------------------------------------------------*/
/* checker.c                                                          */
/* Author: Diane Yang and Abdullah Ramadan                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "dynarray.h"
#include "checker.h"


/* Checks if the count invariant of Node n is equal to the total
   number of nodes in the directory tree.

   Returns true if the two counts match; returns false otherwise.
*/
static boolean Checker_nodeCount(Node n) {
   size_t count = 0;
   size_t c;

   if(n != NULL) {
      count = 1;
      /* Sample check on each non-root Node: Node must be valid */
      /* If not, pass that failure back up immediately */
      for(c = 0; c < Node_getNumChildren(n); c++)
      {
         count += Checker_nodeCount(Node_getChild(n, c));
      }
   }
   return count;
}

/* see checker.h for specification */
boolean Checker_Node_isValid(Node n) {
   Node parent;
   const char* npath;
   const char* ppath;
   const char* rest;
   const char* child1;
   const char* child2;
   size_t c;
   size_t i;
   size_t j;

   parent = Node_getParent(n);

   if(parent != NULL) {

      npath = Node_getPath(n);
      ppath = Node_getPath(parent);

      if( ppath == NULL ) {
         fprintf(stderr, "P is not NULL, but P's path is NULL\n");
         return FALSE;
      }

      /* Sample check that parent's path must be prefix of n's path */
      i = strlen(ppath);

      if(strncmp(npath, ppath, i)) {
         fprintf(stderr, "P's path is not a prefix of C's path\n");
         return FALSE;
      }

      /* Sample check that n's path after parent's path + '/'
         must have no further '/' characters */
      rest = npath + i;
      rest++;
      if(strstr(rest, "/") != NULL) {
         fprintf(stderr, "C's path has grandchild of P's path\n");
         return FALSE;
      }

      /* Sample check that the children of the  parent are in
         alphabetical order */
      for(c = 1; c < Node_getNumChildren(n); c++)
      {
         child1 = Node_getPath(Node_getChild(n, c-1));
         child2 = Node_getPath(Node_getChild(n, c));

         j = strlen( child1 );

         if( strncmp(child1, child2, j) > 0 ) {
            fprintf(stderr, "P's children are not in alphabetical"
                    " order\n");
            return FALSE;
         }
      }
   }

   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at n.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean Checker_treeCheck(Node n) {
   size_t c;

   if(n != NULL) {

      /* Sample check on each non-root Node: Node must be valid */
      /* If not, pass that failure back up immediately */
      if(!Checker_Node_isValid(n))
         return FALSE;

      for(c = 0; c < Node_getNumChildren(n); c++)
      {

         Node child = Node_getChild(n, c);

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if(!Checker_treeCheck(child))
            return FALSE;
      }
   }
   return TRUE;
}



/* see checker.h for specification */
boolean Checker_DT_isValid(boolean isInit, Node root, size_t count) {
   size_t i, nodeCount = 1;

   /* Sample check on a top-level data structure invariants:
      if the DT is not initialized, its count should be 0 and root
      should be NULL. */
   if(!isInit)
   {
      if(count != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }
      if( root != NULL ) {
         fprintf(stderr, "Not initialized, but root is not equal to"
                 " NULL\n");
         return FALSE;
      }
   }

   /* Sample check on a top-level data structure invariants:
      if the DT is initialized, its count should be 0 and root
      should be NULL. */
   if(isInit)
   {
      if( root != NULL && count == 0 ) {
         fprintf(stderr, "Initialized and empty, but root is not equal"
                 "to NULL\n");
         return FALSE;
      }
      if( count != 0 && root == NULL ) {
         fprintf(stderr, "Initialized and empty, but count is not"
                 "equal to 0\n");
         return FALSE;
      }
   }

   if( root != NULL){

      for(i = 0; i < Node_getNumChildren(root); i++){
         nodeCount += Checker_nodeCount(Node_getChild(root, i));
      }

      /* Sample check that the number of nodes is equal to count
         invariant */
      if( nodeCount != count){
         fprintf(stderr, "Number of nodes is not equal to count");
         return FALSE;
      }

      if(Node_getPath(root) == NULL){
         fprintf(stderr, "Root contains a NULL path");
         return FALSE;
      }

      /* Sample check that root path does not contain '/' */
      if( strstr( Node_getPath(root), "/") != NULL ){
         fprintf(stderr, "Root contains a '/' character");
         return FALSE;
      }
   }

   /* Now checks invariants recursively at each Node from the root. */
   return Checker_treeCheck(root);
}
