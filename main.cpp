#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <format>

const std::filesystem::path HashPath = ".hash";

size_t Hash;

bool HasChanged() {

  size_t OldHash = 0;
  {
    std::ifstream HashFile(HashPath);
    if (HashFile) {
      HashFile >> OldHash;
    }
  }

  std::cout << "Last Hash:    " << OldHash << " read from "
            << std::filesystem::absolute(HashPath) << "\n";

  std::cout << "Current Hash: ";

  std::stringstream HashString;
  for (auto de :
       std::filesystem::directory_iterator(std::filesystem::path("Source"))) {
    if (de.is_regular_file()) {
      HashString << static_cast<long long>(
          de.last_write_time().time_since_epoch().count());
    }
  }

  Hash = std::hash<std::string>{}(HashString.str());

  std::cout << Hash << "\n";

  return OldHash != Hash;
}

int main([[maybe_unused]]int argc, char *argv[]) {

  if(argc != 2){
    std::cout << std::format("Usage: {} direcotry\n", argv[0]);
    exit(-1);
  }

  const auto path = std::filesystem::path(argv[1]);

  if(!std::filesystem::is_directory(path)) {
    std::cout << std::format("{} is not a directory\n", path.string());
    exit(-1);
  }

  std::filesystem::current_path(path);

  std::cout << "PreprocessPngs:\n";

  if (!HasChanged()) {
    std::cout << "All Up To Date\n";
    return 0;
  }

  std::unordered_set<std::string> Names;

  for (auto de :
       std::filesystem::directory_iterator(std::filesystem::path("Source"))) {
    if (de.is_regular_file()) {

      std::filesystem::path Destination("PngHeaders");
      Destination /= de.path().stem().string() + ".hpp";

      if (!std::filesystem::exists(Destination.parent_path())) {
        std::filesystem::create_directories(Destination.parent_path());
      }

      std::filesystem::path Source("Source");
      Source /= de.path().filename();

      // std::stringstream s;

      // #ifdef _WIN32
      //			std::string xxdCommand = "xxd.exe";
      // #else
      //			std::string xxdCommand = "xxd";
      // #endif
      std::ifstream i(Source, std::ios_base::binary);
      std::ofstream o(Destination);

      std::string CoreName = de.path().filename().stem().string();

      o << "static const unsigned char Source_" << CoreName << "_png[] = {\n";

      char byte;

      int j = 0;

      if (i.get(byte)) {
        o << "\t0x" << std::setw(2) << std::setfill('0') << std::hex
          << (0xFF & byte);
        ++j;
        while (i.get(byte)) {
          o << ", ";
          if (j++ % 16 == 0) {
            o << "\n\t";
          }
          o << "0x" << std::setw(2) << std::setfill('0') << std::hex
            << (0xFF & byte);
        }
      }

      o << std::dec;
      o << "\n};\n\n";
      o << "static const size_t Source_" << CoreName << "_png_len = " << j
        << ";\n";

      Names.emplace(de.path().stem().string());

      /*s << xxdCommand << " -i \"Source/" << de.path().filename() << "\" > \""
      << Destination << "\""; system(s.str().c_str());

      Names.emplace(de.path().stem().string());

      std::ifstream i(Destination);
      s = {};
      s << i.rdbuf();
      i.close();

      std::string str = s.str();

      std::string Search = "unsigned char";
      std::string Replace = "static const unsigned char";
      if (size_t Pos = str.find(Search, 0); Pos != std::string::npos) {
              str.replace(Pos, Search.size(), Replace);
      }
      Search = "unsigned int";
      Replace = "static const size_t";
      if (size_t Pos = str.find(Search, 0); Pos != std::string::npos) {
              str.replace(Pos, Search.size(), Replace);
      }
      std::ofstream of(Destination);
      of << str;*/
    }
  }
  std::cout << "Writing Png_X_List.hpp\n";

  std::stringstream Ss;
  Ss << R"----(#pragma once

#include "pch.hpp"

#define XList_Png_Images \
)----";

  for (const auto &Name : Names) {
    Ss << "X(" << Name << ")\\\n";
  }

  Ss << "\n";

  std::string NewPngXLists = Ss.str();

  Ss = std::stringstream();
  if (std::filesystem::exists("Png_X_List.hpp")) {
    std::ifstream I("Png_X_Lists.hpp");
    Ss << I.rdbuf();
    std::string OldPngXLists = Ss.str();
    if (NewPngXLists != OldPngXLists) {
      std::ofstream O("Png_X_List.hpp");
      O << NewPngXLists;
    }
    Ss = std::stringstream();
  } else {
    std::ofstream O("Png_X_List.hpp");
    O << NewPngXLists;
  }

  std::cout << "Writing Png_Includes.hpp\n";

  Ss << R"---(#pragma once

#include "pch.hpp"

)---";

  for (const auto &Name : Names) {
    Ss << "#include \"PngHeaders/" + Name + ".hpp\"\n";
  }

  Ss << "\n";

  std::string NewPngIncludes = Ss.str();

  Ss = std::stringstream();
  if (std::filesystem::exists("Png_Includes.hpp")) {
    std::ifstream I("Png_Includes.hpp");
    Ss << I.rdbuf();
    std::string OldPngIncludes = Ss.str();
    if (NewPngIncludes != OldPngIncludes) {
      std::ofstream O("Png_Includes.hpp");
      O << NewPngIncludes;
    }
    Ss = std::stringstream();
  } else {
    std::ofstream O("Png_Includes.hpp");
    O << NewPngIncludes;
  }

  std::ofstream HashFile(HashPath);
  HashFile << Hash;

  return 0;
}
