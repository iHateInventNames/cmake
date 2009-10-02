#include "cmGlobalSymbianMmpGenerator.h"
#include "cmLocalSymbianMmpGenerator.h"
#include "cmCacheManager.h"
#include "cmMakefile.h"
#include "cmSystemTools.h"
#include "cmake.h"

#include <fstream>

cmGlobalSymbianMmpGenerator::cmGlobalSymbianMmpGenerator()
{
  this->FindMakeProgramFile = "CMakeSymbianFindMake.cmake";
}

cmLocalGenerator *cmGlobalSymbianMmpGenerator::CreateLocalGenerator()
{
    cmLocalSymbianMmpGenerator* lg = new cmLocalSymbianMmpGenerator;
    lg->SetGlobalGenerator(this);
    return lg;
}

static int depth(cmLocalGenerator* g)
{
  cmLocalGenerator* parent = g->GetParent();
  int result = 0;

  while(parent)
    {
    ++result;
    parent = parent->GetParent();
    }
  return result;
}

static bool less_depth(cmLocalGenerator* g1, cmLocalGenerator* g2)
{
  return depth(g1) < depth(g2);
}

void cmGlobalSymbianMmpGenerator::Generate()
{
  cmGlobalGenerator::Generate();

  std::string filename = GetCMakeInstance()->GetStartOutputDirectory();
  filename += "/bld.inf";
  std::ofstream bld_inf(filename.c_str());
  bld_inf << "prj_mmpfiles" << std::endl;

  std::vector<std::string> basedir;
  cmSystemTools::SplitPath(GetCMakeInstance()->GetStartOutputDirectory(),
                           basedir);

  std::list<cmLocalGenerator*> locals;
  // sort local generators to the build order
  std::copy(LocalGenerators.begin(), LocalGenerators.end(),
            std::back_insert_iterator<std::list<cmLocalGenerator*> >(locals));
  locals.sort(less_depth);
  locals.reverse();

  std::list<cmLocalGenerator*>::const_iterator i;
  for (i = locals.begin(); i != locals.end(); ++i)
    {
    cmTargets targets = (*i)->GetMakefile()->GetTargets();
    
    for (cmTargets::iterator t = targets.begin(); t != targets.end(); ++t)
      {
      cmLocalGenerator* local = *i;

      if (t->second.GetType() == cmTarget::UTILITY)
          bld_inf << "gnumakefile ";

      std::string prj = local->GetMakefile()->GetCurrentOutputDirectory();
      prj += (prj.length() > 0 ? "/" : "");
      prj += t->first;
      prj += (t->second.GetType() == cmTarget::UTILITY ? "Utils.mk" : ".mmp");
      std::string componentPath = local->ConvertToRelativePath(basedir, prj.c_str());
      bld_inf << cmSystemTools::ConvertToOutputPath(componentPath.c_str());
      bld_inf << std::endl;

      if (t->second.GetType() != cmTarget::UTILITY)
        {
        size_t customStepsCount = t->second.GetPreBuildCommands().size() +
                                  t->second.GetPreLinkCommands().size() +
                                  t->second.GetPostBuildCommands().size();
        if (customStepsCount > 0)
          {
              std::string helperMk = local->GetMakefile()->GetCurrentOutputDirectory();
              helperMk += (helperMk.length() > 0 ? "/" : "");
              helperMk += t->first;
              helperMk += "Utils.mk";
              bld_inf << "gnumakefile ";
              std::string helperPath = local->ConvertToRelativePath(basedir,
                                                                    helperMk.c_str());
              bld_inf << cmSystemTools::ConvertToOutputPath(helperPath.c_str());
              bld_inf << std::endl;
          }
        }
      }
    }
}

void cmGlobalSymbianMmpGenerator
::GetDocumentation(cmDocumentationEntry& entry) const
{
  entry.Name  = this->GetName();
  entry.Brief = "Generates Symbian SDK .mmp project files.";
  entry.Full  =
      "The .mmp files can be used to build a project in usual Symbian "
      "way using SDK tools.";
}