#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iomanip>
#include <cctype>
#include <cstdio>

using namespace std;

struct Token {
    string type, text;
    int line;
};

class CompileError : public runtime_error {
public:
    string stage;
    int line;
    CompileError(const string& s, int l, const string& msg)
        : runtime_error(msg), stage(s), line(l) {}
};

static const map<string, string> keywords = {
    {"inicio", "INICIO"}, {"fim", "FIM"}, {"numero", "NUMERO"},
    {"mostrar", "MOSTRAR"}, {"se", "SE"}, {"entao", "ENTAO"},
    {"fimse", "FIMSE"}
};

class Lexer {
    string source;
    size_t pos = 0;
    int line = 1;

    char peek(size_t offset = 0) const {
        return pos + offset < source.size() ? source[pos + offset] : '\0';
    }

    char advance() {
        char c = peek();
        if (c != '\0') {
            ++pos;
            if (c == '\n') ++line;
        }
        return c;
    }

public:
    explicit Lexer(string src) : source(std::move(src)) {}

    vector<Token> tokenize() {
        vector<Token> tokens;
        while (peek() != '\0') {
            char c = peek();

            if (isspace(static_cast<unsigned char>(c))) {
                advance();
                continue;
            }

            if (c == '/' && peek(1) == '/') {
                while (peek() != '\0' && peek() != '\n') advance();
                continue;
            }

            int tokenLine = line;

            if (isalpha(static_cast<unsigned char>(c)) || c == '_') {
                string word;
                while (isalnum(static_cast<unsigned char>(peek())) || peek() == '_')
                    word += advance();

                auto it = keywords.find(word);
                tokens.push_back({it == keywords.end() ? "IDENTIFICADOR" : it->second, word, tokenLine});
                continue;
            }

            if (isdigit(static_cast<unsigned char>(c))) {
                string number;
                while (isdigit(static_cast<unsigned char>(peek()))) number += advance();

                if (peek() == '.' && isdigit(static_cast<unsigned char>(peek(1)))) {
                    number += advance();
                    while (isdigit(static_cast<unsigned char>(peek()))) number += advance();
                }
                tokens.push_back({"NUMERO_LITERAL", number, tokenLine});
                continue;
            }

            string two;
            two += c;
            two += peek(1);
            if (two == ">=" || two == "<=" || two == "==" || two == "!=") {
                advance(); advance();
                tokens.push_back({"COMPARADOR", two, tokenLine});
                continue;
            }

            string one(1, c);
            if (c == '+' || c == '-' || c == '*' || c == '=') {
                advance();
                tokens.push_back({c == '=' ? "ATRIBUICAO" : "OPERADOR_ARITMETICO", one, tokenLine});
            } else if (c == '>' || c == '<') {
                advance();
                tokens.push_back({"COMPARADOR", one, tokenLine});
            } else if (c == '(' || c == ')' || c == ';') {
                advance();
                string type = c == '(' ? "ABRE_PARENTESE" :
                              c == ')' ? "FECHA_PARENTESE" : "PONTO_VIRGULA";
                tokens.push_back({type, one, tokenLine});
            } else {
                throw CompileError("LEXICO", line, string("simbolo invalido: '") + c + "'");
            }
        }

        tokens.push_back({"EOF", "EOF", line});
        return tokens;
    }
};

struct Expr {
    enum Kind { NUMBER, VARIABLE, BINARY } kind;
    string value;
    unique_ptr<Expr> left, right;
    int line;

    explicit Expr(string v, int ln = 1) : kind(NUMBER), value(std::move(v)), line(ln) {}
    Expr(Kind k, string v, int ln = 1) : kind(k), value(std::move(v)), line(ln) {}
};

struct Condition {
    unique_ptr<Expr> left, right;
    string op;
    int line;
};

struct Stmt {
    enum Kind { ASSIGN, PRINT, IF } kind;
    string name;
    unique_ptr<Expr> expr;
    unique_ptr<Condition> condition;
    vector<unique_ptr<Stmt>> body;
    int line = 1;
};

struct Program {
    vector<string> declarations;
    vector<int> declarationLines;
    vector<unique_ptr<Stmt>> statements;
};

class Parser {
    const vector<Token>& tokens;
    size_t pos = 0;

    const Token& current() const { return tokens[pos]; }
    bool check(const string& type) const { return current().type == type; }

    Token consume(const string& type, const string& expected) {
        if (!check(type))
            throw CompileError("SINTATICO", current().line,
                "esperado " + expected + ", encontrado '" + current().text + "'");
        return tokens[pos++];
    }

    unique_ptr<Expr> parsePrimary() {
        if (check("NUMERO_LITERAL")) {
            Token t = consume("NUMERO_LITERAL", "numero");
            return make_unique<Expr>(t.text, t.line);
        }
        if (check("IDENTIFICADOR")) {
            Token t = consume("IDENTIFICADOR", "identificador");
            auto e = make_unique<Expr>(Expr::VARIABLE, t.text, t.line);
            return e;
        }
        if (check("ABRE_PARENTESE")) {
            consume("ABRE_PARENTESE", "'('");
            auto e = parseExpression();
            consume("FECHA_PARENTESE", "')'");
            return e;
        }
        throw CompileError("SINTATICO", current().line,
            "esperado numero, variavel ou '(' na expressao");
    }

