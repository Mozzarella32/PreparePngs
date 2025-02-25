#include <fstream>
#include <filesystem>
#include <string>
#include <algorithm>
#include <unordered_set>

#include <iostream>

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

	std::cout << "Last Hash:    " << OldHash << "\n";

	std::cout << "Current Hash: ";

	std::stringstream HashString;
	HashString << __TIME__;
	for (auto de : std::filesystem::directory_iterator(std::filesystem::path("Source"))) {
		if (de.is_regular_file()) {
			HashString << de.last_write_time().time_since_epoch().count();
		}
	}

	Hash = std::hash<std::string>{}(HashString.str());

	std::cout << Hash << "\n";

	return OldHash != Hash;
}

int main(int argc, char* argv[]) {
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());

	std::cout << "PreprocessPngs:\n";

	if (!HasChanged()) {
		return 0;
		std::cout << "All Up To Date\n";

	}

	std::unordered_set<std::string> Names;

	for (auto de : std::filesystem::directory_iterator(std::filesystem::path("Source"))) {
		if (de.is_regular_file()) {

			std::filesystem::path Destination("PngHeaders/" + de.path().stem().string() + ".h");

			if (!std::filesystem::exists(Destination.parent_path())) {
				std::filesystem::create_directories(Destination.parent_path());
			}

			std::stringstream s;

#ifdef _WIN32
			std::string xxdCommand = "xxd.exe";
#else 
			std::string xxdCommand = "xxd";
#endif 


			s << xxdCommand << " -i \"Source/" << de.path().filename() << "\" > \"" << Destination << "\"";
			system(s.str().c_str());

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
			of << str;
		}
	}
	std::cout << "Writing Png_X_List.h\n";

	std::stringstream Ss;
	Ss << R"----(#pragma once

#include "pch.h"

#define XList_Png_Images \
)----";

	for (const auto& Name : Names) {
		Ss << "X(" << Name << ")\\\n";
	}

	std::string NewPngXLists = Ss.str();

	Ss = std::stringstream();
	if (std::filesystem::exists("Png_X_List.h")) {
		std::ifstream I("Png_X_Lists.h");
		Ss << I.rdbuf();
		std::string OldPngXLists = Ss.str();
		if (NewPngXLists != OldPngXLists) {
			std::ofstream O("Png_X_List.h");
			O << NewPngXLists;
		}
		Ss = std::stringstream();
	}
	else {
		std::ofstream O("Png_X_List.h");
		O << NewPngXLists;
	}

	std::cout << "Writing Png_Includes.h\n";

	Ss << R"---(#pragma once

#include "pch.h"

)---";

	for (const auto& Name : Names) {
		Ss << "#include \"PngHeaders/" + Name + ".h\"\n";
	}

	std::string NewPngIncludes = Ss.str();

	Ss = std::stringstream();
	if (std::filesystem::exists("Png_Includes.h")) {
		std::ifstream I("Png_Includes.h");
		Ss << I.rdbuf();
		std::string OldPngIncludes = Ss.str();
		if (NewPngIncludes != OldPngIncludes) {
			std::ofstream O("Png_Includes.h");
			O << NewPngIncludes;
		}
		Ss = std::stringstream();
	}
	else {
		std::ofstream O("Png_Includes.h");
		O << NewPngIncludes;
	}

	std::ofstream HashFile(HashPath);
	HashFile << Hash;

	return 0;
}