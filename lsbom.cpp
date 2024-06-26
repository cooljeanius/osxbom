// lsbom.cpp -*- C++ -*-
// This code implements a clone of the NextSTEP/OSX lsbom utility
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

#ifdef HAVE_CONFIG_H
# include "config.h"
# if !defined(VERSION_STRING)
#  if defined(PACKAGE_STRING)
#   define VERSION_STRING PACKAGE_STRING
#  else
#   if !defined(VERSION_STRING_NAME_PART)
#    if defined(PACKAGE)
#     define VERSION_STRING_NAME_PART PACKAGE
#    elif defined(PACKAGE_NAME)
#     define VERSION_STRING_NAME_PART PACKAGE_NAME
#    elif defined(PACKAGE_TARNAME)
#     define VERSION_STRING_NAME_PART PACKAGE_TARNAME
#    else
#     define VERSION_STRING_NAME_PART "lsbom"
#    endif // PACKAGE || PACKAGE_NAME || PACKAGE_TARNAME
#   endif // !VERSION_STRING_NAME_PART
#   if !defined(VERSION_STRING_VERSION_PART)
#    if defined(PACKAGE_VERSION)
#     define VERSION_STRING_VERSION_PART PACKAGE_VERSION
#    elif defined(VERSION)
#     define VERSION_STRING_VERSION_PART VERSION
#    else
#     define VERSION_STRING_VERSION_PART "0.0.x"
#    endif // PACKAGE_VERSION || VERSION
#   endif // !VERSION_STRING_VERSION_PART
#   define VERSION_STRING VERSION_STRING_NAME_PART VERSION_STRING_VERSION_PART
#  endif // PACKAGE_STRING
# endif // !VERSION_STRING
#else
# define LSBOM_CPP_NON_AUTOTOOLS_BUILD 1
#endif // HAVE_CONFIG_H

#include "bom.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

// NOTE: Windows is missing several of these headers:
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> // For getopt() on some platforms
#ifdef HAVE_GETOPT_H
# include <getopt.h>
#endif // HAVE_GETOPT_H
#include <ctype.h>

/* GCC 2.5 and later versions define a function attribute "noreturn",
 * which is the preferred way to declare that a function never returns.
 * However GCC 2.7 appears to be the first version in which this fully
 * works everywhere we use it. */

#ifndef ATTR_NORETURN
# if defined(__GNUC__) && ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ >= 7)))
#  define ATTR_NORETURN __attribute__((noreturn))
# else
#  define ATTR_NORETURN /* nothing */
# endif /* __GNUC__ version check */
#endif /* !ATTR_NORETURN */

using namespace std;

// Pass -D to enable debug outputs:
#define DEBUG(level, msg)                                               \
  if (level <= debug) {                                                 \
    cout << "DEBUG(" << level << "): " << msg << endl;                  \
  }

enum list_type_e {
  LIST_FILES = 1 << 0,
  LIST_DIRS  = 1 << 1,
  LIST_LINKS = 1 << 2,
  LIST_BDEVS = 1 << 3,
  LIST_CDEVS = 1 << 4,
  LIST_ALL = 0x1f
};


static int debug = 0;
static char *data;
static BOMIndexHeader *indexHeader;

char *lookup(int i, uint32_t *length = (uint32_t *)NULL) {
  BOMIndex *index = (indexHeader->index + (int)ntohl((uint32_t)i));
  if (length) {
    *length = ntohl(index->length);
  }
  uint32_t addr = ntohl(index->address);

  DEBUG(2, "@ index=0x" << hex << ntohl((uint32_t)i)
           << " addr=0x" << hex << setw(4) << setfill('0') << addr
           << " len=" << dec << ntohl(index->length));

  return (data + addr);
}


void short_usage(void) {
  cout << "Usage: lsbom [-h] [-s] [-f] [-d] [-l] [-b] [-c] [-m] [-x]\n"
       << "\t"
#if 0
    "[--arch archVal] "
#endif // 0
    "[-p parameters] bom ..." << endl;
}


void ATTR_NORETURN usage_error(const char *msg) {
  cout << msg << endl;
  short_usage();
  exit(1);
}


