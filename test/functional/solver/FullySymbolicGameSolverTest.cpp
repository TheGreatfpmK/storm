#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/storm/storage/dd/DdManager.h"
#include "src/storm/utility/solver.h"
#include "src/storm/settings/SettingsManager.h"

#include "src/storm/solver/SymbolicGameSolver.h"
#include "src/storm/settings/modules/NativeEquationSolverSettings.h"

TEST(FullySymbolicGameSolverTest, Solve_Cudd) {
    // Create some variables.
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::CUDD>> manager(new storm::dd::DdManager<storm::dd::DdType::CUDD>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> state = manager->addMetaVariable("x", 1, 4);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> pl1 = manager->addMetaVariable("a", 0, 1);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> pl2 = manager->addMetaVariable("b", 0, 1);

    storm::dd::Bdd<storm::dd::DdType::CUDD> allRows = manager->getBddZero();
    std::set<storm::expressions::Variable> rowMetaVariables({state.first});
    std::set<storm::expressions::Variable> columnMetaVariables({state.second});
    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowColumnMetaVariablePairs = {state};
    std::set<storm::expressions::Variable> player1Variables({pl1.first});
    std::set<storm::expressions::Variable> player2Variables({pl2.first});

    // Construct simple game.
    storm::dd::Add<storm::dd::DdType::CUDD, double> matrix = manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 2).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.6);
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 1).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.4);

    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 2).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 1).template toAdd<double>() * manager->getConstant(0.2);
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 3).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 1).template toAdd<double>() * manager->getConstant(0.8);

    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 3).template toAdd<double>() * manager->getEncoding(pl1.first, 1).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.5);
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 4).template toAdd<double>() * manager->getEncoding(pl1.first, 1).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.5);
    
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 1).template toAdd<double>() * manager->getEncoding(pl1.first, 1).template toAdd<double>() * manager->getEncoding(pl2.first, 1).template toAdd<double>() * manager->getConstant<double>(1);
    
    std::unique_ptr<storm::utility::solver::SymbolicGameSolverFactory<storm::dd::DdType::CUDD, double>> solverFactory(new storm::utility::solver::SymbolicGameSolverFactory<storm::dd::DdType::CUDD, double>());
    std::unique_ptr<storm::solver::SymbolicGameSolver<storm::dd::DdType::CUDD>> solver = solverFactory->create(matrix, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, player1Variables,player2Variables);
    
    // Create solution and target state vector.
    storm::dd::Add<storm::dd::DdType::CUDD, double> x = manager->template getAddZero<double>();
    storm::dd::Add<storm::dd::DdType::CUDD, double> b = manager->getEncoding(state.first, 2).template toAdd<double>() + manager->getEncoding(state.first, 4).template toAdd<double>();
    
    // Now solve the game with different strategies for the players.
    storm::dd::Add<storm::dd::DdType::CUDD> result = solver->solveGame(storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    x = manager->getAddZero<double>();
    result = solver->solveGame(storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0.5, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    x = manager->getAddZero<double>();
    result = solver->solveGame(storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0.2, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());

    x = manager->getAddZero<double>();
    result = solver->solveGame(storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0.99999892625817599, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}

TEST(FullySymbolicGameSolverTest, Solve_Sylvan) {
    // Create some variables.
    std::shared_ptr<storm::dd::DdManager<storm::dd::DdType::Sylvan>> manager(new storm::dd::DdManager<storm::dd::DdType::Sylvan>());
    std::pair<storm::expressions::Variable, storm::expressions::Variable> state = manager->addMetaVariable("x", 1, 4);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> pl1 = manager->addMetaVariable("a", 0, 1);
    std::pair<storm::expressions::Variable, storm::expressions::Variable> pl2 = manager->addMetaVariable("b", 0, 1);
    
    storm::dd::Bdd<storm::dd::DdType::Sylvan> allRows = manager->getBddZero();
    std::set<storm::expressions::Variable> rowMetaVariables({state.first});
    std::set<storm::expressions::Variable> columnMetaVariables({state.second});
    std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowColumnMetaVariablePairs = {state};
    std::set<storm::expressions::Variable> player1Variables({pl1.first});
    std::set<storm::expressions::Variable> player2Variables({pl2.first});
    
    // Construct simple game.
    storm::dd::Add<storm::dd::DdType::Sylvan, double> matrix = manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 2).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.6);
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 1).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.4);
    
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 2).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 1).template toAdd<double>() * manager->getConstant(0.2);
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 3).template toAdd<double>() * manager->getEncoding(pl1.first, 0).template toAdd<double>() * manager->getEncoding(pl2.first, 1).template toAdd<double>() * manager->getConstant(0.8);
    
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 3).template toAdd<double>() * manager->getEncoding(pl1.first, 1).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.5);
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 4).template toAdd<double>() * manager->getEncoding(pl1.first, 1).template toAdd<double>() * manager->getEncoding(pl2.first, 0).template toAdd<double>() * manager->getConstant(0.5);
    
    matrix += manager->getEncoding(state.first, 1).template toAdd<double>() * manager->getEncoding(state.second, 1).template toAdd<double>() * manager->getEncoding(pl1.first, 1).template toAdd<double>() * manager->getEncoding(pl2.first, 1).template toAdd<double>() * manager->getConstant<double>(1);
    
    std::unique_ptr<storm::utility::solver::SymbolicGameSolverFactory<storm::dd::DdType::Sylvan, double>> solverFactory(new storm::utility::solver::SymbolicGameSolverFactory<storm::dd::DdType::Sylvan, double>());
    std::unique_ptr<storm::solver::SymbolicGameSolver<storm::dd::DdType::Sylvan>> solver = solverFactory->create(matrix, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, player1Variables,player2Variables);
    
    // Create solution and target state vector.
    storm::dd::Add<storm::dd::DdType::Sylvan, double> x = manager->template getAddZero<double>();
    storm::dd::Add<storm::dd::DdType::Sylvan, double> b = manager->getEncoding(state.first, 2).template toAdd<double>() + manager->getEncoding(state.first, 4).template toAdd<double>();
    
    // Now solve the game with different strategies for the players.
    storm::dd::Add<storm::dd::DdType::Sylvan> result = solver->solveGame(storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Minimize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    x = manager->getAddZero<double>();
    result = solver->solveGame(storm::OptimizationDirection::Minimize, storm::OptimizationDirection::Maximize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0.5, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    x = manager->getAddZero<double>();
    result = solver->solveGame(storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Minimize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0.2, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
    
    x = manager->getAddZero<double>();
    result = solver->solveGame(storm::OptimizationDirection::Maximize, storm::OptimizationDirection::Maximize, x, b);
    result *= manager->getEncoding(state.first, 1).template toAdd<double>();
    result = result.sumAbstract({state.first});
    EXPECT_NEAR(0.99999892625817599, result.getValue(), storm::settings::getModule<storm::settings::modules::NativeEquationSolverSettings>().getPrecision());
}
