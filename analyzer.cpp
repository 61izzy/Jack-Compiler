#include "utils.hpp"
#include "tokenizer.hpp"
#include "compilationengine.hpp"

// TODO: add comments later and remove commented out code

void processFile(const std::string &pathstring, const std::string &filename) {
    // opening input and output files
    std::ifstream input(pathstring + "/" + filename + ".jack");
    std::ofstream output(pathstring + "/" + filename + ".xml");

    // initializing tokenizer and compilation engine
    Tokenizer tokenizer(input);
    CompilationEngine engine(tokenizer, output);
}
int main(int argc, char *argv[]) {
    // using std::filesystem to process input file
    std::filesystem::path path = argv[1];

    // if input is a file, we assume that it is a .vm file and process the individual file
    if (std::filesystem::is_regular_file(path)) {
        // processing file name and folder
        std::string filepath = argv[1];
        int filename_start = filepath.rfind("/") + 1, filename_end = filepath.rfind(".");
        std::string filename = filepath.substr(filename_start, filename_end - filename_start),
               pathstring = filepath.substr(0, filename_start - 1);

        processFile(pathstring, filename);
        // // opening input and output files
        // std::ifstream input(argv[1]);
        // std::ofstream output(filefolder + "/" + filename + ".asm");

        // // initializing parser and codewriter
        // Parser parser(input);
        // CodeWriter writer(output, filename);

        // processFile(parser, writer);

        // writer.close();
    }
    else {
        // processing path string
        std::string pathstring = path.string();
        // remove trailing slashes
        if (pathstring.length() > 1 && pathstring[pathstring.length() - 1] == '\\' && pathstring[pathstring.length() - 2] == '\\') 
            pathstring = pathstring.substr(0, pathstring.length() - 2);
        if (pathstring[pathstring.length() - 1] == '/' || pathstring[pathstring.length() - 1] == '\\') 
            pathstring = pathstring.substr(0, pathstring.length() - 1);
        // std::string foldername = pathstring.substr(pathstring.rfind('/') + 1);

        // // opening output file
        // std::ofstream output(pathstring + "/" + foldername + ".asm");

        // // initializing codewriter
        // CodeWriter writer(output);
        // writer.writeInit();

        // iterating through all files in the directory, skipping folders and files without the .jack extension
        for (auto& entry : std::filesystem::directory_iterator(path)) {
            std::string filename = entry.path().filename().string();
            if (!entry.is_regular_file() || filename.substr(filename.rfind('.')) != ".jack") continue;
            filename = filename.substr(0, filename.length() - 5);

            // // opening input file
            // std::ifstream input(pathstring + "/" + filename);

            // // initializing parser for current file
            // Parser parser(input);

            // // setting filename for writer
            // writer.setFileName(filename.substr(0, filename.rfind('.')));

            processFile(pathstring, filename);
        }

        // writer.close();
    }

    return 0;
}