void usage(void) {
  short_usage();
  cout << "\n"
    "\t-h              print full usage\n"
    "\t-s              print pathnames only\n"
    "\t-f              list files\n"
    "\t-d              list directories\n"
    "\t-l              list symbolic links\n"
    "\t-b              list block devices\n"
    "\t-c              list character devices\n"
    "\t-m              print modified times\n"
    "\t-x              suppress modes for directories and symlinks\n"
#if 0
    "\t--arch archVal  print info for architecture archVal (\"ppc\", "
    "\"i386\", \"hppa\", \"sparc\", etc)\n"
#endif // 0
#if !defined(__APPLE__)
    "\t-v              print version info"
#endif /* !__APPLE__ */
    "\t-p parameters   print only some of the results.  EACH OPTION CAN "
    "ONLY BE USED ONCE\n"
    "\t\tParameters:\n"
    "\t\t\tf    file name\n"
    "\t\t\tF    file name with quotes (i.e. \"/usr/bin/lsbom\")\n"
    "\t\t\tm    file mode (permissions)\n"
    "\t\t\tM    symbolic file mode\n"
    "\t\t\tg    group id\n"
    "\t\t\tG    group name\n"
    "\t\t\tu    user id\n"
    "\t\t\tU    user name\n"
    "\t\t\tt    mod time\n"
    "\t\t\tT    formatted mod time\n"
    "\t\t\ts    file size\n"
    "\t\t\tS    formatted size\n"
    "\t\t\tc    32-bit checksum\n"
    "\t\t\t/    user id/group id\n"
    "\t\t\t?    user name/group name\n"
    "\t\t\tl    link name\n"
    "\t\t\tL    quoted link name\n"
    "\t\t\t0    device type\n"
    "\t\t\t1    device major\n"
    "\t\t\t2    device minor\n"
       << flush;
}


void ATTR_NORETURN error(const char *msg) {
  cerr << msg << endl;
  exit(1);
}

// The Xcode project should have 'USE_AGV' defined in its preprocessor
// macros entry:
#if defined(__APPLE__) && defined(USE_AGV)
extern const unsigned char osxbomVersionString[];
extern const double osxbomVersionNumber;
#endif /* __APPLE__ && USE_AGV */

void version(void) {
#if defined(__APPLE__) && defined(USE_AGV)
  printf("version: ");
  if (*osxbomVersionString) {
    printf("%s ", osxbomVersionString);
  }
  if (osxbomVersionNumber) {
    printf("%f", osxbomVersionNumber);
  }
#else
# if defined(HAVE_CONFIG_H) && defined(VERSION_STRING)
  printf("version: %s", VERSION_STRING);
# endif /* HAVE_CONFIG_H */
#endif /* __APPLE__ && USE_AGV */
  cout << "\n";
}


