//Author(s): Svarun Soda
//Class: Aerospace & Science Team of College of the Canyons

#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <direct.h>
#include <windows.h>

using namespace std;
using std::filesystem::current_path;
namespace fs = std::filesystem;

//// FUNCTION LIST ////

void ReadParticleFile(ifstream& file, string fileExt);
bool FindNextTargetFile(vector<string> fileList, int currentFileNum, int totalFileNum);
vector<string> FindAllTargetFiles(string workingDirectory, string ext);
void BringCursorBack(int offsetX, int offsetY);

//// GLOBAL VARIABLE INITIALIZATION /////

int FileNum = 0;
//int LowSpikeThreshold;
int HighSpikeThreshold;

std::string CurrentFileName = "";

ofstream ReportFile;

//// MAIN FUNCTION ////

int main() {

	cout << "Particle Spike Counter Program\n";
	cout << "Author(s): Svarun Soda\n\n\n";

	// ACQUIRING THE FILEPATH OF THE WORKING DIRECTORY

	char buffer[256];
	_getcwd(buffer, 256);
	string workingDirectory(buffer);

	cout << "Working directory: \"" << workingDirectory << "\".\n\n";

	// ACQUIRING ALL FILES IN CURRENT DIRECTORY OF THE DESIRED FILE TYPE

	string fileExtension = "";

	cout << "Please enter the file extension (ex. \".TXT\" for a text file, \"NO_EXT\" for no extension. Case sensitive!): ";
	cin >> fileExtension;
	
	vector<string> targetFiles = FindAllTargetFiles(workingDirectory, fileExtension);
	
	int totalFileNum = 0;
	int totalParticleHits = 0;
	int tracker = 0;

	for (auto i = targetFiles.begin(); i < targetFiles.end(); i++) {

		totalFileNum++;

	}

	cout << "Please enter the desired high spike threshold: ";
	cin >> HighSpikeThreshold;
	cout << "\n\n";

	cout << "------------------------- " << totalFileNum << " \"" << fileExtension << "\" files found in the current directory. -------------------------\n";

	// READING THE DESIRED FILES AND SEARCHING FOR PARTICLE HITS

	while (FindNextTargetFile(targetFiles, tracker, totalFileNum) == true) {

		ifstream currentFile(CurrentFileName);

		if (!currentFile.is_open()) {

			cout << "\n-------------------------------\nERROR: Could not open file \"" << CurrentFileName << "\"!\n-------------------------------\n";

		} else {

			cout << "\n-------------------------------\nProcessing file \"" << CurrentFileName << "\"...\n";

			ReadParticleFile(currentFile, fileExtension);

			cout << "-------------------------------\n";

		}

		currentFile.close();

		tracker++;

	}

	cout << "\n------------------------- Total particle hits in " << tracker <<" files: " << totalParticleHits << ". -------------------------\n";

	// PROGRAM END

	cout << "\n\nPROGRAM FINISHED\n\n";

	system("pause");

	return 0;

}

//// FILE-READING FUNCTIONS ////

void ReadParticleFile(ifstream& file, string fileExt) {

	//vector<int> particleList;
	string currentLine;
	int particleCount = 0;
	int linesProcessed = 0;
	bool outputFileInitialized = false;

	cout << "Lines processed: " << linesProcessed << "\nParticle hits: " << particleCount;

	BringCursorBack(0, -1);

	while (getline(file, currentLine)) {

		int currentNum = std::stoi(currentLine);

		if (currentNum >= HighSpikeThreshold) {

			if ((outputFileInitialized == false)) {

				string rawFileName = CurrentFileName;

				if ((fileExt.compare("NO_EXT") != 0) && (fileExt.compare("no_ext") != 0)) {

					rawFileName = CurrentFileName.substr(0, CurrentFileName.find_last_of("."));

				}

				ReportFile.open(rawFileName + "_SpikeCounterParticleReport.txt");

				ReportFile << "SpikeCounter report file for \"" + CurrentFileName + "\":\n\n--------------------- HIGH VALUE STREAM START ---------------------\n\n" << flush;

				outputFileInitialized = true;

			}

			//particleList.insert(particleList.begin() + particleCount, currentNum);

			particleCount++;

			if (outputFileInitialized == true) {

				ReportFile << "- Found a reading of " + std::to_string(currentNum) + " at iteration #" + std::to_string(linesProcessed + 1) + ".\n" << flush;

			}

		}

		linesProcessed++;
		
		cout << "Lines processed: " << linesProcessed << "\nParticle hits: " << particleCount;

		BringCursorBack(0, -1);

	}

	if (outputFileInitialized == true) {

		ReportFile << "\n--------------------- HIGH VALUE STREAM END ---------------------\n\n" << flush;

		ReportFile.close();

	}

	BringCursorBack(0, 2);

}

//// FILE-FINDING FUNCTIONS ////

bool FindNextTargetFile(vector<string> fileList, int currentFileNum, int totalFileNum) {

	if (currentFileNum == totalFileNum) {

		return false;

	}
	
	for (auto i = fileList.begin() + currentFileNum; i < fileList.end(); i++) {

		string currentFilePath = *i;

		CurrentFileName = currentFilePath.substr(currentFilePath.find_last_of("/\\") + 1);

		return true;

	}

}

vector<string> FindAllTargetFiles(string workingDirectory, string ext) {

	// ACQUIRING ALL THE FILES THE CURRENT DIRECTORY

	vector<string> allFiles;
	int tracker = 0;

	for (const auto& directoryEntry : fs::directory_iterator(workingDirectory)) {

		allFiles.insert(allFiles.begin() + tracker, directoryEntry.path().string());

		tracker++;

	}

	// ACQUIRING THE FILES OF INTEREST IN THE CURRENT DIRECTORY

	vector<string> targetFiles;
	int insertIdx = 0;
	tracker = 0;
	
	for (auto i = allFiles.begin(); i < allFiles.end(); i++) {

		string currentFile = *i;
		bool canInsert = false;

		if (currentFile.find("SpikeCounterParticleReport") == string::npos) {

			if (currentFile.find(ext) != string::npos) {

				canInsert = true;

			} else {

				if ((ext.compare("NO_EXT") == 0) || (ext.compare("no_ext") == 0) && (currentFile.find(".") == string::npos)) {

					canInsert = true;

				}

			}

		}

		if (canInsert == true) {

			targetFiles.insert(targetFiles.begin() + insertIdx, *i);

			insertIdx++;

		}

		tracker++;

	}

	return targetFiles;

}

//// SUPPORTING FUNCTIONS ////

void BringCursorBack(int offsetX, int offsetY) {

	COORD newCursorPos;
	CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);
	COORD currentCursorPos = consoleInfo.dwCursorPosition;

	newCursorPos.X = offsetX;
	newCursorPos.Y = currentCursorPos.Y + offsetY;

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), newCursorPos);

}