    unique_ptr<Expr> parseUnary() {
        if (check("OPERADOR_ARITMETICO") && (current().text == "+" || current().text == "-")) {
            string op = current().text;
            int ln = current().line;
            ++pos;
            auto right = parseUnary();
            auto zero = make_unique<Expr>("0", ln);
            auto result = make_unique<Expr>(Expr::BINARY, op, ln);
            result->left = std::move(zero);
            result->right = std::move(right);
            return result;
        }
        return parsePrimary();
    }

    unique_ptr<Expr> parseTerm() {
        auto left = parseUnary();
        while (check("OPERADOR_ARITMETICO") && current().text == "*") {
            string op = current().text; ++pos;
            auto node = make_unique<Expr>(Expr::BINARY, op, current().line);
            node->left = std::move(left);
            node->right = parseUnary();
            left = std::move(node);
        }
        return left;
    }

    unique_ptr<Expr> parseExpression() {
        auto left = parseTerm();
        while (check("OPERADOR_ARITMETICO") &&
              (current().text == "+" || current().text == "-")) {
            string op = current().text; ++pos;
            auto node = make_unique<Expr>(Expr::BINARY, op, current().line);
            node->left = std::move(left);
            node->right = parseTerm();
            left = std::move(node);
        }
        return left;
    }

    unique_ptr<Condition> parseCondition() {
        auto c = make_unique<Condition>();
        c->line = current().line;
        c->left = parseExpression();
        if (!check("COMPARADOR"))
            throw CompileError("SINTATICO", current().line, "esperado operador de comparacao");
        c->op = current().text; ++pos;
        c->right = parseExpression();
        return c;
    }

    unique_ptr<Stmt> parseStatement() {
        auto s = make_unique<Stmt>();
        s->line = current().line;

        if (check("IDENTIFICADOR")) {
            s->kind = Stmt::ASSIGN;
            s->name = consume("IDENTIFICADOR", "nome da variavel").text;
            consume("ATRIBUICAO", "'='");
            s->expr = parseExpression();
            consume("PONTO_VIRGULA", "';'");
            return s;
        }

        if (check("MOSTRAR")) {
            consume("MOSTRAR", "'mostrar'");
            s->kind = Stmt::PRINT;
            s->expr = parseExpression();
            consume("PONTO_VIRGULA", "';'");
            return s;
        }

        if (check("SE")) {
            consume("SE", "'se'");
            s->kind = Stmt::IF;
            consume("ABRE_PARENTESE", "'('");
            s->condition = parseCondition();
            consume("FECHA_PARENTESE", "')'");
            consume("ENTAO", "'entao'");
            while (!check("FIMSE") && !check("EOF") && !check("FIM"))
                s->body.push_back(parseStatement());
            consume("FIMSE", "'fimse'");
            return s;
        }

        throw CompileError("SINTATICO", current().line, "comando inesperado: '" + current().text + "'");
    }

public:
    explicit Parser(const vector<Token>& t) : tokens(t) {}

    Program parse() {
        Program p;
        consume("INICIO", "'inicio'");

        while (check("NUMERO")) {
            consume("NUMERO", "'numero'");
            Token id = consume("IDENTIFICADOR", "identificador");
            consume("PONTO_VIRGULA", "';'");
            p.declarations.push_back(id.text);
            p.declarationLines.push_back(id.line);
        }

        while (!check("FIM") && !check("EOF"))
            p.statements.push_back(parseStatement());

        consume("FIM", "'fim'");
        consume("EOF", "fim do arquivo");
        return p;
    }
};

struct Symbol {
    string type = "numero";
    bool initialized = false;
    int line = 0;
};

class SemanticAnalyzer {
    map<string, Symbol> symbols;

    void checkExpr(const Expr* e, const map<string, bool>& state) {
        if (e->kind == Expr::VARIABLE) {
            auto it = symbols.find(e->value);
            if (it == symbols.end())
                throw CompileError("SEMANTICO", e->line, "variavel '" + e->value + "' nao declarada");
            auto st = state.find(e->value);
            if (st == state.end() || !st->second)
                throw CompileError("SEMANTICO", e->line,
                    "variavel '" + e->value + "' usada antes de receber valor");
        } else if (e->kind == Expr::BINARY) {
            checkExpr(e->left.get(), state);
            checkExpr(e->right.get(), state);
        }
    }

