#include "util/TFolderManip.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <dirent.h>
  #include <sys/stat.h>
#endif

namespace BlackT {


std::string TFolderManip::getDirectory(std::string filepath) {
  for (int i = filepath.size() - 1; i >= 0; i--) {
    if (filepath[i] == '/') {
      return filepath.substr(0, i);
    }
  }
  
  return std::string();
}

std::string TFolderManip::getFilename(std::string filepath) {
  for (int i = filepath.size() - 1; i >= 0; i--) {
    if (filepath[i] == '/') {
      return filepath.substr(i + 1, filepath.size() - (i + 1));
    }
  }
  
  return std::string();
}

#ifdef _WIN32
#else
  void TFolderManip::getFileListing(const char* dirname,
                         NameCollection& dst) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) return;
    
    dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
      struct stat buf;
      stat((std::string(dirname) + "/" + dirent->d_name).c_str(), &buf);
      if (S_ISREG(buf.st_mode)) {
        dst.push_back(std::string(dirent->d_name));
      }
    }
    
    closedir(dir);
  }
#endif
  
#ifdef _WIN32
#else
  void TFolderManip::getSubFolderListing(const char* dirname,
                                  NameCollection& dst) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) return;
    
    dirent* dirent;
    while ((dirent = readdir(dir)) != NULL) {
      struct stat buf;
      stat((std::string(dirname) + "/" + dirent->d_name).c_str(), &buf);
      if (S_ISDIR(buf.st_mode)) {
        // don't return current/previous directory entries
        if ((strcmp(dirent->d_name, ".") != 0)
            && (strcmp(dirent->d_name, "..") != 0)) {
          dst.push_back(std::string(dirent->d_name));
        }
      }
    }
    
    closedir(dir);
  }
#endif

void TFolderManip::createDirectory(std::string name) {
  if (name.size() != 0) {
    // first, create every parent directory, since mkdir
    // will not create subdirectories
    createDirectory(getDirectory(name));
    
    #ifdef _WIN32
	    CreateDirectoryA(name.c_str(), NULL);
    #else
      mkdir(name.c_str(), S_IRWXU|S_IRWXG|S_IRWXO);
    #endif
  }
}

void TFolderManip::createDirectoryForFile(std::string name) {
  createDirectory(getDirectory(name));
}


} 
