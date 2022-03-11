//
// Created by Gabriel Motta on 3/7/22.
//

#include "include/core/commandlineinput.hpp"
#include <algorithm>

Core::CommandLineInput::CommandLineInput(int &argc, char *argv[])
{
  for(int i = 1; i < argc; ++i)
    this->args.push_back(std::string(argv[i]));
}

std::pair<bool, std::string> Core::CommandLineInput::getValueForTag(const std::string &option, const std::string &shortoption) const
{
  auto itr = std::find(this->args.begin(), this->args.end(), option);
  if(itr != this->args.end() && (itr+1) != this->args.end())
  {
    return {true,*(itr+1)};
  } else if(itr != this->args.end()){
    return {true, {}};
  }

  itr = std::find(this->args.begin(), this->args.end(), shortoption);
  if(itr != this->args.end() && (itr+1) != this->args.end())
  {
    return {true,*(itr+1)};
  } else if(itr != this->args.end()){
    return {true, {}};
  }

  return {false,{}};
}

bool Core::CommandLineInput::tagExists(const std::string &option) const
{
  return std::find(this->args.begin(), this->args.end(), option) != this->args.end();
}

std::string Core::CommandLineInput::getValueThatEndsWith(const std::string &substr)
{
  for (auto& entry : args){
    if(entry.length() > substr.length())
    {
      if(0 == entry.compare(entry.length() - substr.length(), substr.length(), substr))
      {
        return entry;
      }
    }
  }
  return {};
}
