#ifndef STORM_STORAGE_PRISM_SYNCHRONIZINGPARALLELCOMPOSITION_H_
#define STORM_STORAGE_PRISM_SYNCHRONIZINGPARALLELCOMPOSITION_H_

#include "src/storm/storage/prism/ParallelComposition.h"

namespace storm {
    namespace prism {
        class SynchronizingParallelComposition : public ParallelComposition {
        public:
            SynchronizingParallelComposition(std::shared_ptr<Composition> const& left, std::shared_ptr<Composition> const& right);
            
            virtual boost::any accept(CompositionVisitor& visitor, boost::any const& data) const override;
            
        protected:
            virtual void writeToStream(std::ostream& stream) const override;
        };
    }
}

#endif /* STORM_STORAGE_PRISM_SYNCHRONIZINGPARALLELCOMPOSITION_H_ */
