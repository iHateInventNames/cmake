#ifndef cmLocalSymbianMmpGenerator_h
#define cmLocalSymbianMmpGenerator_h

#include "cmLocalGenerator.h"
#include "cmTarget.h"

class cmLocalSymbianMmpGenerator : public cmLocalGenerator
{
public:
  void Generate();

private:
  void writeMmp(cmTarget& target);
  void writeTargetType(cmTarget& target, std::ostream& mmp);

  std::string addGenericOption(cmTarget& target, const std::string& option,
                               std::ostream& mmp);
  void addIncludes(std::ostream& mmp);
  void addResources(cmTarget& target, std::ostream& mmp);
  void addSources(cmTarget& target, std::ostream& mmp);
  void addLibraries(cmTarget& target, std::ostream& mmp);
  bool addMacro(std::ostream& mmp, const char* macros);
  void addDefinitions(cmTarget& target, std::ostream& mmp);

  void writeGenericResource(const cmSymbianResource& res, std::ostream& mmp);
  void writeBitmap(const cmSymbianResource& res, std::ostream& mmp);

  static std::string keyword_with_param(const std::string& option);
  static std::string targettype_extension(const std::string& targettype);

  void writeMakefile(cmTarget& target);

  static void replaceSemicolonsWithSpaces(std::string& s);
};

#endif
