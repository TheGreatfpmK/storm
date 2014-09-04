#ifndef STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_
#define STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_

namespace storm {
    namespace expressions {
        // Forward-declare all expression classes.
        class IfThenElseExpression;
        class BinaryBooleanFunctionExpression;
        class BinaryNumericalFunctionExpression;
        class BinaryRelationExpression;
        class VariableExpression;
        class UnaryBooleanFunctionExpression;
        class UnaryNumericalFunctionExpression;
        class BooleanLiteralExpression;
        class IntegerLiteralExpression;
        class DoubleLiteralExpression;
        
        class ExpressionVisitor {
        public:
            virtual void visit(IfThenElseExpression const* expression) = 0;
            virtual void visit(BinaryBooleanFunctionExpression const* expression) = 0;
            virtual void visit(BinaryNumericalFunctionExpression const* expression) = 0;
            virtual void visit(BinaryRelationExpression const* expression) = 0;
            virtual void visit(VariableExpression const* expression) = 0;
            virtual void visit(UnaryBooleanFunctionExpression const* expression) = 0;
            virtual void visit(UnaryNumericalFunctionExpression const* expression) = 0;
            virtual void visit(BooleanLiteralExpression const* expression) = 0;
            virtual void visit(IntegerLiteralExpression const* expression) = 0;
            virtual void visit(DoubleLiteralExpression const* expression) = 0;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_EXPRESSIONVISITOR_H_ */