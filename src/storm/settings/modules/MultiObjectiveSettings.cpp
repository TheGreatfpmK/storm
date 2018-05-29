#include "storm/settings/modules/MultiObjectiveSettings.h"

#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/settings/ArgumentValidators.h"


namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string MultiObjectiveSettings::moduleName = "multiobjective";
            const std::string MultiObjectiveSettings::methodOptionName = "method";
            const std::string MultiObjectiveSettings::exportPlotOptionName = "exportplot";
            const std::string MultiObjectiveSettings::precisionOptionName = "precision";
            const std::string MultiObjectiveSettings::maxStepsOptionName = "maxsteps";
            const std::string MultiObjectiveSettings::schedulerRestrictionOptionName = "schedrest";
            
            MultiObjectiveSettings::MultiObjectiveSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> methods = {"pcaa", "constraintbased"};
                this->addOption(storm::settings::OptionBuilder(moduleName, methodOptionName, true, "The method to be used for multi objective model checking.").addArgument(storm::settings::ArgumentBuilder::createStringArgument("name", "The name of the method to use.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(methods)).setDefaultValueString("pcaa").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, exportPlotOptionName, true, "Saves data for plotting of pareto curves and achievable values.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("directory", "A path to an existing directory in which the results will be saved.").build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, precisionOptionName, true, "The precision used for the approximation of numerical- and pareto queries.")
                                .addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision.").setDefaultValueDouble(1e-04).addValidatorDouble(ArgumentValidatorFactory::createDoubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, maxStepsOptionName, true, "Aborts the computation after the given number of refinement steps (= computed pareto optimal points).")
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("value", "the threshold for the number of refinement steps to be performed.").build()).build());
                std::vector<std::string> memoryPatterns = {"positional", "goalmemory", "arbitrary"};
                this->addOption(storm::settings::OptionBuilder(moduleName, schedulerRestrictionOptionName, false, "Restricts the class of considered schedulers to non-randomized schedulers with the provided memory pattern.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("memorypattern", "The Pattern of the memory.").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(memoryPatterns)).build())
                                .addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("memorystates", "The Number of memory states (only if supported by the pattern).").setDefaultValueUnsignedInteger(0).build()).build());
            }
            
            storm::modelchecker::multiobjective::MultiObjectiveMethod MultiObjectiveSettings::getMultiObjectiveMethod() const {
                std::string methodAsString = this->getOption(methodOptionName).getArgumentByName("name").getValueAsString();
                if (methodAsString == "pcaa") {
                    return storm::modelchecker::multiobjective::MultiObjectiveMethod::Pcaa;
                } else  {
                    STORM_LOG_ASSERT(methodAsString == "constraintbased", "Unexpected method name for multi objective model checking method.");
                    return storm::modelchecker::multiobjective::MultiObjectiveMethod::ConstraintBased;
                }
            }
            
            bool MultiObjectiveSettings::isExportPlotSet() const {
                return this->getOption(exportPlotOptionName).getHasOptionBeenSet();
            }
            
            std::string MultiObjectiveSettings::getExportPlotDirectory() const {
                std::string result = this->getOption(exportPlotOptionName).getArgumentByName("directory").getValueAsString();
                if (result.back() != '/') {
                    result.push_back('/');
                }
                return result;
            }
            
            double MultiObjectiveSettings::getPrecision() const {
                return this->getOption(precisionOptionName).getArgumentByName("value").getValueAsDouble();
            }
            
            bool MultiObjectiveSettings::isMaxStepsSet() const {
                return this->getOption(maxStepsOptionName).getHasOptionBeenSet();
            }
            
            uint_fast64_t MultiObjectiveSettings::getMaxSteps() const {
                return this->getOption(maxStepsOptionName).getArgumentByName("value").getValueAsUnsignedInteger();
            }
            
            bool MultiObjectiveSettings::hasSchedulerRestriction() const {
                return this->getOption(schedulerRestrictionOptionName).getHasOptionBeenSet();
            }
				
            storm::storage::SchedulerClass MultiObjectiveSettings::getSchedulerRestriction() const {
                storm::storage::SchedulerClass result;
                result.setIsDeterministic(true);
                
                std::string pattern = this->getOption(schedulerRestrictionOptionName).getArgumentByName("memorypattern").getValueAsString();
                uint64_t states = this->getOption(schedulerRestrictionOptionName).getArgumentByName("memorystates").getValueAsUnsignedInteger();
                if (pattern == "positional") {
                    result.setPositional();
                    STORM_LOG_THROW(states <=1 , storm::exceptions::IllegalArgumentException, "The number of memory states should not be provided for the given memory pattern.");
                } else if (pattern == "goalmemory") {
                    result.setMemoryPattern(storm::storage::SchedulerClass::MemoryPattern::GoalMemory);
                    STORM_LOG_THROW(states == 0, storm::exceptions::IllegalArgumentException, "The number of memory states should not be provided for the given memory pattern.");
                } else if (pattern == "arbitrary") {
                    STORM_LOG_THROW(states > 0, storm::exceptions::IllegalArgumentException, "Invalid number of memory states for provided Pattern. Please specify a positive number.");
                    result.setMemoryPattern(storm::storage::SchedulerClass::MemoryPattern::Arbitrary);
                    result.setMemoryStates(states);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "Invalid memory pattern: " + pattern + ".");
                }
                return result;
            }
				
            bool MultiObjectiveSettings::check() const {
                std::shared_ptr<storm::settings::ArgumentValidator<std::string>> validator = ArgumentValidatorFactory::createWritableFileValidator();
                
                if (isExportPlotSet()) {
                    if (!(validator->isValid(getExportPlotDirectory() + "boundaries.csv")
                        && validator->isValid(getExportPlotDirectory() + "overapproximation.csv")
                        && validator->isValid(getExportPlotDirectory() + "underapproximation.csv")
                        && validator->isValid(getExportPlotDirectory() + "paretopoints.csv"))) {
                        return false;
                    }
                }
    
                if (hasSchedulerRestriction()) {
                    getSchedulerRestriction();
                }
                
                return true;
            }

        } // namespace modules
    } // namespace settings
} // namespace storm
