#include <iostream>
#include <fstream>

using namespace std;

int main() {
  int retVal = -1;
  int buildCount = -1;
  { // read the current build count
    ifstream buildHeader("buildcount.h");
    if (buildHeader) {
      char define[1024];
      char cnt[1024];
      buildHeader >> define >> cnt >> buildCount;
      if (string(define) != "#define") {
        retVal = 3;
      } else if (string(cnt) != "BUILDCOUNT") {
        retVal = 4;
      } else {
        retVal = 0;
      } // if
    } else {
      retVal = 1;
    } // if
  } // block
  if (!retVal) {
    ofstream buildHeader("buildcount.h");
    if (buildHeader) {
      buildHeader << "#define BUILDCOUNT " << buildCount+1 << endl;
    } else {
      retVal = 2;
    } // if
  } // if
  if (retVal) {
    cout << "Could not generate build count header file, error code:" << retVal << endl;
  } // if
  return retVal;
}
