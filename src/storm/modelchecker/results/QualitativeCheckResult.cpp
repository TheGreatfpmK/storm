#include "src/storm/modelchecker/results/QualitativeCheckResult.h"

#include "src/storm/utility/macros.h"
#include "src/storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace modelchecker {
        QualitativeCheckResult& QualitativeCheckResult::operator&=(QualitativeCheckResult const& other) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform logical 'and' on the two check results.");
        }
        
        QualitativeCheckResult& QualitativeCheckResult::operator|=(QualitativeCheckResult const& other) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform logical 'or' on the two check results.");
        }
        
        void QualitativeCheckResult::complement() {
            STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Unable to perform logical 'not' on the check result.");
        }
        
        bool QualitativeCheckResult::isQualitative() const {
            return true;
        }
    }
}
