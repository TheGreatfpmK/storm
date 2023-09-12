#pragma once

#include "storm/storage/prism/Program.h"
#include "storm/models/sparse/Model.h"


namespace storm {
    namespace synthesis {

        template<typename ValueType>
        class ObservationEvaluator {

        public:

            ObservationEvaluator(
                storm::prism::Program & prism,
                storm::models::sparse::Model<ValueType> const& model
            );

            /** Number of observation expressions. */
            uint32_t num_obs_expressions;
            /** For each observation expression its label. */
            std::vector<std::string> obs_expr_label;
            /** For each observation expression whether it is boolean. */
            std::vector<bool> obs_expr_is_boolean;
            
            /** Number of observation ids. */
            uint32_t num_obs_ids = 0;
            /** For each state its observation id. */
            std::vector<uint32_t> state_to_obs_id;

            uint64_t observationIdValue(uint32_t obs_id, uint32_t obs_expr);

        private:
            /** Bitwidth of observation expression value size. */
            static const int OBS_EXPR_VALUE_SIZE = 64;
            /** Mapping of observation expressions evaluation to a unique observation id. */
            std::map<storm::storage::BitVector,uint32_t> obs_evaluation_to_id;
            /** Mapping of observation id to observation expressions evaluation. */
            std::vector<storm::storage::BitVector> id_to_obs_evaluation;
            
        };

    }
}