int main(int argc, char *argv[]) {
  bool suppressDirSimModes = false;
  bool suppressDevSize = false;
  bool pathsOnly = false;
  int listType = 0;
  char params[16] = "";

  ios_base::Init *__ioinit = new ios_base::Init;

  /* FIXME: see comment below: */
  if ((argv == NULL) || (argv[1] == NULL) || (strncmp(argv[1], "--help", 8UL) == 0)) {
    delete __ioinit; usage(); return 0;
  } else if (strncmp(argv[1], "--version", 16UL) == 0) {
    delete __ioinit; version(); return 0;
  }

  while (true) {
    // FIXME: switch to getopt_long():
    char c = (char)getopt(argc, argv, "hsfdlbcmxvp:D::");
    if (c == -1) {
      break;
    } else {
      if (debug >= 2) {
        printf("c is '%c'\n", c);
      }
    }

    switch (c) {
      case 'h': usage(); exit(0); break;
      case 's': pathsOnly = true; break;
      case 'f': listType |= LIST_FILES; break;
      case 'd': listType |= LIST_DIRS; break;
      case 'l': listType |= LIST_LINKS; break;
      case 'b': listType |= LIST_BDEVS; break;
      case 'c': listType |= LIST_CDEVS; break;
      case 'm': strcat(params, "T"); break;
      case 'x': suppressDirSimModes = true; break;
      case 'v': version(); exit(0); break;
      case 'a': usage_error("--arch not supported");
      case 'p':
        if (optarg == NULL) {
          usage_error("NULL optarg");
        } else if (15 < strlen(optarg)) {
          usage_error("Too many parameters");
        }
        strcpy(params, optarg);
        break;
      case 'D':
        if (optarg) {
          debug = atoi(optarg);
        } else {
          debug++;
        }
        break;
      case ':': case '?':
        if (optopt) {
          switch (optopt) {
            case '-':
              continue;
            default:
              short_usage(); exit(1); break;
          }
        }
		/*FALLTHRU*/
      case '-': continue;
      default:;
    }
  }

  if (optind == argc) {
    usage();
    exit(1);
  }

  if (listType == 0) {
    listType = LIST_ALL;
  }
  if (!params[0]) {
    strcpy(params, "fm/scl0");
    suppressDevSize = true;
  }

  for (int i = optind; (i < argc); i++) {
    fstream f(argv[i], (ios::in | ios::binary));

    // Get file length:
    f.seekg(0, ios::end);
    streampos length = f.tellg();
    f.seekg(0);

    // Allocate space:
    data = new char[length];

    // Read data:
    f.read(data, length);

    if (f.fail()) {
      delete __ioinit;
      cerr << "Failed to read BOM file" << endl;
      return 1;
    }

    f.close();

    BOMHeader *header = (BOMHeader *)data;

    if (string(header->magic, 8) != "BOMStore") {
      delete __ioinit;
      cerr << "Not a BOM file: " << argv[i] << endl;
      return 1;
    }

    indexHeader = (BOMIndexHeader *)(data + ntohl(header->indexOffset));

    // Process vars:
    BOMVars *vars = (BOMVars *)(data + ntohl(header->varsOffset));
    char *ptr = (char *)vars->first;
    for (unsigned int ii = 0U; (ii < ntohl(vars->count)); (ii++)) {
      BOMVar *var = (BOMVar *)ptr;

      uint32_t varDataLength;
      char *varData = lookup((int)var->index, &varDataLength);
      BOMTree *tree = (BOMTree *)varData;
      string name = string(var->name, var->length);

      DEBUG(2, "BOMVar 0x" << hex << ntohl(var->index) << ' ' << name
               << ':');

      if (name == "Paths") {
        BOMPaths *paths = (BOMPaths *)lookup((int)tree->child);

        typedef map<uint32_t, string> filenames_t;
        typedef map<uint32_t, uint32_t> parents_t;
        filenames_t filenames;
        parents_t parents;

        while (paths->isLeaf == htons(0)) {
          paths = (BOMPaths *)lookup((int)paths->indices[0].index0);
        }

        while (paths) {
          for (unsigned int j = 0U; (j < ntohs(paths->count)); j++) {
            uint32_t index0;
            uint32_t index1;
            BOMFile *file;
            BOMPathInfo1 *info1;
            BOMPathInfo2 *info2;
            uint32_t length2;

            index0 = paths->indices[j].index0;
            index1 = paths->indices[j].index1;
            file = (BOMFile *)lookup((int)index1);
            info1 = (BOMPathInfo1 *)lookup((int)index0);
            info2 = (BOMPathInfo2 *)lookup((int)info1->index, &length2);

            // Compute full name:
            string filename = file->name;
            filenames[info1->id] = filename;
            if (file->parent) {
              parents[info1->id] = file->parent;
            }

            parents_t::iterator it = parents.find(info1->id);
            while (it != parents.end()) {
              filename = (filenames[it->second] + "/" + filename);
              it = parents.find(it->second);
            }

            // Check type:
            switch (info2->type) {
            case TYPE_FILE:
                if (!(LIST_FILES & listType)) {
                  continue;
                }
                break;
            case TYPE_DIR:
                if (!(LIST_DIRS & listType)) {
                  continue;
                }
                break;
            case TYPE_LINK:
                if (!(LIST_LINKS & listType)) {
                  continue;
                }
                break;
            case TYPE_DEV:
              {
                uint16_t mode = ntohs(info2->mode);
                bool isBlock = (mode & 0x4000);
                if (isBlock && !(LIST_BDEVS & listType)) {
                  continue;
                }
                if (!isBlock && !(LIST_CDEVS & listType)) {
                  continue;
                }
                break;
              }
            default:
                break;
            }

            if (pathsOnly) {
              cout << filename << '\n';
            } else {
              // Print requested parameters:
              bool printed = true;
              for (unsigned jj = 0; params[jj]; jj++) {
                if (jj && printed) {
                  cout << '\t';
                }
                printed = true;

                switch (params[jj]) {
                case 'f': cout << filename; continue;
                case 'F': cout << '"' << filename << '"'; continue;
                case 'g': cout << dec << ntohl(info2->group); continue;
                case 'G': error("Group name not yet supported");
                case 'u': cout << dec << ntohl(info2->user); continue;
                case 'U': error("User name not yet supported");
                case '/':
                  cout << dec << ntohl(info2->user) << '/'
                       << ntohl(info2->group);
                  continue;
                case '?': error("User/group name not yet supported");

                default:
                  if (!suppressDirSimModes ||
                      ((info2->type != TYPE_DIR) && (info2->type != TYPE_LINK))) {
                    switch (params[jj]) {
                      case 'm': cout << oct << ntohs(info2->mode); continue;
                      case 'M': error("Symbolic mode not yet supported");
                      default:;
                    }
                  }

                  if ((info2->type == TYPE_FILE) || (info2->type == TYPE_LINK)) {
                    switch (params[jj]) {
                      case 't': cout << dec << ntohl(info2->modtime); continue;
                      case 'T': error("Formated mod time not yet supported");
                      case 'c': cout << dec << ntohl(info2->checksum); continue;
                      default:;
                    }
                  }

                  if ((info2->type != TYPE_DIR) &&
                      (!suppressDevSize || (info2->type != TYPE_DEV))) {
                    switch (params[jj]) {
                      case 's': cout << dec << ntohl(info2->size); continue;
                      case 'S': error("Formated size not yet supported");
                      default:;
                    }
                  }

                  if (info2->type == TYPE_LINK) {
                    switch (params[jj]) {
                      case 'l': cout << info2->linkName; continue;
                      case 'L': cout << '"' << info2->linkName << '"'; continue;
                      default:;
                    }
                  }

                  if (info2->type == TYPE_DEV) {
                    uint32_t devType = ntohl(info2->devType);

                    switch (params[jj]) {
                      case '0': cout << dec << devType; continue;
                      case '1': cout << dec << (devType >> 24); continue;
                      case '2': cout << dec << (devType & 0xff); continue;
                      default:;
                    }
                  }
                }

                printed = false;
              } // end for-loop on 'jj'
            }
            cout << '\n';

            DEBUG(1, "id=0x" << hex << ntohl(info1->id) << ' '
                     << "parent=0x" << ntohl(file->parent) << ' '
                     << "type=" << dec << (unsigned)info2->type << ' '
                     << "unknown0=" << dec << (unsigned)info2->unknown0
                     << ' ' << "architecture=0x" << hex
                     << ntohs(info2->architecture) << ' ' << "unknown1="
                     << dec << (unsigned)info2->unknown1 << ' '
                     << "length2=" << dec << length2);

            if (3 < debug) {
              for (unsigned int k = 0U; (k < length2); k++) {
                if (k) {
                  if (((k % 16) == 0) || (k == (length2 - 1))) {
                    unsigned int len = (k % 16);
                    if (!len) {
                      len = 16;
                    }

                    if (len < 16) {
                      for (unsigned int l = 0U; (l < (16 - len)); l++) {
                        cout << "     ";
                      }
                      cout << ' ';
                    }

                    for (unsigned int l = (k - len); (l < k); l++) {
                      if ((l % 8) == 0) {
                        cout << ' ';
                      }

                      unsigned char c = ((unsigned char *)info2)[l];
                      if (isprint(c)) {
                        cout << (char)c;
                      } else {
                        cout << '.';
                      }
                    }

                    cout << '\n';
                  } else if ((k % 8) == 0) {
                    cout << ' ';
                  }
                }

                cout << "0x" << setfill('0') << setw(2) << hex
                     << (unsigned int)((unsigned char *)info2)[k] << ' ';
              }
            }
          } // end for-loop on 'j'

          if (paths->forward == htonl(0)) {
            paths = (BOMPaths *)NULL;
          } else {
            paths = (BOMPaths *)lookup((int)paths->forward);
          }
        }
      }

      ptr += (sizeof(BOMVar) + var->length);
    } // end for-loop on 'ii'
  } // end for-loop on 'i'

  cout << flush;

  delete __ioinit;

  return 0;
}

// EOF
