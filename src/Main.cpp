#include <iostream>

#include "argparse.h"
#include "Extractor.h"

/* Main function */
int main(int argc, const char* argv[])
{
    ArgumentParser parser;
    parser.addArgument("-i", "--input", 1, true);
    parser.parse(argc, argv);
    std::string videoPath = parser.retrieve<std::string>("input");

    // draw all objects in vector
    Extractor *extractor = new Extractor();
    extractor->setVideoName(videoPath);
    extractor->loadVideo();
    extractor->extractVideo();

    return EXIT_SUCCESS;
}
