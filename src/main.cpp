#include <iostream>
#include <string>
#include <algorithm>

#include "tensorflow/Tfml.hpp"
#include "video/Vstream.hpp"
#include "serial/Aserial.hpp"

using namespace std;
using namespace tfml;

int main(int argc, char* argv[]) {

    Vstream stream(VIDEO_SOURCE);
    Detector det(CHECKPOINT_PATH, LABELMAP_PATH);
    Aserial serial(SERIAL_PORT, 100000);

    det.detect(stream);

    while (stream.running())
    {
        vector<DetObj> detection = det.detection();

        sort(
            detection.begin(),
            detection.end(),
            [](const DetObj & fobj, const DetObj & nobj) {
                    return fobj.points.y > nobj.points.y;
                }
                );

        const DetObj & obj = detection.front();

        serial.write(
                string("R"
                        + to_string(obj.points.x)
                        + ";"
                        + to_string(obj.points.y)
                        + ";"
                        + to_string(obj.points.height)
                        + ";"
                        + to_string(obj.points.width)
                        + "\n").c_str()
                );
    }
}
