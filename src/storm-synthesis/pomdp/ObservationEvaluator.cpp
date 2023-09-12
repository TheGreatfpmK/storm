#include "storm-synthesis/pomdp/ObservationEvaluator.h"

#include "storm/exceptions/InvalidTypeException.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

#include <map>

namespace storm {
    namespace synthesis {

        template<typename ValueType>
        ObservationEvaluator<ValueType>::ObservationEvaluator(
            storm::prism::Program & prism,
            storm::models::sparse::Model<ValueType> const& model
        ) {
            
            // substitute constanst and simplify formulas in the program
            prism = prism.substituteConstantsFormulas(true,true);

            // identify names and types of observation labels
            this->num_obs_expressions = prism.getNumberOfObservationLabels();
            this->obs_expr_label.resize(this->num_obs_expressions);
            this->obs_expr_is_boolean.resize(this->num_obs_expressions);

            for(uint32_t o = 0; o < this->num_obs_expressions; o++) {
                auto const& obs_label = prism.getObservationLabels()[o];
                obs_expr_label[o] = obs_label.getName();
                auto const& obs_expr = obs_label.getStatePredicateExpression();
                STORM_LOG_THROW(obs_expr.hasBooleanType() or obs_expr.hasIntegerType(), storm::exceptions::InvalidTypeException,
                    "expected boolean or integer observation expression");
                this->obs_expr_is_boolean[o] = obs_expr.hasBooleanType();
            }

            // evaluate observation expression for each state valuation
            storm::expressions::ExpressionEvaluator<double> evaluator(prism.getManager());
            auto const& state_valuations = model.getStateValuations();
            // associate each evaluation with the unique observation id
            this->state_to_obs_id.resize(model.getNumberOfStates());
            this->num_obs_ids = 0;
            for(uint_fast64_t state = 0; state < model.getNumberOfStates(); state++) {

                // collect state valuation into evaluator
                for(auto it = state_valuations.at(state).begin(); it != state_valuations.at(state).end(); ++it) {
                    auto const& var = it.getVariable();
                    STORM_LOG_THROW(it.isBoolean() or it.isInteger(), storm::exceptions::InvalidTypeException,
                        "expected boolean or integer variable");
                    // we pass Jani variables to the evaluator, but it seems to work, perhaps it works with variable names
                    if(it.isBoolean()) {
                        evaluator.setBooleanValue(var, it.getBooleanValue());
                    } else if(it.isInteger()) {
                        evaluator.setIntegerValue(var, it.getIntegerValue());
                    }
                }
                
                // evaluate observation expressions and assign id
                storm::storage::BitVector evaluation(OBS_EXPR_VALUE_SIZE*num_obs_expressions);
                for (uint32_t o = 0; o < num_obs_expressions; o++) {
                    evaluation.setFromInt(
                        OBS_EXPR_VALUE_SIZE*o,
                        OBS_EXPR_VALUE_SIZE,
                        evaluator.asInt(prism.getObservationLabels()[o].getStatePredicateExpression())
                    );
                }
                auto result = this->obs_evaluation_to_id.insert(std::make_pair(evaluation,num_obs_ids));
                if(not result.second) {
                    // existing evaluation
                    this->state_to_obs_id[state] = result.first->second;
                } else {
                    // new evaluation
                    this->state_to_obs_id[state] = num_obs_ids;
                    this->id_to_obs_evaluation.push_back(evaluation);
                    this->num_obs_ids++;
                }
            }
        }

        template<typename ValueType>
        uint64_t ObservationEvaluator<ValueType>::observationIdValue(uint32_t obs_id, uint32_t obs_expr) {
            return this->id_to_obs_evaluation[obs_id].getAsInt(OBS_EXPR_VALUE_SIZE*obs_expr,OBS_EXPR_VALUE_SIZE);
        }

    
        template class ObservationEvaluator<double>;

    }
}