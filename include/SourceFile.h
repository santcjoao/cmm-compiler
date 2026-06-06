#ifndef __SourceFile_h
#define __SourceFile_h

#include <string>

class SourceFile {
public:
  SourceFile(const std::string &filename);

  const std::string &getSource() const;
  const std::string &getFilename() const;
  std::string getLineContent(int lineNumber) const;
  bool isValid() const;

private:
  std::string filename;
  std::string source;
  bool valid;
};

#endif
