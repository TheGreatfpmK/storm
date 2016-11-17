#ifndef STORM_ADAPTERS_DDEXPRESSIONADAPTER_H_
#define STORM_ADAPTERS_DDEXPRESSIONADAPTER_H_

#include <memory>

#include "src/storm/storage/expressions/Variable.h"
#include "src/storm/storage/expressions/Expressions.h"
#include "src/storm/storage/expressions/ExpressionVisitor.h"

#include "src/storm/storage/dd/Add.h"
#include "src/storm/storage/dd/Bdd.h"
#include "src/storm/storage/dd/DdManager.h"

namespace storm {
    namespace adapters {
        
        template<storm::dd::DdType Type, typename ValueType = double>
        class AddExpressionAdapter : public storm::expressions::ExpressionVisitor {
        public:
            AddExpressionAdapter(std::shared_ptr<storm::dd::DdManager<Type>> ddManager, std::shared_ptr<std::map<storm::expressions::Variable, storm::expressions::Variable>> const& variableMapping);
            
            storm::dd::Add<Type, ValueType> translateExpression(storm::expressions::Expression const& expression);
            storm::dd::Bdd<Type> translateBooleanExpression(storm::expressions::Expression const& expression);
            
            virtual boost::any visit(storm::expressions::IfThenElseExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::BinaryBooleanFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::BinaryNumericalFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::BinaryRelationExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::VariableExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::UnaryBooleanFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::UnaryNumericalFunctionExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::BooleanLiteralExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::IntegerLiteralExpression const& expression, boost::any const& data) override;
            virtual boost::any visit(storm::expressions::RationalLiteralExpression const& expression, boost::any const& data) override;

        private:
            // The manager responsible for the DDs built by this adapter.
            std::shared_ptr<storm::dd::DdManager<Type>> ddManager;
            
            // This member maps the variables used in the expressions to the variables used by the DD manager.
            std::shared_ptr<std::map<storm::expressions::Variable, storm::expressions::Variable>> variableMapping;
        };
        
    } // namespace adapters
} // namespace storm

#endif /* STORM_ADAPTERS_DDEXPRESSIONADAPTER_H_ */
