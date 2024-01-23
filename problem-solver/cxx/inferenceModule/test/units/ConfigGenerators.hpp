#pragma once

#include "inferenceConfig/InferenceConfig.hpp"

namespace inference::generatorTest
{

class ConfigGenerator
{
public:
  virtual InferenceConfig getInferenceConfig(InferenceConfig inferenceConfig) const
  {
    return inferenceConfig;
  }

  virtual std::string getName() const
  {
    return "ConfigGenerator";
  }
};

class ConfigGeneratorSearchWithoutReplacements : public ConfigGenerator
{
public:
  virtual InferenceConfig getInferenceConfig(InferenceConfig inferenceConfig) const override
  {
    inferenceConfig.atomicLogicalFormulaSearchBeforeGenerationType = SEARCH_WITHOUT_REPLACEMENTS;
    return inferenceConfig;
  }

  virtual std::string getName() const override
  {
    return "ConfigGeneratorSearchWithoutReplacements";
  }
};

class ConfigGeneratorSearchWithReplacements : public ConfigGenerator
{
public:
  virtual InferenceConfig getInferenceConfig(InferenceConfig inferenceConfig) const override
  {
    inferenceConfig.atomicLogicalFormulaSearchBeforeGenerationType = SEARCH_WITHOUT_REPLACEMENTS;
    return inferenceConfig;
  }

  virtual std::string getName() const override
  {
    return "ConfigGeneratorSearchWithReplacements";
  }
};

}  // namespace inference::generator
