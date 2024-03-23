#include <iostream>
#include <string>
#include <algorithm>

#include "clparser/Parser.hpp"

#include "tensorflow/Tfml.hpp"
#include "video/Vstream.hpp"
#include "serial/Aserial.hpp"

using namespace std;
using namespace tfml;

int main(int argc, char* argv[]) {

    ClPosArg videoSource("video_source_pos_arg", true);
    ClOption videoSourceOption("video_source_option",
                               { "v", "videoSource" },
                               "Specify a video source.",
                               { videoSource });

    ClPosArg checkpointPath("checkpoint_path_pos_arg", true);
    ClOption checkpointPathOption("checkpoint_path_option",
                                  {"c", "checkpointPath"},
                                  "Specify the folder where your checkpoint is located in.",
                                  { checkpointPath });

    ClPosArg labelmapPath("labelmap_path_pos_arg", true);
    ClOption labelmapPathOption("labelmap_path_option",
                               { "l", "labelmapPath" },
                               "Specify the folder where your labelmap is located in.",
                               { labelmapPath });

    ClPosArg serialPort("serial_port_pos_arg", true);
    ClOption serialPortOption("serial_port_option",
                               { "s", "serialPort" },
                               "Specify a port for serial communication.",
                               { serialPort });

    ClPosArg baudRate("baud_rate_pos_arg", true);
    ClOption baudRateOption("baud_rate_option",
                               { "b", "baudRate" },
                               "Specify a baud rate for serial communication.",
                               { serialPort });

    ClParser parser({ videoSourceOption, checkpointPathOption, labelmapPathOption, serialPortOption, baudRateOption});
    parser.addAppName("aijnano");
    parser.addAppVersion("0.0.1");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.parse(argc, argv);

    cout << "parsing finished..." << endl;

    Vstream videoStream { };
    Detector det { checkpointPath.value(), labelmapPath.value() };
    
    cout << "Detector initialized." << endl;
    
    Aserial serial(serialPort.cvalue(), stoi(baudRate.value()));
    
    cout << "initialized Vstream, Detector and Aserial instances." << endl;

    det.detect(&videoStream);

    cout << "now detecting..." << endl;

    while (videoStream.running())
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
        
         
        cout << string("R"
                        + to_string(obj.points.x)
                        + ";"
                        + to_string(obj.points.y)
                        + ";"
                        + to_string(obj.points.height)
                        + ";"
                        + to_string(obj.points.width)
                        + "\n").c_str();

        serial.out(
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
    cout << "finished.";
}
