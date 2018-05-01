#ifndef TFOLDERMANIP_H
#define TFOLDERMANIP_H


#include <string>
#include <vector>

namespace BlackT {


  
/**
 * Folder manipulation functions.
 *
 * NOTE: Currently, these functions are restricted to Unix-like systems.
 * Trying to use them on other systems will produce compile or link errors.
 */
class TFolderManip {
public:
  typedef std::vector<std::string> NameCollection;

  static std::string getDirectory(std::string filepath);

  static std::string getFilename(std::string filepath);
  
  /**
   * Fills dst with a list of all files (not directories) in directory dirname.
   */
  static void getFileListing(const char* dirname,
                       NameCollection& dst);
  
  /**
   * Fills dst with a list of all subdirectories in directory dirname.
   */
  static void getSubFolderListing(const char* dirname,
                                NameCollection& dst);

  static void createDirectory(std::string name);

  static void createDirectoryForFile(std::string name);
  
protected:
  
};


}


#endif