    void checkStatements(const vector<unique_ptr<Stmt>>& statements, map<string, bool>& state) {
        for (const auto& s : statements) {
            if (s->kind == Stmt::ASSIGN) {
                if (!symbols.count(s->name))
                    throw CompileError("SEMANTICO", s->line,
                        "variavel '" + s->name + "' nao declarada");
                checkExpr(s->expr.get(), state);
                state[s->name] = true;
            } else if (s->kind == Stmt::PRINT) {
                checkExpr(s->expr.get(), state);
            } else {
                checkExpr(s->condition->left.get(), state);
                checkExpr(s->condition->right.get(), state);
                map<string, bool> branchState = state;
                checkStatements(s->body, branchState);
            }
        }
    }

public:
    map<string, Symbol> analyze(const Program& p) {
        for (size_t i = 0; i < p.declarations.size(); ++i) {
            const string& name = p.declarations[i];
            if (symbols.count(name))
                throw CompileError("SEMANTICO", p.declarationLines[i],
                    "variavel '" + name + "' declarada mais de uma vez");
            symbols[name] = {"numero", false, p.declarationLines[i]};
        }

        map<string, bool> state;
        for (const auto& entry : symbols) state[entry.first] = false;
        checkStatements(p.statements, state);
        for (auto& entry : symbols) entry.second.initialized = state[entry.first];
        return symbols;
    }
};

class CodeGenerator {
    string expression(const Expr* e) {
        if (e->kind == Expr::NUMBER || e->kind == Expr::VARIABLE) return e->value;
        return "(" + expression(e->left.get()) + " " + e->value + " " +
               expression(e->right.get()) + ")";
    }

    void statements(const vector<unique_ptr<Stmt>>& list, ostringstream& out, int indent) {
        string pad(indent * 4, ' ');
        for (const auto& s : list) {
            if (s->kind == Stmt::ASSIGN) {
                out << pad << s->name << " = " << expression(s->expr.get()) << "\n";
            } else if (s->kind == Stmt::PRINT) {
                out << pad << "print(" << expression(s->expr.get()) << ")\n";
            } else {
                out << pad << "if " << expression(s->condition->left.get()) << " "
                    << s->condition->op << " " << expression(s->condition->right.get()) << ":\n";
                if (s->body.empty()) out << pad << "    pass\n";
                else statements(s->body, out, indent + 1);
            }
        }
    }

public:
    string generate(const Program& p) {
        ostringstream out;
        out << "# Codigo gerado pelo compilador Lume\n";
        statements(p.statements, out, 0);
        return out.str();
    }
};

static string readFile(const string& path) {
    ifstream in(path);
    if (!in) throw runtime_error("nao foi possivel abrir o arquivo: " + path);
    stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

static void printTokens(const vector<Token>& tokens) {
    cout << "\n=== ANALISE LEXICA: TOKENS ===\n";
    cout << left << setw(22) << "CATEGORIA" << setw(22) << "TRECHO" << "LINHA\n";
    for (const auto& t : tokens) {
        if (t.type == "EOF") continue;
        cout << left << setw(22) << t.type << setw(22) << t.text << t.line << "\n";
    }
}

static void printSymbols(const map<string, Symbol>& symbols) {
    cout << "\n=== TABELA DE SIMBOLOS ===\n";
    cout << left << setw(20) << "NOME" << setw(15) << "TIPO" << "INICIALIZADA\n";
    for (const auto& entry : symbols) {
        cout << left << setw(20) << entry.first << setw(15) << entry.second.type
             << (entry.second.initialized ? "sim" : "nao") << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        cerr << "Uso: compilador <arquivo.lume> [arquivo_saida.py]\n";
        return 1;
    }

    string inputPath = argv[1];
    string outputPath;
    if (argc == 3) outputPath = argv[2];
    else {
        size_t dot = inputPath.find_last_of('.');
        outputPath = (dot == string::npos ? inputPath : inputPath.substr(0, dot)) + ".py";
    }

    try {
        string source = readFile(inputPath);

        Lexer lexer(source);
        vector<Token> tokens = lexer.tokenize();
        printTokens(tokens);

        Parser parser(tokens);
        Program program = parser.parse();
        cout << "\n=== ANALISE SINTATICA ===\nPrograma sintaticamente correto.\n";

        SemanticAnalyzer semantic;
        map<string, Symbol> symbols = semantic.analyze(program);
        printSymbols(symbols);
        cout << "\n=== ANALISE SEMANTICA ===\nPrograma semanticamente correto.\n";

        CodeGenerator generator;
        string generated = generator.generate(program);

        ofstream out(outputPath);
        if (!out) throw runtime_error("nao foi possivel criar o arquivo de saida");
        out << generated;
        out.close();

        cout << "\n=== GERACAO DE CODIGO ===\n";
        cout << "Compilacao concluida! Arquivo gerado: " << outputPath << "\n";
        return 0;
    } catch (const CompileError& e) {
        cerr << "\nERRO " << e.stage << " na linha " << e.line << ": " << e.what() << "\n";
        cerr << "Compilacao interrompida. Nenhum novo codigo foi gerado.\n";
        return 2;
    } catch (const exception& e) {
        cerr << "\nERRO: " << e.what() << "\n";
        return 1;
    }
}
