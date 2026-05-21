#include "Compiler.hpp"
#include "semanticanalyzer/SemanticAnalyzer.hpp"


Compiler::Compiler(const std::filesystem::path &path, const std::string &outputDir) 
    : inputPath(path), outputDir(outputDir), cstRoot_(nullptr) {}
Compiler::~Compiler() {
    if (cstRoot_ != nullptr) {
        delete cstRoot_;
        cstRoot_ = nullptr;
    }
}

// sekaligus ngelakuin write kali yak :/
void Compiler::lexer() {
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

void Compiler::parser() {
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

void Compiler::semantic() {
    if (!cstErrors_.empty()) {
        std::cout << "Terdapat Error pada Proses-Proses Sebelumnya, Semantic tidak dapat dilanjutkan\n";
        return;
    }

    std::cout << "Processing Semantic Analysis...\n";
    ASTBuilder builder;

    ASTProgramNode* astRoot = builder.build(cstRoot_);
    if (astRoot == nullptr) {
        throw std::runtime_error("Semantic error: AST root tidak dapat dibangun.");
    }

    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.visitProgramNode(astRoot);
    std::string tablesText = semanticAnalyzer.dumpTables();

    const std::string baseName = inputPath.stem().string();
    const std::filesystem::path fullPath = std::filesystem::path(outputDir) / (baseName + "-ast.txt");

    if (!std::filesystem::exists(outputDir)) {
        std::filesystem::create_directories(outputDir);
    }

    Writer writer(fullPath.string(), cstRoot_, cstErrors_);

    writer.printDecoratedASTWithTables(astRoot, tablesText);
    writer.writeDecoratedASTWithTablesToFile(astRoot, tablesText);

    delete astRoot;
}