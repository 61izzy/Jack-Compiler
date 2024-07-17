#include "utils.hpp"
#include "compilationengine.hpp"

void processFile(const std::string &pathstring, const std::string &filename) {
    // opening input and output files
    std::ifstream input(pathstring + "/" + filename + ".jack");
    std::ofstream output(pathstring + "/" + filename + ".vm");

    // initializing tokenizer, vmwriter, and compilation engine
    Tokenizer tokenizer(input);
    VMWriter writer(output);
    CompilationEngine engine(tokenizer, writer);
    writer.close();
}
int main(int argc, char *argv[]) {
    // using std::filesystem to process input file
    std::filesystem::path path = argv[1];

    // if input is a file, we assume that it is a .vm file and process the individual file
    if (std::filesystem::is_regular_file(path)) {
        // processing file name and folder, removing the .jack extension
        std::string filepath = argv[1];
        int filename_start = filepath.rfind("/") + 1, filename_end = filepath.rfind(".");
        std::string filename = filepath.substr(filename_start, filename_end - filename_start),
               pathstring = filepath.substr(0, filename_start - 1);

        processFile(pathstring, filename);
    }
    else {
        // processing path string
        std::string pathstring = path.string();
        // remove trailing slashes
        if (pathstring.length() > 1 && pathstring[pathstring.length() - 1] == '\\' && pathstring[pathstring.length() - 2] == '\\') 
            pathstring = pathstring.substr(0, pathstring.length() - 2);
        if (pathstring[pathstring.length() - 1] == '/' || pathstring[pathstring.length() - 1] == '\\') 
            pathstring = pathstring.substr(0, pathstring.length() - 1);

        // iterating through all files in the directory, skipping folders and files without the .jack extension
        for (auto& entry : std::filesystem::directory_iterator(path)) {
            std::string filename = entry.path().filename().string();
            if (!entry.is_regular_file() || filename.substr(filename.rfind('.')) != ".jack") continue;
            // removing the .jack extension
            filename = filename.substr(0, filename.length() - 5);
            processFile(pathstring, filename);
        }
    }

    return 0;
}