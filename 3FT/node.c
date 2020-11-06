/*--------------------------------------------------------------------*/
/* node.c                                                             */
/* Author: Christopher Moretti                                        */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "dynarray.h"
#include "node.h"


struct fileS {
   void *contents;
   size_t length;
};

/*
   A node structure represents either a file or a          void *contents;
         size_t length;ectory in a 
   file tree
*/
struct node {
   /* the full path of this node */
   char* path;

   /* the parent directory of this node
      NULL for the root of the file tree */
   Node parent;

   /* defines whether the node at this path is
      a file or a directory */
   nodeType type;

   /* Either holds the subdirectories of 
   this node stored in sorted order 
   by pathname or the file contents*/
   union {
      DynArray_T children;
      struct fileS file;
   } storage;
};


/*
  returns a path with contents
  n->path/dir
  or NULL if there is an allocation error.

  Allocates memory for the returned string,
  which is then owened by the caller!
*/
static char* Node_buildPath(Node n, const char* name) {
   char* path;

   assert(name != NULL);

   if(n == NULL)
      path = malloc(strlen(name)+1);
   else
      path = malloc(strlen(n->path) + 1 + strlen(name) + 1);

   if(path == NULL)
      return NULL;
   *path = '\0';

   if(n != NULL) {
      strcpy(path, n->path);
      strcat(path, "/");
   }
   strcat(path, name);

   return path;
}

/* see node.h for specification */
Node Node_create(const char* name, Node parent, nodeType type){

   Node new;

   assert(name != NULL);

   new = malloc(sizeof(struct node));
   if(new == NULL)
      return NULL;

   new->path = Node_buildPath(parent, name);

   if(new->path == NULL) {
      free(new);
      return NULL;
   }

   new->parent = parent;
   new->type = type;

   if(type == DIRECTORY){
      new->storage.children = DynArray_new(0);
      if(new->storage.children == NULL) {
         free(new->path);
         free(new);
         return NULL;
      }
   }
   return new;
}

/* adds a pointer to the contents of a file, *contents,
   to a file-type node, n */
void Node_addFile(Node n, void *contents) {
   assert(n->type == FILE_S);
   assert(contents != NULL);

   n->storage.file.contents = contents;
}

/* see node.h for specification */
size_t Node_destroy(Node n) {
   size_t i;
   size_t count = 0;
   Node c;

   assert(n != NULL);
   
   if(n->type == DIRECTORY){
      for(i = 0; i < DynArray_getLength(n->storage.children); i++)
         {
            c = DynArray_get(n->storage.children, i);
            count += Node_destroy(c);
         }
      DynArray_free(n->storage.children);
   }

   free(n->path);
   free(n);
   
   count++;

   return count;
}

/* see node.h for specification */
/*char* Node_getPath(Node n) {
   char* pathCopy;

   assert(n != NULL);

   pathCopy = malloc(strlen(n->path)+1);
   if(pathCopy == NULL)
      return NULL;

   return strcpy(pathCopy, n->path);
}
*/
const char* Node_getPath(Node n) {

   assert(n != NULL);
   return n->path;
}

/* see node.h for specification */
int Node_compare(Node node1, Node node2) {
   assert(node1 != NULL);
   assert(node2 != NULL);

   if(node1->type != node2->type){
      /* If node1 is a FILE_S, it will return -1 otherwise node1 is 
         a DIRECTORY and node2 is a file so it will return 1 */
      return (node1->type)?-1:1;
   }

   return strcmp(node1->path, node2->path);
}



/* see node.h for specification */
size_t Node_getNumChildren(Node n) {
   assert(n != NULL);

   /* If n is a file, it will return 0, otherwise it will return the 
      numhber of children*/
   return (n->type)? 0 : DynArray_getLength(n->storage.children);
}

/* see node.h for specification */
int Node_hasChild(Node n, const char* path, nodeType type) {
   size_t index;
   int result;
   Node checker;

   assert(n != NULL);
   assert(path != NULL);
   assert(n->type != FILE_S);

   checker = Node_create(path, NULL, type);
   if(checker == NULL)
      return -1;
   result = DynArray_bsearch(n->storage.children, checker, &index,
                    (int (*)(const void*, const void*)) Node_compare);
   (void) Node_destroy(checker);

   return result;
}

/* see node.h for specification */
Node Node_getChild(Node n, size_t childID) {
   assert(n != NULL);
   assert(n->type == DIRECTORY);

   if(DynArray_getLength(n->storage.children) > childID)
      return DynArray_get(n->storage.children, childID);
   else
      return NULL;
}

/* see node.h for specification */
Node Node_getParent(Node n) {
   assert(n != NULL);

   return n->parent;
}

/* see node.h for specification */
int Node_linkChild(Node parent, Node child) {
   size_t i;
   char* rest;

   assert(parent != NULL);
   assert(parent->type == DIRECTORY);
   assert(child != NULL);

   if(Node_hasChild(parent, child->path, NULL))
      return ALREADY_IN_TREE;
   i = strlen(parent->path);
   if(strncmp(child->path, parent->path, i))
      return PARENT_CHILD_ERROR;
   rest = child->path + i;
   if(strlen(child->path) >= i && rest[0] != '/')
      return PARENT_CHILD_ERROR;
   rest++;
   if(strstr(rest, "/") != NULL)
      return PARENT_CHILD_ERROR;

   child->parent = parent;

   if(DynArray_bsearch(parent->storage.children, child, &i,
         (int (*)(const void*, const void*)) Node_compare) == 1)
      return ALREADY_IN_TREE;

   if(DynArray_addAt(parent->storage.children, i, child) == TRUE)
      return SUCCESS;
   else
      return PARENT_CHILD_ERROR;
}

/* see node.h for specification */
int  Node_unlinkChild(Node parent, Node child) {
   size_t i;

   assert(parent != NULL);
   assert(parent->type == DIRECTORY);
   assert(child != NULL);

   if(DynArray_bsearch(parent->storage.children, child, &i,
         (int (*)(const void*, const void*)) Node_compare) == 0)
      return PARENT_CHILD_ERROR;

   (void) DynArray_removeAt(parent->storage.children, i);
   return SUCCESS;
}


/* see node.h for specification */
int Node_addChild(Node parent, const char* name, nodeType type) {
   Node new;
   int result;

   assert(parent != NULL);
   assert(parent->type == DIRECTORY);
   assert(name != NULL);

   new = Node_create(name, parent, type);
   if(new == NULL)
      return PARENT_CHILD_ERROR;

   result = Node_linkChild(parent, new);
   if(result != SUCCESS)
      (void) Node_destroy(new);

   return result;
}

/* See node.h for specification */
void Node_insertFileContents(Node n, void *contents, size_t length){
   assert(n != NULL);
   assert(n->type == FILE_S);

   n->storage.file.contents = contents;
   n->storage.file.length = length;

}

/* See node.h for specification */
void *Node_getFileContents(Node n){
   assert(n != NULL);
   assert(n->type == FILE_S);

   return n->storage.file.contents;
}

/* See node.h for specification */
size_t Node_getFileLength(Node n){
   assert(n != NULL);
   assert(n->type == FILE_S);

   return n->storage.file.length;
}

/* See node.h for specification */
nodeType Node_getType(Node n){
   assert(n != NULL);
   return n->type;
}

/* see node.h for specification */
char* Node_toString(Node n) {
   char* copyPath;

   assert(n != NULL);

   copyPath = malloc(strlen(n->path)+1);
   if(copyPath == NULL)
      return NULL;
   else
      return strcpy(copyPath, n->path);
}

