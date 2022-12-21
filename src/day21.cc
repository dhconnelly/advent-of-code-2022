#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

#include "util.h"

enum class expr_type { val, binary, var };
struct expr {
    expr_type typ;
    int64_t val;
    char op;
    std::pair<std::unique_ptr<expr>, std::unique_ptr<expr>> args;
    std::string var;

    expr& operator=(const expr& e) {
        typ = e.typ;
        switch (e.typ) {
            case expr_type::val: val = e.val; return *this;
            case expr_type::var: var = e.var; return *this;
            case expr_type::binary:
                op = e.op;
                args.first = std::make_unique<expr>(*e.args.first);
                args.second = std::make_unique<expr>(*e.args.second);
                return *this;
        }
    }
    ~expr() = default;
    expr() {}
    expr(const std::string& name) : typ(expr_type::var), var(name) {}
    expr(const expr& e) { *this = e; }
};

std::pair<std::string, expr> parse(const std::string& s) {
    std::smatch m;
    static const std::regex lit(R"(^(\w+): (\d+)$)");
    if (std::regex_match(s, m, lit)) {
        expr e;
        e.typ = expr_type::val;
        e.val = int_match(m, 2);
        return {m[1].str(), std::move(e)};
    }
    static const std::regex binary(R"(^(\w+): (\w+) ([\+\-\*\/]) (\w+)$)");
    if (std::regex_match(s, m, binary)) {
        auto left = std::make_unique<expr>();
        left->typ = expr_type::var;
        left->var = m[2].str();
        auto right = std::make_unique<expr>();
        right->typ = expr_type::var;
        right->var = m[4].str();
        expr e;
        e.typ = expr_type::binary;
        e.op = *m[3].first;
        e.args = {std::move(left), std::move(right)};
        return {m[1].str(), std::move(e)};
    }
    die("bad rule: " + s);
}

std::ostream& operator<<(std::ostream& os, const expr& e) {
    if (e.typ == expr_type::val) return os << e.val;
    if (e.typ == expr_type::var) return os << e.var;
    else {
        return os << '(' << *e.args.first << ' ' << e.op << ' '
                  << *e.args.second << ')';
    }
}

std::map<std::string, expr> parse(std::istream&& is) {
    std::map<std::string, expr> rules;
    for (std::string s; std::getline(is, s);) rules.insert(parse(s));
    return rules;
}

class evaluator {
public:
    evaluator(const std::map<std::string, expr>& rules) : rules_(rules) {}

    expr evaluate(const expr& e) {
        if (e.typ == expr_type::val) return e;
        if (e.typ == expr_type::var) return evaluate(e.var);

        expr result;
        const expr& left = evaluate(*e.args.first);
        const expr& right = evaluate(*e.args.second);

        if (left.typ != expr_type::val || right.typ != expr_type::val) {
            result.typ = expr_type::binary;
            result.op = e.op;
            result.args.first = std::make_unique<expr>(left);
            result.args.second = std::make_unique<expr>(right);
            return result;
        }

        switch (e.op) {
            case '+': result.val = left.val + right.val; break;
            case '-': result.val = left.val - right.val; break;
            case '*': result.val = left.val * right.val; break;
            case '/': result.val = left.val / right.val; break;
            case '=': result.val = left.val == right.val; break;
            default: die("bad expr");
        }

        result.typ = expr_type::val;
        return result;
    }

    expr evaluate(const std::string& name) {
        if (auto it = cache_.find(name); it != cache_.end()) return it->second;
        if (auto it = rules_.find(name);
            it != rules_.end() && it->second.typ != expr_type::var) {
            return cache_[name] = evaluate(it->second);
        }
        return expr(name);
    }

private:
    std::map<std::string, expr> rules_;
    std::map<std::string, expr> cache_;
};

int main(int argc, char* argv[]) {
    if (argc != 2) die("usage: day21 <file>");
    std::map rules = parse(std::ifstream(argv[1]));

    evaluator e1(rules);
    std::cout << e1.evaluate("root") << std::endl;

    rules["root"].op = '=';
    rules["humn"].typ = expr_type::var;
    rules["humn"].var = "humn";
    evaluator e2(rules);
    expr eqn = e2.evaluate("root");
    if (eqn.typ != expr_type::binary || eqn.op != '=' ||
        eqn.args.first->typ != expr_type::binary ||
        eqn.args.second->typ != expr_type::val) {
        die("bad equation");
    }
    std::stringstream s;
    s << std::endl;
    s << "python -c \"import sympy" << std::endl;
    s << "humn = sympy.Symbol('humn')" << std::endl;
    s << "print(sympy.solve(sympy.Eq(" << *eqn.args.first << ", "
      << *eqn.args.second << "))[0])\"" << std::endl;
    system(s.str().c_str());
}
