#include "Engine.hpp"

#include <sstream>

Engine::Engine(const std::filesystem::path &path, const std::string &outputDir) 
    : inputPath(path), outputDir(outputDir), cstRoot_(nullptr), astRoot_(nullptr) {}
Engine::~Engine() {
    if (cstRoot_ != nullptr) {
        delete cstRoot_;
        cstRoot_ = nullptr;
    }
    if (astRoot_ != nullptr) {
        delete astRoot_;
    }
}

// sekaligus ngelakuin write kali yak :/
void Engine::lexer() {
    std::cout << "Processing lexer...\n";
    Lexer lexing(inputPath);
    lexing.tokenize();

    tokens_ = lexing.getTokens();
    const auto &errors = lexing.getErrors();

    if (!errors.empty()) {
        std::cout << "\n=== Lexical Errors ===\n";
        for (const auto &error : errors) {
            std::cout << error;
        }
        std::cout << "======================\n\n";
    }

    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-result.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    Writer writer(fullPath.string(), tokens_);
    writer.writeTokenToFile();
}

void Engine::parser() {
    if (tokens_.empty()) {
        throw std::runtime_error("Syntax error: no tokens");
    }

    std::cout << "Processing parser...\n";
    Parser parser(tokens_);
    CSTNodes* root = parser.parse();
    // keep ownership of the CST root for later semantic analysis
    cstRoot_ = root;

    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-parse-tree.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    cstErrors_ = parser.getErrors();

    Writer writer(fullPath.string(), root, cstErrors_);
    writer.printCST();
    writer.writeCSTToFile();
    writer.printParserError();

}

void Engine::semantic() {
    if (!cstErrors_.empty()) {
        std::cout << "Terdapat Error pada Proses-Proses Sebelumnya, Semantic tidak dapat dilanjutkan\n";
        return;
    }

    std::cout << "Processing Semantic Analysis...\n";
    ASTBuilder builder;

    if (astRoot_ != nullptr) {
        delete astRoot_;
        astRoot_ = nullptr;
    }

    astRoot_ = builder.build(cstRoot_);

    if (astRoot_ == nullptr) {
        semanticErrors_.push_back("Semantic error: AST root tidak dapat dibangun.");
        throw std::runtime_error("Semantic error: AST root tidak dapat dibangun.");
    }

    semanticAnalyzer_.analyze(astRoot_);
    std::string tablesText = semanticAnalyzer_.dumpTables();

    semanticErrors_ = semanticAnalyzer_.getErrors();


    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-ast.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    Writer writer(fullPath.string(), cstRoot_, cstErrors_);

    writer.printDecoratedASTWithTables(astRoot_, tablesText);
    writer.writeDecoratedASTWithTablesToFile(astRoot_, tablesText);

    if (!semanticErrors_.empty()) {
        std::cout << "\n=== Semantic Errors ===\n";
        for (const auto& err : semanticErrors_) {
            std::cout << err << "\n";
        }
        std::cout << "=======================\n\n";
    }

    const auto& warnings = semanticAnalyzer_.getWarnings();

    if (!warnings.empty()) {
        std::cout << "\n=== Semantic Warnings ===\n";
        for (const std::string& warning : warnings) {
            std::cout << warning << "\n";
        }
        std::cout << "==========================\n";
    }
}

void Engine::intermediateCodeGenerator() {
    if (!semanticErrors_.empty()) {
        std::cout << "Semantic gagal, Intermediate Code tidak dapat dilanjutkan\n";
        return;
    }

    if (astRoot_ == nullptr) {
        std::cout << "Decorated AST belum tersedia, Intermediate Code tidak dapat dilanjutkan\n";
        return;
    }

    std::cout << "Processing Intermediate Code Generation...\n";

    IntermediateCodeGenerator generator(semanticAnalyzer_.getSymbolTable(), semanticAnalyzer_);
    instructions_ = generator.generate(astRoot_);

    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-intermediate-code.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    Writer writer(fullPath.string(), instructions_);
    writer.printIntermediateCode();
    writer.writeIntermediateCodeToFile();
}

void Engine::execute() {
    if (!cstErrors_.empty() || !semanticErrors_.empty()) {
        std::cout << "Terdapat error pada tahapan kompilasi. Interpreter tidak dapat dijalankan.\n";
        return;
    }

    if (instructions_.empty()) {
        std::cout << "Instruksi TAC kosong. Silakan jalankan intermediateCodeGenerator() terlebih dahulu.\n";
        return;
    }

    std::stringstream debugBuffer;  // Untuk log Stack & IP
    std::stringstream actualOutput; // Untuk output WRT & WRTLN

    std::streambuf* oldCoutBuffer = std::cout.rdbuf(); 
    std::cout.rdbuf(debugBuffer.rdbuf());             
    std::string runtimeErrorMsg = "";

    try {
        Interpreter interpreter;
        interpreter.execute(instructions_, actualOutput); 
    } catch (const std::exception& e) {
        runtimeErrorMsg = std::string("\n[Runtime Error] Eksekusi terhenti: ") + e.what() + "\n";
    }

    std::cout.rdbuf(oldCoutBuffer);
    
    if (!runtimeErrorMsg.empty()) {
        std::cerr << runtimeErrorMsg;
    }

    std::string executionResult = debugBuffer.str();
    
    if (actualOutput.str().empty()) {
        executionResult += "(Tidak ada output)\n";
    } else {
        executionResult += actualOutput.str();
    }
    
    if (!runtimeErrorMsg.empty()) {
        executionResult += runtimeErrorMsg;
    }

    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-output.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    Writer writer(fullPath.string(), executionResult);
    writer.printExecutionOutput();
    writer.writeExecutionOutputToFile();
}