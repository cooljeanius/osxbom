// bom.h

#ifndef OSX_BOM_H
#define OSX_BOM_H 1

//
// These structure define part of the NextSTEP/OSX BOM file format
//
// Initial code
// Author: Joseph Coffland
// Date: October, 2011
//
// Additional work on BOMPath & BOMTree
// Author: Julian Devlin
// Date: October, 2012
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// This program is in the public domain.
//

#include <stdint.h>

struct BOMHeader {
  char magic[8]; // = BOMStore
  uint32_t unknown0; // = 1?
  uint32_t unknown1; // = 73 = 0x49?
  uint32_t indexOffset; // Length of first part
  uint32_t indexLength; // Length of second part
  uint32_t varsOffset;
  uint32_t trailerLen; // FIXME: What does this data at the end mean?
} __attribute__((packed));


struct BOMIndex {
  uint32_t address;
  uint32_t length;
} __attribute__((packed));


struct BOMIndexHeader {
  uint32_t unknown0; // FIXME: What is this? It is not the length of the array...
  BOMIndex index[];
} __attribute__((packed));


struct BOMTree { // 21 bytes
  char tree[4]; // = "tree"
  uint32_t unknown0;
  uint32_t child; // FIXME: Not sure about this one...
  uint32_t nodeSize; // byte count of each entry in the tree (BOMPaths)
  uint32_t pathCount; // total number of paths in all leaves combined
  uint8_t unknown3;
} __attribute__((packed));


struct BOMVar {
  uint32_t index;
  uint8_t length;
  char name[]; // length
} __attribute__((packed));


struct BOMVars {
  uint32_t count; // Number of entries that follow
  BOMVar first[];
} __attribute__((packed));


struct BOMPathIndices {
  uint32_t index0;
  uint32_t index1;
} __attribute__((packed));


struct BOMPaths {
  uint16_t isLeaf; // if 0 then this entry refers to other BOMPaths entries
  uint16_t count;  // for leaf, count of paths. for top level, (# of leafs - 1)
  uint32_t forward;  // next leaf, when there are multiple leafs
  uint32_t backward; // previous leaf, when there are multiple leafs
  BOMPathIndices indices[];
} __attribute__((packed));


enum {
  TYPE_FILE = 1, // BOMPathInfo2 is exe=88 regular=35 bytes
  TYPE_DIR  = 2, // BOMPathInfo2 is 31 bytes
  TYPE_LINK = 3, // BOMPathInfo2 is 44? bytes
  TYPE_DEV  = 4, // BOMPathInfo2 is 35 bytes
};


// Not sure of all the corect values here:
enum {
  ARCH_PPC = 0,
  ARCH_I386 = 1 << 12,
  ARCH_HPPA = 0,
  ARCH_SPARC = 0,
};


struct BOMPathInfo2 {
  uint8_t type; // See types above
  uint8_t unknown0; // = 1?
  uint16_t architecture; // Not sure exactly what this means...
  uint16_t mode;
  uint32_t user;
  uint32_t group;
  uint32_t modtime;
  uint32_t size;
  uint8_t unknown1; // = 1?
  union {
    uint32_t checksum;
    uint32_t devType;
  };
  uint32_t linkNameLength;
  char linkName[];

  // FIXME: executable files have a buch of other crap here:
} __attribute__((packed));


struct BOMPathInfo1 {
  uint32_t id;
  uint32_t index; // Pointer to BOMPathInfo2
} __attribute__((packed));


struct BOMFile {
  uint32_t parent; // Parent BOMPathInfo1->id
  char name[];
} __attribute__((packed));

// prototypes:
char *lookup(int i, uint32_t *length);
void short_usage(void);
void usage_error(const char *msg);
void usage(void);
void error(const char *msg);
void version(void);

#endif // OSX_BOM_H

// EOF
