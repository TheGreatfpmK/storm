#include "src/settings/SettingsManager.h"

#include <cstring>
#include <cctype>
#include <mutex>
#include <iomanip>
#include <fstream>
#include <regex>
#include <set>
#include <boost/algorithm/string.hpp>
#include <boost/io/ios_state.hpp>

#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/OptionParserException.h"
#include "src/utility/storm-version.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/DebugSettings.h"
#include "src/settings/modules/CounterexampleGeneratorSettings.h"
#include "src/settings/modules/CuddSettings.h"
#include "src/settings/modules/SylvanSettings.h"
#include "src/settings/modules/GmmxxEquationSolverSettings.h"
#include "src/settings/modules/NativeEquationSolverSettings.h"
#include "src/settings/modules/BisimulationSettings.h"
#include "src/settings/modules/GlpkSettings.h"
#include "src/settings/modules/GurobiSettings.h"
#include "src/settings/modules/ParametricSettings.h"
#include "src/settings/modules/SparseDtmcEliminationModelCheckerSettings.h"
#include "src/settings/modules/TopologicalValueIterationEquationSolverSettings.h"
#include "src/utility/macros.h"
#include "src/settings/Option.h"


namespace storm {
    namespace settings {
        
        SettingsManager::SettingsManager() : modules(), longNameToOptions(), shortNameToOptions(), moduleOptions() {
            // Register all known settings modules.
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GeneralSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::DebugSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::CounterexampleGeneratorSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::CuddSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::SylvanSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GmmxxEquationSolverSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::NativeEquationSolverSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::BisimulationSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GlpkSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::GurobiSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::TopologicalValueIterationEquationSolverSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::ParametricSettings()));
            this->addModule(std::unique_ptr<modules::ModuleSettings>(new modules::SparseDtmcEliminationModelCheckerSettings()));
        }
        
        SettingsManager::~SettingsManager() {
            // Intentionally left empty.
        }
        
        SettingsManager& SettingsManager::manager() {
            static SettingsManager settingsManager;
            return settingsManager;
        }
        
        void SettingsManager::setFromCommandLine(int const argc, char const * const argv[]) {
            // We convert the arguments to a vector of strings and strip off the first element since it refers to the
            // name of the program.
            std::vector<std::string> argumentVector(argc - 1);
            for (int i = 1; i < argc; ++i) {
                argumentVector[i - 1] = std::string(argv[i]);
            }
            
            this->setFromExplodedString(argumentVector);
        }
        
        void SettingsManager::setFromString(std::string const& commandLineString) {
            if (commandLineString.empty()) {
                return;
            }
            std::vector<std::string> argumentVector;
            boost::split(argumentVector, commandLineString, boost::is_any_of("\t "));
            this->setFromExplodedString(argumentVector);
        }
        
        void SettingsManager::setFromExplodedString(std::vector<std::string> const& commandLineArguments) {
            // In order to assign the parsed arguments to an option, we need to keep track of the "active" option's name.
            bool optionActive = false;
            bool activeOptionIsShortName = false;
            std::string activeOptionName = "";
            std::vector<std::string> argumentCache;
            
            // Walk through all arguments.
            for (uint_fast64_t i = 0; i < commandLineArguments.size(); ++i) {
                std::string const& currentArgument = commandLineArguments[i];
                
                // Check if the given argument is a new option or belongs to a previously given option.
                if (!currentArgument.empty() && currentArgument.at(0) == '-') {
                    if (optionActive) {
                        // At this point we know that a new option is about to come. Hence, we need to assign the current
                        // cache content to the option that was active until now.
                        setOptionsArguments(activeOptionName, activeOptionIsShortName ? this->shortNameToOptions : this->longNameToOptions, argumentCache);
                        
                        // After the assignment, the argument cache needs to be cleared.
                        argumentCache.clear();
                    } else {
                        optionActive = true;
                    }
                    
                    if (currentArgument.at(1) == '-') {
                        // In this case, the argument has to be the long name of an option. Try to get all options that
                        // match the long name.
                        std::string optionName = currentArgument.substr(2);
                        auto optionIterator = this->longNameToOptions.find(optionName);
                        STORM_LOG_THROW(optionIterator != this->longNameToOptions.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
                        activeOptionIsShortName = false;
                        activeOptionName = optionName;
                    } else {
                        // In this case, the argument has to be the short name of an option. Try to get all options that
                        // match the short name.
                        std::string optionName = currentArgument.substr(1);
                        auto optionIterator = this->shortNameToOptions.find(optionName);
                        STORM_LOG_THROW(optionIterator != this->shortNameToOptions.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
                        activeOptionIsShortName = true;
                        activeOptionName = optionName;
                    }
                } else if (optionActive) {
                    // Add the current argument to the list of arguments for the currently active options.
                    argumentCache.push_back(currentArgument);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::OptionParserException, "Found stray argument '" << currentArgument << "' that is not preceeded by a matching option.");
                }
            }
            
            // If an option is still active at this point, we need to set it.
            if (optionActive) {
                setOptionsArguments(activeOptionName, activeOptionIsShortName ? this->shortNameToOptions : this->longNameToOptions, argumentCache);
            }

            // Include the options from a possibly specified configuration file, but don't overwrite existing settings.
            if (storm::settings::generalSettings().isConfigSet()) {
                this->setFromConfigurationFile(storm::settings::generalSettings().getConfigFilename());
            }
            
            // Finally, check whether all modules are okay with the current settings.
            this->finalizeAllModules();
        }
        
        void SettingsManager::setFromConfigurationFile(std::string const& configFilename) {
            std::map<std::string, std::vector<std::string>> configurationFileSettings = parseConfigFile(configFilename);
            
            for (auto const& optionArgumentsPair : configurationFileSettings) {
                auto options = this->longNameToOptions.find(optionArgumentsPair.first);
                
                // We don't need to check whether this option exists or not, because this is already checked when
                // parsing the configuration file.
                
                // Now go through all the matching options and set them according to the values.
                for (auto option : options->second) {
                    if (option->getHasOptionBeenSet()) {
                        // If the option was already set from the command line, we issue a warning and ignore the
                        // settings from the configuration file.
                        STORM_LOG_WARN("The option '" << option->getLongName() << "' of module '" << option->getModuleName() << "' has been set in the configuration file '" << configFilename << "', but was overwritten on the command line." << std::endl);
                    } else {
                        // If, however, the option has not been set yet, we try to assign values ot its arguments
                        // based on the argument strings.
                        setOptionArguments(optionArgumentsPair.first, option, optionArgumentsPair.second);
                    }
                }
            }
            // Finally, check whether all modules are okay with the current settings.
            this->finalizeAllModules();
        }
        
        void SettingsManager::printHelp(std::string const& hint) const {
            STORM_PRINT("usage: storm [options]" << std::endl << std::endl);
            
            if (hint == "all") {
                // Find longest option name.
                uint_fast64_t maxLength = getPrintLengthOfLongestOption();
                for (auto const& moduleName : this->moduleNames) {
                    printHelpForModule(moduleName, maxLength);
                }
            } else {
                // Create a regular expression from the input hint.
                std::regex hintRegex(hint, std::regex_constants::ECMAScript | std::regex_constants::icase);
                
                // Remember which options we printed, so we don't display options twice.
                std::set<std::shared_ptr<Option>> printedOptions;
                
                // Try to match the regular expression against the known modules.
                std::vector<std::string> matchingModuleNames;
                uint_fast64_t maxLengthModules = 0;
                for (auto const& moduleName : this->moduleNames) {
                    if (std::regex_search(moduleName, hintRegex)) {
                        matchingModuleNames.push_back(moduleName);
                        maxLengthModules = std::max(maxLengthModules, getPrintLengthOfLongestOption(moduleName));
                        
                        // Add all options of this module to the list of printed options so we don't print them twice.
                        auto optionIterator = this->moduleOptions.find(moduleName);
                        printedOptions.insert(optionIterator->second.begin(), optionIterator->second.end());
                    }
                }

                // Try to match the regular expression against the known options.
                std::vector<std::shared_ptr<Option>> matchingOptions;
                uint_fast64_t maxLengthOptions = 0;
                for (auto const& optionName : this->longOptionNames) {
                    if (std::regex_search(optionName, hintRegex)) {
                        auto optionIterator = this->longNameToOptions.find(optionName);
                        for (auto const& option : optionIterator->second) {
                            // Only add the option if we have not already added it to the list of options that is going
                            // to be printed anyway.
                            if (printedOptions.find(option) == printedOptions.end()) {
                                maxLengthOptions = std::max(maxLengthOptions, option->getPrintLength());
                                matchingOptions.push_back(option);
                            }
                        }
                    }
                }
                
                // Print the matching modules.
                uint_fast64_t maxLength = std::max(maxLengthModules, maxLengthOptions);
                if (matchingModuleNames.size() > 0) {
                    STORM_PRINT("Matching modules for hint '" << hint << "':" << std::endl)
                    for (auto const& matchingModuleName : matchingModuleNames) {
                        printHelpForModule(matchingModuleName, maxLength);
                    }
                }
                
                // Print the matching options.
                if (matchingOptions.size() > 0) {
                    STORM_PRINT("Matching options for hint '" << hint << "':" << std::endl);
                    for (auto const& option : matchingOptions) {
                        STORM_PRINT(std::setw(maxLength) << std::left << *option << std::endl);
                    }
                }
                
                if (matchingModuleNames.empty() && matchingOptions.empty()) {
                    STORM_PRINT("Hint '" << hint << "' did not match any modules or options." << std::endl);
                }
            }
        }
        
        void SettingsManager::printHelpForModule(std::string const& moduleName, uint_fast64_t maxLength) const {
            auto moduleIterator = moduleOptions.find(moduleName);
            STORM_LOG_THROW(moduleIterator != moduleOptions.end(), storm::exceptions::IllegalFunctionCallException, "Cannot print help for unknown module '" << moduleName << "'.");
            STORM_PRINT("##### Module '" << moduleName << "' " << std::string(std::min(maxLength, maxLength - moduleName.length() - 16), '#') << std::endl);
            
            // Save the flags for std::cout so we can manipulate them and be sure they will be restored as soon as this
            // stream goes out of scope.
            boost::io::ios_flags_saver out(std::cout);
            
            for (auto const& option : moduleIterator->second) {
                STORM_PRINT(std::setw(maxLength) << std::left << *option << std::endl);
            }
            STORM_PRINT(std::endl);
        }
        
        void SettingsManager::printVersion() const {
            STORM_PRINT(storm::utility::StormVersion::shortVersionString());
            STORM_PRINT(std::endl);
        }
        
        uint_fast64_t SettingsManager::getPrintLengthOfLongestOption() const {
            uint_fast64_t length = 0;
            for (auto const& moduleName : this->moduleNames) {
                length = std::max(getPrintLengthOfLongestOption(moduleName), length);
            }
            return length;
        }
        
        uint_fast64_t SettingsManager::getPrintLengthOfLongestOption(std::string const& moduleName) const {
            auto moduleIterator = modules.find(moduleName);
            STORM_LOG_THROW(moduleIterator != modules.end(), storm::exceptions::IllegalFunctionCallException, "Unable to retrieve option length of unknown module '" << moduleName << "'.");
            return moduleIterator->second->getPrintLengthOfLongestOption();
        }
        
        void SettingsManager::addModule(std::unique_ptr<modules::ModuleSettings>&& moduleSettings) {
            auto moduleIterator = this->modules.find(moduleSettings->getModuleName());
            STORM_LOG_THROW(moduleIterator == this->modules.end(), storm::exceptions::IllegalFunctionCallException, "Unable to register module '" << moduleSettings->getModuleName() << "' because a module with the same name already exists.");
            
            // Take over the module settings object.
            std::string const& moduleName = moduleSettings->getModuleName();
            this->moduleNames.push_back(moduleName);
            this->modules.emplace(moduleSettings->getModuleName(), std::move(moduleSettings));
            auto iterator = this->modules.find(moduleName);
            std::unique_ptr<modules::ModuleSettings> const& settings = iterator->second;
            
            // Now register the options of the module.
            this->moduleOptions.emplace(moduleName, std::vector<std::shared_ptr<Option>>());
            for (auto const& option : settings->getOptions()) {
                this->addOption(option);
            }
        }
        
        void SettingsManager::addOption(std::shared_ptr<Option> const& option) {
            // First, we register to which module the given option belongs.
            auto moduleOptionIterator = this->moduleOptions.find(option->getModuleName());
            STORM_LOG_THROW(moduleOptionIterator != this->moduleOptions.end(), storm::exceptions::IllegalFunctionCallException, "Cannot add option for unknown module '" << option->getModuleName() << "'.");
            moduleOptionIterator->second.emplace_back(option);
            
            // Then, we add the option's name (and possibly short name) to the registered options. If a module prefix is
            // not required for this option, we have to add both versions to our mappings, the prefixed one and the
            // non-prefixed one.
            if (!option->getRequiresModulePrefix()) {
                bool isCompatible = this->isCompatible(option, option->getLongName(), this->longNameToOptions);
                STORM_LOG_THROW(isCompatible, storm::exceptions::IllegalFunctionCallException, "Unable to add option '" << option->getLongName() << "', because an option with the same name is incompatible with it.");
                addOptionToMap(option->getLongName(), option, this->longNameToOptions);
            }
            // For the prefixed name, we don't need a compatibility check, because a module is not allowed to register the same option twice.
            addOptionToMap(option->getModuleName() + ":" + option->getLongName(), option, this->longNameToOptions);
            longOptionNames.push_back(option->getModuleName() + ":" + option->getLongName());
            
            if (option->getHasShortName()) {
                if (!option->getRequiresModulePrefix()) {
                    bool isCompatible = this->isCompatible(option, option->getShortName(), this->shortNameToOptions);
                    STORM_LOG_THROW(isCompatible, storm::exceptions::IllegalFunctionCallException, "Unable to add option '" << option->getLongName() << "', because an option with the same name is incompatible with it.");
                    addOptionToMap(option->getShortName(), option, this->shortNameToOptions);
                }
                addOptionToMap(option->getModuleName() + ":" + option->getShortName(), option, this->shortNameToOptions);
            }
        }
        
        modules::ModuleSettings const& SettingsManager::getModule(std::string const& moduleName) const {
            auto moduleIterator = this->modules.find(moduleName);
            STORM_LOG_THROW(moduleIterator != this->modules.end(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve unknown module '" << moduleName << "'.");
            return *moduleIterator->second;
        }
        
        modules::ModuleSettings& SettingsManager::getModule(std::string const& moduleName) {
            auto moduleIterator = this->modules.find(moduleName);
            STORM_LOG_THROW(moduleIterator != this->modules.end(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve unknown module '" << moduleName << "'.");
            return *moduleIterator->second;
        }
        
        bool SettingsManager::isCompatible(std::shared_ptr<Option> const& option, std::string const& optionName, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> const& optionMap) {
            auto optionIterator = optionMap.find(optionName);
            if (optionIterator != optionMap.end()) {
                for (auto const& otherOption : optionIterator->second) {
                    bool locallyCompatible = option->isCompatibleWith(*otherOption);
                    if (!locallyCompatible) {
                        return false;
                    }
                }
            }
            return true;
        }
        
        void SettingsManager::setOptionArguments(std::string const& optionName, std::shared_ptr<Option> option, std::vector<std::string> const& argumentCache) {
            STORM_LOG_THROW(argumentCache.size() <= option->getArgumentCount(), storm::exceptions::OptionParserException, "Too many arguments for option '" << optionName << "'.");
            
            // Now set the provided argument values one by one.
            for (uint_fast64_t i = 0; i < argumentCache.size(); ++i) {
                ArgumentBase& argument = option->getArgument(i);
                bool conversionOk = argument.setFromStringValue(argumentCache[i]);
                STORM_LOG_THROW(conversionOk, storm::exceptions::OptionParserException, "Conversion of value of argument '" << argument.getName() << "' to its type failed.");
            }
            
            // In case there are optional arguments that were not set, we set them to their default value.
            for (uint_fast64_t i = argumentCache.size(); i < option->getArgumentCount(); ++i) {
                ArgumentBase& argument = option->getArgument(i);
                argument.setFromDefaultValue();
            }
            
            option->setHasOptionBeenSet();
        }
        
        void SettingsManager::setOptionsArguments(std::string const& optionName, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>> const& optionMap, std::vector<std::string> const& argumentCache) {
            auto optionIterator = optionMap.find(optionName);
            STORM_LOG_THROW(optionIterator != optionMap.end(), storm::exceptions::OptionParserException, "Unknown option '" << optionName << "'.");
            
            // Iterate over all options and set the arguments.
            for (auto& option : optionIterator->second) {
                setOptionArguments(optionName, option, argumentCache);
            }
        }
        
        void SettingsManager::addOptionToMap(std::string const& name, std::shared_ptr<Option> const& option, std::unordered_map<std::string, std::vector<std::shared_ptr<Option>>>& optionMap) {
            auto optionIterator = optionMap.find(name);
            if (optionIterator == optionMap.end()) {
                std::vector<std::shared_ptr<Option>> optionVector;
                optionVector.push_back(option);
                optionMap.emplace(name, optionVector);
            } else {
                optionIterator->second.push_back(option);
            }
        }
        
        void SettingsManager::finalizeAllModules() {
            for (auto const& nameModulePair : this->modules) {
                nameModulePair.second->finalize();
                nameModulePair.second->check();

            }
        }
        
        std::map<std::string, std::vector<std::string>> SettingsManager::parseConfigFile(std::string const& filename) const {
            std::map<std::string, std::vector<std::string>> result;
            
            std::ifstream input(filename);
            STORM_LOG_THROW(input.good(), storm::exceptions::OptionParserException, "Could not read from config file '" << filename << "'.");

            bool globalScope = true;
            std::string activeModule = "";
            uint_fast64_t lineNumber = 1;
            for (std::string line; getline(input, line); ++lineNumber) {
                // If the first character of the line is a "[", we expect the settings of a new module to start and
                // the line to be of the shape [<module>].
                if (line.at(0) == '[') {
                    STORM_LOG_THROW(line.at(0) == '[' && line.find("]") == line.length() - 1 && line.find("[", 1) == line.npos, storm::exceptions::OptionParserException, "Illegal module name header in configuration file '" << filename << " in line " << std::to_string(lineNumber) << ". Expected [<module>] where <module> is a placeholder for a known module.");
                    
                    // Extract the module name and check whether it's a legal one.
                    std::string moduleName = line.substr(1, line.length() - 2);
                    STORM_LOG_THROW(moduleName != "" && (moduleName == "global" || (this->modules.find(moduleName) != this->modules.end())), storm::exceptions::OptionParserException, "Module header in configuration file '" << filename << " in line " << std::to_string(lineNumber) << " refers to unknown module '" << moduleName << ".");
                    
                    // If the module name is "global", we unset the currently active module and treat all options to follow as unprefixed.
                    if (moduleName == "global") {
                        globalScope = true;
                    } else {
                        activeModule = moduleName;
                        globalScope = false;
                    }
                } else {
                    // In this case, we expect the line to be of the shape o or o=a b c, where o is an option and a, b
                    // and c are the values that are supposed to be assigned to the arguments of the option.
                    std::size_t assignmentSignIndex = line.find("=");
                    bool containsAssignment = false;
                    if (assignmentSignIndex != line.npos) {
                        containsAssignment = true;
                    }
                    
                    std::string optionName;
                    if (containsAssignment) {
                        optionName = line.substr(0, assignmentSignIndex);
                    } else {
                        optionName = line;
                    }
                    
                    if (globalScope) {
                        STORM_LOG_THROW(this->longNameToOptions.find(optionName) != this->longNameToOptions.end(), storm::exceptions::OptionParserException, "Option assignment in configuration file '" << filename << " in line " << lineNumber << " refers to unknown option '" << optionName << "'.");
                    } else {
                        STORM_LOG_THROW(this->longNameToOptions.find(activeModule + ":" + optionName) != this->longNameToOptions.end(), storm::exceptions::OptionParserException, "Option assignment in configuration file '" << filename << " in line " << lineNumber << " refers to unknown option '" << activeModule << ":" <<  optionName << "'.");
                    }
                    
                    std::string fullOptionName = (!globalScope ? activeModule + ":" : "") + optionName;
                    STORM_LOG_WARN_COND(result.find(fullOptionName) == result.end(), "Option '" << fullOptionName << "' is set in line " << lineNumber << " of configuration file " << filename << ", but has been set before.");
                    
                    // If the current line is an assignment, split the right-hand side of the assignment into parts
                    // enclosed by quotation marks.
                    if (containsAssignment) {
                        std::string assignedValues = line.substr(assignmentSignIndex + 1);
                        std::vector<std::string> argumentCache;
                    
                        // As horrible as it may look, this regular expression matches either a quoted string (possibly
                        // containing escaped quotes) or a simple word (without whitespaces and quotes).
                        std::regex argumentRegex("\"(([^\\\\\"]|((\\\\\\\\)*\\\\\")|\\\\[^\"])*)\"|(([^ \\\\\"]|((\\\\\\\\)*\\\\\")|\\\\[^\"])+)");
                        boost::algorithm::trim_left(assignedValues);
                        
                        while (!assignedValues.empty()) {
                            std::smatch match;
                            bool hasMatch = std::regex_search(assignedValues, match, argumentRegex);
                            
                            // If the input could not be matched, we have a parsing error.
                            STORM_LOG_THROW(hasMatch, storm::exceptions::OptionParserException, "Parsing error in configuration file '" << filename << "' in line " << lineNumber << ". Unexpected input '" << assignedValues << "'.");

                            // Extract the matched argument and cut off the quotation marks if necessary.
                            std::string matchedArgument = std::string(match[0].first, match[0].second);
                            if (matchedArgument.at(0) == '"') {
                                matchedArgument = matchedArgument.substr(1, matchedArgument.length() - 2);
                            }
                            argumentCache.push_back(matchedArgument);
                            
                            assignedValues = assignedValues.substr(match.length());
                            boost::algorithm::trim_left(assignedValues);
                        }
                        
                        
                        // After successfully parsing the argument values, we store them in the result map.
                        result.emplace(fullOptionName, argumentCache);
                    } else {
                        // In this case, we can just insert the option to indicate it should be set (without arguments).
                        result.emplace(fullOptionName, std::vector<std::string>());
                    }
                }
            }
            
            return result;
        }
        
        SettingsManager const& manager() {
            return SettingsManager::manager();
        }
        
        SettingsManager& mutableManager() {
            return SettingsManager::manager();
        }
        
        storm::settings::modules::GeneralSettings const& generalSettings() {
            return dynamic_cast<storm::settings::modules::GeneralSettings const&>(manager().getModule(storm::settings::modules::GeneralSettings::moduleName));
        }
        
        storm::settings::modules::GeneralSettings& mutableGeneralSettings() {
            return dynamic_cast<storm::settings::modules::GeneralSettings&>(storm::settings::SettingsManager::manager().getModule(storm::settings::modules::GeneralSettings::moduleName));
        }
        
        storm::settings::modules::DebugSettings const& debugSettings()  {
            return dynamic_cast<storm::settings::modules::DebugSettings const&>(manager().getModule(storm::settings::modules::DebugSettings::moduleName));
        }
        
        storm::settings::modules::CounterexampleGeneratorSettings const& counterexampleGeneratorSettings() {
            return dynamic_cast<storm::settings::modules::CounterexampleGeneratorSettings const&>(manager().getModule(storm::settings::modules::CounterexampleGeneratorSettings::moduleName));
        }
        
        storm::settings::modules::CuddSettings const& cuddSettings() {
            return dynamic_cast<storm::settings::modules::CuddSettings const&>(manager().getModule(storm::settings::modules::CuddSettings::moduleName));
        }

        storm::settings::modules::SylvanSettings const& sylvanSettings() {
            return dynamic_cast<storm::settings::modules::SylvanSettings const&>(manager().getModule(storm::settings::modules::SylvanSettings::moduleName));
        }
        
        storm::settings::modules::GmmxxEquationSolverSettings const& gmmxxEquationSolverSettings() {
            return dynamic_cast<storm::settings::modules::GmmxxEquationSolverSettings const&>(manager().getModule(storm::settings::modules::GmmxxEquationSolverSettings::moduleName));
        }
        
        storm::settings::modules::NativeEquationSolverSettings const& nativeEquationSolverSettings() {
            return dynamic_cast<storm::settings::modules::NativeEquationSolverSettings const&>(manager().getModule(storm::settings::modules::NativeEquationSolverSettings::moduleName));
        }
        
        storm::settings::modules::BisimulationSettings const& bisimulationSettings() {
            return dynamic_cast<storm::settings::modules::BisimulationSettings const&>(manager().getModule(storm::settings::modules::BisimulationSettings::moduleName));
        }
        
        storm::settings::modules::GlpkSettings const& glpkSettings() {
            return dynamic_cast<storm::settings::modules::GlpkSettings const&>(manager().getModule(storm::settings::modules::GlpkSettings::moduleName));
        }
        
        storm::settings::modules::GurobiSettings const& gurobiSettings() {
            return dynamic_cast<storm::settings::modules::GurobiSettings const&>(manager().getModule(storm::settings::modules::GurobiSettings::moduleName));
		}

		storm::settings::modules::TopologicalValueIterationEquationSolverSettings const& topologicalValueIterationEquationSolverSettings() {
			return dynamic_cast<storm::settings::modules::TopologicalValueIterationEquationSolverSettings const&>(manager().getModule(storm::settings::modules::TopologicalValueIterationEquationSolverSettings::moduleName));
		}
        
        storm::settings::modules::ParametricSettings const& parametricSettings() {
            return dynamic_cast<storm::settings::modules::ParametricSettings const&>(manager().getModule(storm::settings::modules::ParametricSettings::moduleName));
        }

        storm::settings::modules::SparseDtmcEliminationModelCheckerSettings const& sparseDtmcEliminationModelCheckerSettings() {
            return dynamic_cast<storm::settings::modules::SparseDtmcEliminationModelCheckerSettings const&>(manager().getModule(storm::settings::modules::SparseDtmcEliminationModelCheckerSettings::moduleName));
        }
    }
}