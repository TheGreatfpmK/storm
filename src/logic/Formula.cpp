#include "src/logic/Formulas.h"
#include <sstream>

#include "src/logic/FragmentChecker.h"
#include "src/logic/FormulaInformationVisitor.h"

namespace storm {
    namespace logic {
        bool Formula::isPathFormula() const {
            return false;
        }
        
        bool Formula::isStateFormula() const {
            return false;
        }
        
        bool Formula::isBinaryStateFormula() const {
            return false;
        }
        
        bool Formula::isUnaryStateFormula() const {
            return false;
        }
        
        bool Formula::isBinaryBooleanStateFormula() const {
            return false;
        }
        
        bool Formula::isUnaryBooleanStateFormula() const {
            return false;
        }
        
        bool Formula::isBooleanLiteralFormula() const {
            return false;
        }
        
        bool Formula::isTrueFormula() const {
            return false;
        }
        
        bool Formula::isFalseFormula() const {
            return false;
        }
        
        bool Formula::isAtomicExpressionFormula() const {
            return false;
        }
        
        bool Formula::isAtomicLabelFormula() const {
            return false;
        }
        
        bool Formula::isUntilFormula() const {
            return false;
        }
        
        bool Formula::isBoundedUntilFormula() const {
            return false;
        }
        
        bool Formula::isEventuallyFormula() const {
            return false;
        }
        
        bool Formula::isReachabilityProbabilityFormula() const {
            return false;
        }
        
        bool Formula::isGloballyFormula() const {
            return false;
        }
        
        bool Formula::isBinaryPathFormula() const {
            return false;
        }
        
        bool Formula::isUnaryPathFormula() const {
            return false;
        }
        
        bool Formula::isConditionalProbabilityFormula() const {
            return false;
        }
        
        bool Formula::isConditionalRewardFormula() const {
            return false;
        }
        
        bool Formula::isProbabilityPathFormula() const {
            return false;
        }
        
        bool Formula::isRewardPathFormula() const {
            return false;
        }
        
        bool Formula::isTimePathFormula() const {
            return false;
        }
        
        bool Formula::isNextFormula() const {
            return false;
        }
        
        bool Formula::isLongRunAverageOperatorFormula() const {
            return false;
        }
        
        bool Formula::isTimeOperatorFormula() const {
            return false;
        }
        
        bool Formula::isCumulativeRewardFormula() const {
            return false;
        }
        
        bool Formula::isInstantaneousRewardFormula() const {
            return false;
        }
        
        bool Formula::isReachabilityRewardFormula() const {
            return false;
        }
        
        bool Formula::isLongRunAverageRewardFormula() const {
            return false;
        }
        
        bool Formula::isReachabilityTimeFormula() const {
            return false;
        }
        
        bool Formula::isProbabilityOperatorFormula() const {
            return false;
        }
        
        bool Formula::isRewardOperatorFormula() const {
            return false;
        }
        
        bool Formula::isOperatorFormula() const {
            return false;
        }
        
        bool Formula::hasQualitativeResult() const {
            return true;
        }
        
        bool Formula::hasQuantitativeResult() const {
            return false;
        }
        
        bool Formula::isInFragment(FragmentSpecification const& fragment) const {
            FragmentChecker checker;
            return checker.conformsToSpecification(*this, fragment);
        }
        
        FormulaInformation Formula::info() const {
            FormulaInformationVisitor visitor;
            return visitor.getInformation(*this);
        }
        
        std::shared_ptr<Formula const> Formula::getTrueFormula() {
            return std::shared_ptr<Formula const>(new BooleanLiteralFormula(true));
        }
        
        PathFormula& Formula::asPathFormula() {
            return dynamic_cast<PathFormula&>(*this);
        }
        
        PathFormula const& Formula::asPathFormula() const {
            return dynamic_cast<PathFormula const&>(*this);
        }
        
        StateFormula& Formula::asStateFormula() {
            return dynamic_cast<StateFormula&>(*this);
        }
        
        StateFormula const& Formula::asStateFormula() const {
            return dynamic_cast<StateFormula const&>(*this);
        }
        
        BinaryStateFormula& Formula::asBinaryStateFormula() {
            return dynamic_cast<BinaryStateFormula&>(*this);
        }
        
        BinaryStateFormula const& Formula::asBinaryStateFormula() const {
            return dynamic_cast<BinaryStateFormula const&>(*this);
        }
        
        UnaryStateFormula& Formula::asUnaryStateFormula() {
            return dynamic_cast<UnaryStateFormula&>(*this);
        }
        
