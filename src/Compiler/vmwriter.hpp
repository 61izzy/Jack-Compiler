#ifndef VMWRITER_HPP
#define VMWRITER_HPP

class VMWriter {
private:
    std::ofstream &output;

public:
    VMWriter(std::ofstream &output) : output{output} {}
    inline void writePush(const std::string &segment, const int &index) {
        output << "\tpush " << segment << " " << index << "\n";
    }
    inline void writePop(const std::string &segment, const int &index) {
        output << "\tpop " << segment << " " << index << "\n";
    }
    inline void writeArithmetic(const std::string &command) {
        output << "\t" << command << "\n";
    }
    inline void writeLabel(const std::string &label) {
        output << "label " << label << "\n";
    }
    inline void writeGoto(const std::string &label) {
        output << "\tgoto " << label << "\n";
    }
    inline void writeIf(const std::string &label) {
        output << "\tif-goto " << label << "\n";
    }
    inline void writeCall(const std::string &name, const int &nArgs) {
        output << "\tcall " << name << " " << nArgs << "\n";
    }
    inline void writeFunction(const std::string &name, const int &nLocals) {
        output << "function " << name << " " << nLocals << "\n";
    }
    inline void writeReturn() {
        output << "\treturn\n";
    }
    inline void close() {
        output.close();
    }
};

#endif