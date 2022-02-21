#include "dtl-translate.hpp"


#include "dtl-ast.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace translate {

static class Builder {


}

class TableExpressionCompiler : public dtl::ast::TableExpressionVisitor {


}

static void compile_table_expression(dtl::ast::TableExpression& expression) {

}



static class StatementCompiler : public dtl::ast::StatementVisitor {
  public:
    void visit_assignment_statement(AssignmentStatement& statement) override final {
        dtl::ir::Table& expr_table = compile_table_expression(
            *statement.expression, builder, scope
        );

        dtl::ir::Table& stmt_table(strip_namespaces(expr_table.columns));
        export(stmt_table);

    }

    void visit_update_statement(UpdateStatement& statement) override final {

    }

    void visit_delete_statement(DeleteStatement& statement) override final {

    }

    void visit_insert_statement(InsertStatement& statement) override final {

    }

    void visit_export_statement(ExportStatement& statement) override final {

    }
}

static void compile_statement(dtl::ast::Statement& statement) {
    StatementCompiler compiler(builder);
    compiler.visit(statement);
}




dtl::ir::Program ast_to_ir(dtl::ast::Script script) {

}

}  /* namespace translate */
}  /* namespace dtl */