        UnaryStateFormula const& Formula::asUnaryStateFormula() const {
            return dynamic_cast<UnaryStateFormula const&>(*this);
        }
        
        ConditionalFormula& Formula::asConditionalFormula() {
            return dynamic_cast<ConditionalFormula&>(*this);
        }
        
        ConditionalFormula const& Formula::asConditionalFormula() const {
            return dynamic_cast<ConditionalFormula const&>(*this);
        }
        
        BinaryBooleanStateFormula& Formula::asBinaryBooleanStateFormula() {
            return dynamic_cast<BinaryBooleanStateFormula&>(*this);
        }
        
        BinaryBooleanStateFormula const& Formula::asBinaryBooleanStateFormula() const {
            return dynamic_cast<BinaryBooleanStateFormula const&>(*this);
        }
        
        UnaryBooleanStateFormula& Formula::asUnaryBooleanStateFormula() {
            return dynamic_cast<UnaryBooleanStateFormula&>(*this);
        }
        
        UnaryBooleanStateFormula const& Formula::asUnaryBooleanStateFormula() const {
            return dynamic_cast<UnaryBooleanStateFormula const&>(*this);
        }
        
        BooleanLiteralFormula& Formula::asBooleanLiteralFormula() {
            return dynamic_cast<BooleanLiteralFormula&>(*this);
        }
        
        BooleanLiteralFormula const& Formula::asBooleanLiteralFormula() const {
            return dynamic_cast<BooleanLiteralFormula const&>(*this);
        }
        
        AtomicExpressionFormula& Formula::asAtomicExpressionFormula() {
            return dynamic_cast<AtomicExpressionFormula&>(*this);
        }
        
        AtomicExpressionFormula const& Formula::asAtomicExpressionFormula() const {
            return dynamic_cast<AtomicExpressionFormula const&>(*this);
        }
        
        AtomicLabelFormula& Formula::asAtomicLabelFormula() {
            return dynamic_cast<AtomicLabelFormula&>(*this);
        }
        
        AtomicLabelFormula const& Formula::asAtomicLabelFormula() const {
            return dynamic_cast<AtomicLabelFormula const&>(*this);
        }
        
        UntilFormula& Formula::asUntilFormula() {
            return dynamic_cast<UntilFormula&>(*this);
        }
        
        UntilFormula const& Formula::asUntilFormula() const {
            return dynamic_cast<UntilFormula const&>(*this);
        }
        
        BoundedUntilFormula& Formula::asBoundedUntilFormula() {
            return dynamic_cast<BoundedUntilFormula&>(*this);
        }
        
        BoundedUntilFormula const& Formula::asBoundedUntilFormula() const {
            return dynamic_cast<BoundedUntilFormula const&>(*this);
        }
        
        EventuallyFormula& Formula::asEventuallyFormula() {
            return dynamic_cast<EventuallyFormula&>(*this);
        }
        
        EventuallyFormula const& Formula::asEventuallyFormula() const {
            return dynamic_cast<EventuallyFormula const&>(*this);
        }
        
        EventuallyFormula& Formula::asReachabilityRewardFormula() {
            return dynamic_cast<EventuallyFormula&>(*this);
        }
        
        EventuallyFormula const& Formula::asReachabilityRewardFormula() const {
            return dynamic_cast<EventuallyFormula const&>(*this);
        }
        
        EventuallyFormula& Formula::asReachabilityProbabilityFormula() {
            return dynamic_cast<EventuallyFormula&>(*this);
        }
        
        EventuallyFormula const& Formula::asReachabilityProbabilityFormula() const {
            return dynamic_cast<EventuallyFormula const&>(*this);
        }
        
        EventuallyFormula& Formula::asReachabilityTimeFormula() {
            return dynamic_cast<EventuallyFormula&>(*this);
        }

        EventuallyFormula const& Formula::asReachabilityTimeFormula() const {
            return dynamic_cast<EventuallyFormula const&>(*this);
        }
        
        GloballyFormula& Formula::asGloballyFormula() {
            return dynamic_cast<GloballyFormula&>(*this);
        }
        
        GloballyFormula const& Formula::asGloballyFormula() const {
            return dynamic_cast<GloballyFormula const&>(*this);
        }
        
        BinaryPathFormula& Formula::asBinaryPathFormula() {
            return dynamic_cast<BinaryPathFormula&>(*this);
        }
        
        BinaryPathFormula const& Formula::asBinaryPathFormula() const {
            return dynamic_cast<BinaryPathFormula const&>(*this);
        }
        
        UnaryPathFormula& Formula::asUnaryPathFormula() {
            return dynamic_cast<UnaryPathFormula&>(*this);
        }
        
