#include <iostream>
#include <string>

#include "tensorflow/Tfml.hpp"
#include "video/Vstream.hpp"


using namespace std;
using namespace tfml;

int main(int argc, char* argv[]) {

    Vstream stream("/dev/video0");
    Detector det(CHECKPOINT_PATH, LABELMAP_PATH);

    det.detect(stream);

    vector<DetObj> detection = det.detection();

    for (const DetObj & obj : detection)
        cout << obj.label << obj.points << endl;
}
