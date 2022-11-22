#pragma once

#include "storm/models/sparse/Pomdp.h"
#include "storm/logic/Formula.h"

namespace storm {
    namespace synthesis {

        class SubPomdpBuilder {

        public:

            SubPomdpBuilder(
                storm::models::sparse::Pomdp<double> const& pomdp,
                std::string const& reward_name,
                std::string const& target_label
            );

            void setRelevantStates(storm::storage::BitVector const& relevant_states);

            storm::storage::BitVector const& getHorizonStates();

            std::shared_ptr<storm::models::sparse::Pomdp<double>> restrictPomdp(
                std::map<uint64_t,double> const& initial_belief,
                std::map<uint64_t,double> const& horizon_values
            );

        private:

            // original POMDP
            storm::models::sparse::Pomdp<double> const& pomdp;
            // name of the investigated reward
            std::string const& reward_name;
            // label assigned to target states
            std::string const& target_label;
            // for each state, a list of immediate successors (excluding state itself)
            std::vector<std::set<uint64_t>> reachable_successors;
            

            // index of the new initial state
            const uint64_t initial_state = 0;
            // index of the new sink state
            const uint64_t sink_state = 1;
            // label associated with initial distribution as well as shortcut actions
            const std::string empty_label = "";

            // states relevant for the current restriction
            storm::storage::BitVector relevant_states;
            // horizon states reachable from the relevant ones
            storm::storage::BitVector horizon_states;
            // total number of states in the sub-POMDP
            // (equal to the number of reachable states + 2)
            uint64_t num_states;
            // for each state of a POMDP its index in the sub-POMDP
            // (0 for unreachable states)
            std::vector<uint64_t> state_map;
            
            
            storm::storage::SparseMatrix<double> constructTransitionMatrix(
                std::map<uint64_t,double> const& initial_belief
            );
            storm::models::sparse::StateLabeling constructStateLabeling();
            storm::models::sparse::ChoiceLabeling constructChoiceLabeling(uint64_t num_rows);
            std::vector<uint32_t> constructObservabilityClasses();
            storm::models::sparse::StandardRewardModel<double> constructRewardModel(
                uint64_t num_rows,
                std::map<uint64_t,double> const& horizon_values
            );
        

        };




    }
}