        UnaryPathFormula const& Formula::asUnaryPathFormula() const {
            return dynamic_cast<UnaryPathFormula const&>(*this);
        }
        
        NextFormula& Formula::asNextFormula() {
            return dynamic_cast<NextFormula&>(*this);
        }
        
        NextFormula const& Formula::asNextFormula() const {
            return dynamic_cast<NextFormula const&>(*this);
        }
        
        LongRunAverageOperatorFormula& Formula::asLongRunAverageOperatorFormula() {
            return dynamic_cast<LongRunAverageOperatorFormula&>(*this);
        }
        
        LongRunAverageOperatorFormula const& Formula::asLongRunAverageOperatorFormula() const {
            return dynamic_cast<LongRunAverageOperatorFormula const&>(*this);
        }
        
        TimeOperatorFormula& Formula::asTimeOperatorFormula() {
            return dynamic_cast<TimeOperatorFormula&>(*this);
        }
        
        TimeOperatorFormula const& Formula::asTimeOperatorFormula() const {
            return dynamic_cast<TimeOperatorFormula const&>(*this);
        }
        
        CumulativeRewardFormula& Formula::asCumulativeRewardFormula() {
            return dynamic_cast<CumulativeRewardFormula&>(*this);
        }
        
        CumulativeRewardFormula const& Formula::asCumulativeRewardFormula() const {
            return dynamic_cast<CumulativeRewardFormula const&>(*this);
        }
        
        InstantaneousRewardFormula& Formula::asInstantaneousRewardFormula() {
            return dynamic_cast<InstantaneousRewardFormula&>(*this);
        }
        
        InstantaneousRewardFormula const& Formula::asInstantaneousRewardFormula() const {
            return dynamic_cast<InstantaneousRewardFormula const&>(*this);
        }
                
        LongRunAverageRewardFormula& Formula::asLongRunAverageRewardFormula() {
            return dynamic_cast<LongRunAverageRewardFormula&>(*this);
        }
        
        LongRunAverageRewardFormula const& Formula::asLongRunAverageRewardFormula() const {
            return dynamic_cast<LongRunAverageRewardFormula const&>(*this);
        }
        
        ProbabilityOperatorFormula& Formula::asProbabilityOperatorFormula() {
            return dynamic_cast<ProbabilityOperatorFormula&>(*this);
        }
        
        ProbabilityOperatorFormula const& Formula::asProbabilityOperatorFormula() const {
            return dynamic_cast<ProbabilityOperatorFormula const&>(*this);
        }
        
        RewardOperatorFormula& Formula::asRewardOperatorFormula() {
            return dynamic_cast<RewardOperatorFormula&>(*this);
        }
        
        RewardOperatorFormula const& Formula::asRewardOperatorFormula() const {
            return dynamic_cast<RewardOperatorFormula const&>(*this);
        }
        
        OperatorFormula& Formula::asOperatorFormula() {
            return dynamic_cast<OperatorFormula&>(*this);
        }
        
        OperatorFormula const& Formula::asOperatorFormula() const {
            return dynamic_cast<OperatorFormula const&>(*this);
        }
        
        std::vector<std::shared_ptr<AtomicExpressionFormula const>> Formula::getAtomicExpressionFormulas() const {
            std::vector<std::shared_ptr<AtomicExpressionFormula const>> result;
            this->gatherAtomicExpressionFormulas(result);
            return result;
        }
        
        std::vector<std::shared_ptr<AtomicLabelFormula const>> Formula::getAtomicLabelFormulas() const {
            std::vector<std::shared_ptr<AtomicLabelFormula const>> result;
            this->gatherAtomicLabelFormulas(result);
            return result;
        }
        
        std::set<std::string> Formula::getReferencedRewardModels() const {
            std::set<std::string> referencedRewardModels;
            this->gatherReferencedRewardModels(referencedRewardModels);
            return referencedRewardModels;
        }
        
        std::shared_ptr<Formula const> Formula::asSharedPointer() {
            return this->shared_from_this();
        }
        
        std::shared_ptr<Formula const> Formula::asSharedPointer() const {
            return this->shared_from_this();
        }
        
        void Formula::gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const {
            return;
        }
        
        void Formula::gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const {
            return;
        }
        
        void Formula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
            return;
        }
                
        std::string Formula::toString() const {
            std::stringstream str2;
            writeToStream(str2);
            return str2.str();
        }
        
        std::ostream& operator<<(std::ostream& out, Formula const& formula) {
            return formula.writeToStream(out);
        }
    }
}