#include "storm/abstraction/ExpressionTranslator.h"

#include "storm/abstraction/AbstractionInformation.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/storage/expressions/Expression.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace abstraction {
        
        using namespace storm::expressions;
        
        template <storm::dd::DdType DdType>
        ExpressionTranslator<DdType>::ExpressionTranslator(AbstractionInformation<DdType>& abstractionInformation, std::unique_ptr<storm::solver::SmtSolver>&& smtSolver) : abstractionInformation(abstractionInformation), equivalenceChecker(std::move(smtSolver)), locationVariables(abstractionInformation.getLocationExpressionVariables()), abstractedVariables(abstractionInformation.getAbstractedVariables()) {
            // Intentionally left empty.
        }

        template <storm::dd::DdType DdType>
        storm::dd::Bdd<DdType> ExpressionTranslator<DdType>::translate(storm::expressions::Expression const& expression) {
            return boost::any_cast<storm::dd::Bdd<DdType>>(expression.accept(*this, boost::none));
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(IfThenElseExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expressions of this kind are currently not supported by the abstraction expression translator.");
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
            // Check whether the expression is either fully contained in the location variables fragment or the abstracted
            // variables fragment.
            std::set<storm::expressions::Variable> variablesInExpression;
            expression.gatherVariables(variablesInExpression);
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(variablesInExpression.begin(), variablesInExpression.end(), locationVariables.begin(), locationVariables.end(), std::inserter(tmp, tmp.begin()));
            bool hasLocationVariables = !tmp.empty();
            
            tmp.clear();
            std::set_intersection(variablesInExpression.begin(), variablesInExpression.end(), abstractedVariables.begin(), abstractedVariables.end(), std::inserter(tmp, tmp.begin()));
            bool hasAbstractedVariables = !tmp.empty();
            
            STORM_LOG_THROW(hasLocationVariables || hasAbstractedVariables, storm::exceptions::NotSupportedException, "Expressions without variables are currently not supported by the abstraction expression translator.");
            
            if (hasAbstractedVariables && !hasLocationVariables) {
                for (uint64_t predicateIndex = 0; predicateIndex < abstractionInformation.get().getNumberOfPredicates(); ++predicateIndex) {
                    if (equivalenceChecker.areEquivalent(abstractionInformation.get().getPredicateByIndex(predicateIndex), expression.toExpression())) {
                        return abstractionInformation.get().encodePredicateAsSource(predicateIndex);
                    }
                }

                // At this point, none of the predicates was found to be equivalent, so we split the expression.
            }
            
            storm::dd::Bdd<DdType> left = boost::any_cast<storm::dd::Bdd<DdType>>(expression.getFirstOperand()->accept(*this, boost::none));
            storm::dd::Bdd<DdType> right = boost::any_cast<storm::dd::Bdd<DdType>>(expression.getSecondOperand()->accept(*this, boost::none));
            switch (expression.getOperatorType()) {
                case BinaryBooleanFunctionExpression::OperatorType::And: return left && right;
                case BinaryBooleanFunctionExpression::OperatorType::Or: return left || right;
                case BinaryBooleanFunctionExpression::OperatorType::Xor: return left.exclusiveOr(right);
                case BinaryBooleanFunctionExpression::OperatorType::Implies: return !left || right;
                case BinaryBooleanFunctionExpression::OperatorType::Iff: return (left && right) || (!left && !right);
            }
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(BinaryNumericalFunctionExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expressions of this kind are currently not supported by the abstraction expression translator.");
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(BinaryRelationExpression const& expression, boost::any const& data) {
            // Check whether the expression is either fully contained in the location variables fragment or the abstracted
            // variables fragment.
            std::set<storm::expressions::Variable> variablesInExpression;
            expression.gatherVariables(variablesInExpression);
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(variablesInExpression.begin(), variablesInExpression.end(), locationVariables.begin(), locationVariables.end(), std::inserter(tmp, tmp.begin()));
            bool hasLocationVariables = !tmp.empty();
            
            tmp.clear();
            std::set_intersection(variablesInExpression.begin(), variablesInExpression.end(), abstractedVariables.begin(), abstractedVariables.end(), std::inserter(tmp, tmp.begin()));
            bool hasAbstractedVariables = !tmp.empty();
            
            STORM_LOG_THROW(hasLocationVariables || hasAbstractedVariables, storm::exceptions::NotSupportedException, "Expressions without variables are currently not supported by the abstraction expression translator.");
            STORM_LOG_THROW(!hasLocationVariables || !hasAbstractedVariables, storm::exceptions::NotSupportedException, "Expressions with two types (location variables and abstracted variables) of variables are currently not supported by the abstraction expression translator.");
            
            if (hasLocationVariables) {
                storm::dd::Add<DdType, double> left = boost::any_cast<storm::dd::Add<DdType, double>>(expression.getFirstOperand()->accept(*this, boost::none));
                storm::dd::Add<DdType, double> right = boost::any_cast<storm::dd::Add<DdType, double>>(expression.getSecondOperand()->accept(*this, boost::none));

                switch (expression.getRelationType()) {
                    case BinaryRelationExpression::RelationType::Equal: return left.equals(right);
                    case BinaryRelationExpression::RelationType::NotEqual: return left.notEquals(right);
                    case BinaryRelationExpression::RelationType::Less: return left.less(right);
                    case BinaryRelationExpression::RelationType::LessOrEqual: return left.lessOrEqual(right);
                    case BinaryRelationExpression::RelationType::Greater: return left.greater(right);
                    case BinaryRelationExpression::RelationType::GreaterOrEqual: return left.greaterOrEqual(right);
                }
            } else {
                for (uint64_t predicateIndex = 0; predicateIndex < abstractionInformation.get().getNumberOfPredicates(); ++predicateIndex) {
                    if (equivalenceChecker.areEquivalent(abstractionInformation.get().getPredicateByIndex(predicateIndex), expression.toExpression())) {
                        return abstractionInformation.get().encodePredicateAsSource(predicateIndex);
                    }
                }
                
                // At this point, none of the predicates was found to be equivalent, but there is no need to split as the subexpressions are not valid predicates.
                
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expressions of this kind are currently not supported by the abstraction expression translator.");
            }
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(VariableExpression const& expression, boost::any const& data) {
            if (abstractedVariables.find(expression.getVariable()) != abstractedVariables.end()) {
                for (uint64_t predicateIndex = 0; predicateIndex < abstractionInformation.get().getNumberOfPredicates(); ++predicateIndex) {
                    if (equivalenceChecker.areEquivalent(abstractionInformation.get().getPredicateByIndex(predicateIndex), expression.toExpression())) {
                        return abstractionInformation.get().encodePredicateAsSource(predicateIndex);
                    }
                }
                
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expressions of this kind are currently not supported by the abstraction expression translator.");
            } else {
                return abstractionInformation.get().getDdManager().template getIdentity<double>(abstractionInformation.get().getDdLocationVariable(expression.getVariable(), true));
            }
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            // Check whether the expression is either fully contained in the location variables fragment or the abstracted
            // variables fragment.
            std::set<storm::expressions::Variable> variablesInExpression;
            expression.gatherVariables(variablesInExpression);
            
            std::set<storm::expressions::Variable> tmp;
            std::set_intersection(variablesInExpression.begin(), variablesInExpression.end(), locationVariables.begin(), locationVariables.end(), std::inserter(tmp, tmp.begin()));
            bool hasLocationVariables = !tmp.empty();
            
            tmp.clear();
            std::set_intersection(variablesInExpression.begin(), variablesInExpression.end(), abstractedVariables.begin(), abstractedVariables.end(), std::inserter(tmp, tmp.begin()));
            bool hasAbstractedVariables = !tmp.empty();
            
            STORM_LOG_THROW(hasLocationVariables || hasAbstractedVariables, storm::exceptions::NotSupportedException, "Expressions without variables are currently not supported by the abstraction expression translator.");
            
            if (hasAbstractedVariables && !hasLocationVariables) {
                for (uint64_t predicateIndex = 0; predicateIndex < abstractionInformation.get().getNumberOfPredicates(); ++predicateIndex) {
                    if (equivalenceChecker.areEquivalent(abstractionInformation.get().getPredicateByIndex(predicateIndex), expression.toExpression())) {
                        return abstractionInformation.get().encodePredicateAsSource(predicateIndex);
                    }
                }
                
                // At this point, none of the predicates was found to be equivalent, so we split the expression.
            }
            
            storm::dd::Bdd<DdType> sub = boost::any_cast<storm::dd::Bdd<DdType>>(expression.getOperand()->accept(*this, boost::none));
            switch (expression.getOperatorType()) {
                case UnaryBooleanFunctionExpression::OperatorType::Not: return !sub;
            }
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(UnaryNumericalFunctionExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expressions of this kind are currently not supported by the abstraction expression translator.");
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(BooleanLiteralExpression const& expression, boost::any const& data) {
            if (expression.isTrue()) {
                return abstractionInformation.get().getDdManager().getBddOne();
            } else {
                return abstractionInformation.get().getDdManager().getBddZero();
            }
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(IntegerLiteralExpression const& expression, boost::any const& data) {
            return abstractionInformation.get().getDdManager().template getConstant<double>(expression.getValue());
        }
        
        template <storm::dd::DdType DdType>
        boost::any ExpressionTranslator<DdType>::visit(RationalLiteralExpression const& expression, boost::any const& data) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Expressions of this kind are currently not supported by the abstraction expression translator.");
        }
        
        template class ExpressionTranslator<storm::dd::DdType::CUDD>;
        template class ExpressionTranslator<storm::dd::DdType::Sylvan>;
        
    }
}
