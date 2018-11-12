#pragma once

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/SymbolicQualitativeResultMinMax.h"
#include "storm/abstraction/SymbolicQualitativeMdpResult.h"

namespace storm {
    namespace abstraction {
        
        template<storm::dd::DdType Type>
        class SymbolicQualitativeMdpResultMinMax : public SymbolicQualitativeResultMinMax<Type> {
        public:
            SymbolicQualitativeMdpResultMinMax() = default;
            
            virtual SymbolicQualitativeResult<Type> const& getProb0(storm::OptimizationDirection const& dir) const override;
            virtual SymbolicQualitativeResult<Type> const& getProb1(storm::OptimizationDirection const& dir) const override;
            
            SymbolicQualitativeMdpResult<Type> prob0Min;
            SymbolicQualitativeMdpResult<Type> prob1Min;
            SymbolicQualitativeMdpResult<Type> prob0Max;
            SymbolicQualitativeMdpResult<Type> prob1Max;
        };
        
    }
}
