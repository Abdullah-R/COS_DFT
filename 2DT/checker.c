/*--------------------------------------------------------------------*/
/* checker.c                                                          */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "dynarray.h"
#include "checker.h"


/* see checker.h for specification */
boolean Checker_Node_isValid(Node n) {
   Node parent;
   const char* npath;
   const char* ppath;
   const char* rest;
   size_t i;

   /* Sample check: a NULL pointer is not a valid Node */
   /* if(n == NULL) {
      fprintf(stderr, "Node is a NULL pointer\n");
      return FALSE;
      } */

   parent = Node_getParent(n);

   if(parent != NULL) {
      npath = Node_getPath(n);

      /* Sample check that parent's path must be prefix of n's path */
      ppath = Node_getPath(parent);
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

   /*
   if(n == NULL) {
      fprintf(stderr, "Root is NULL\n");
   }
   */

   if(n != NULL) {
      fprintf(stderr, "test\n");
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

   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0. */

   /* fprintf(stderr, "%d\n", (int)root); */

   if(!isInit)
   {
      if(count != 0) {
         fprintf(stderr, "Not initialized, but count is not 0\n");
         return FALSE;
      }
      if( root != NULL ) {
         fprintf(stderr, "Not initialized, but root is not equal to NULL\n");
         return FALSE;
      }
   }

   if( count == 0 && isInit )
   {
      if( root != NULL ) {
         fprintf(stderr, "Initialized and empty, but root is not equal to NULL\n");
         return FALSE;
      }
   }

   if( root == NULL && isInit ){
      if( count != 0 ) {
         fprintf(stderr, "Initizlied and empty, but count is not equal to 0\n");
         return FALSE;
      }
   }

   if( count == 0 && Node_getNumChildren(root) != 0 ){
      fprintf(stderr, "Children array is not empty \n");
      return FALSE;
   }

   /* Now checks invariants recursively at each Node from the root. */
   return Checker_treeCheck(root);
}
