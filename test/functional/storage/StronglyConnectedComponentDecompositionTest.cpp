#include "gtest/gtest.h"
#include "storm-config.h"
#include "src/storm/parser/AutoParser.h"
#include "src/storm/storage/SparseMatrix.h"
#include "src/storm/storage/StronglyConnectedComponentDecomposition.h"
#include "src/storm/models/sparse/StandardRewardModel.h"
#include "src/storm/models/sparse/MarkovAutomaton.h"

TEST(StronglyConnectedComponentDecomposition, SmallSystemFromMatrix) {
	storm::storage::SparseMatrixBuilder<double> matrixBuilder(6, 6);
	ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 0, 0.3));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 5, 0.7));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 2, 1.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 1, 0.4));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 2, 0.3));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 3, 0.3));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(3, 4, 1.0));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(4, 3, 0.5));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(4, 4, 0.5));
	ASSERT_NO_THROW(matrixBuilder.addNextValue(5, 1, 1.0));

	storm::storage::SparseMatrix<double> matrix;
	ASSERT_NO_THROW(matrix = matrixBuilder.build());
	storm::storage::BitVector allBits(6, true);

	storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;

	ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(matrix, allBits, false, false));
	ASSERT_EQ(4ul, sccDecomposition.size());

	ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(matrix, allBits, true, false));
	ASSERT_EQ(3ul, sccDecomposition.size());

	ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(matrix, allBits, true, true));
	ASSERT_EQ(1ul, sccDecomposition.size());
}

TEST(StronglyConnectedComponentDecomposition, FullSystem1) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.tra", STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny1.lab", "", "");

	std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> markovAutomaton = abstractModel->as<storm::models::sparse::MarkovAutomaton<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton));
    ASSERT_EQ(5ul, sccDecomposition.size());
    
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true));
    ASSERT_EQ(2ul, sccDecomposition.size());

    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true, true));
    ASSERT_EQ(2ul, sccDecomposition.size());
    
    markovAutomaton = nullptr;
}

TEST(StronglyConnectedComponentDecomposition, FullSystem2) {
	std::shared_ptr<storm::models::sparse::Model<double>> abstractModel = storm::parser::AutoParser<>::parseModel(STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny2.tra", STORM_CPP_BASE_PATH "/examples/ma/tiny/tiny2.lab", "", "");

	std::shared_ptr<storm::models::sparse::MarkovAutomaton<double>> markovAutomaton = abstractModel->as<storm::models::sparse::MarkovAutomaton<double>>();
    
    storm::storage::StronglyConnectedComponentDecomposition<double> sccDecomposition;
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true, false));
    
    ASSERT_EQ(2ul, sccDecomposition.size());
    
    // Now, because there is no ordering we have to check the contents of the MECs in a symmetrical way.
    storm::storage::StateBlock const& scc1 = sccDecomposition[0];
    storm::storage::StateBlock const& scc2 = sccDecomposition[1];

    storm::storage::StateBlock correctScc1 = {1, 3, 8, 9, 10};
    storm::storage::StateBlock correctScc2 = {4, 5, 6, 7};
    ASSERT_TRUE(scc1 == storm::storage::StateBlock(correctScc1.begin(), correctScc1.end()) || scc1 == storm::storage::StateBlock(correctScc2.begin(), correctScc2.end()));
    ASSERT_TRUE(scc2 == storm::storage::StateBlock(correctScc1.begin(), correctScc1.end()) || scc2 == storm::storage::StateBlock(correctScc2.begin(), correctScc2.end()));
    
    ASSERT_NO_THROW(sccDecomposition = storm::storage::StronglyConnectedComponentDecomposition<double>(*markovAutomaton, true, true));
    ASSERT_EQ(1ul, sccDecomposition.size());

    markovAutomaton = nullptr;
}
